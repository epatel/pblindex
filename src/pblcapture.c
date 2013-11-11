/* ===========================================================================
 
 Copyright (c) 2013 Edward Patel
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 =========================================================================== */

#include "pebble.h"
#include "pblcapture.h"

static bool pbl_capture_sending = false;
static int num_lines = 168;
static unsigned char pbl_capture_frameBuffer[18*168];
static int pbl_capture_sentLen;
static AppMessageOutboxFailed previousAppMessageOutboxFailed = NULL;
static AppMessageOutboxSent previousAppMessageOutboxSent = NULL;

// The *hack* to get to the framebuffer
struct GContext {
    void *ptr;
};
struct GContext *gctx = NULL;

static Layer *pbl_capture_layer = NULL;

static void capture_layer_update_callback(struct Layer *layer, GContext *ctx) {
    gctx = ctx;
}

static void pbl_capture_send_buffer(void *userdata) {
    int len = 64;

    if (pbl_capture_sentLen+len > 18*num_lines)
        len = 18*num_lines - pbl_capture_sentLen;
    if (len <= 0)
        return;

    Tuplet start = TupletInteger(1396920915, pbl_capture_sentLen+(168-num_lines)*18); // 'SCRS'
    Tuplet buf = TupletBytes(1396920900, &pbl_capture_frameBuffer[pbl_capture_sentLen], len); // 'SCRD'

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (iter == NULL)
        return;

    dict_write_tuplet(iter, &start);
    dict_write_tuplet(iter, &buf);
    dict_write_end(iter);

    pbl_capture_sentLen += len;

    app_message_outbox_send();
}

static void pbl_capture_make_framebuffer_copy() {
    if (!gctx)
        return;
    unsigned char *ptr = (unsigned char *)(gctx->ptr);
    int len = 0;
    pbl_capture_sentLen = 0;
    for (int y=0; y<168; y++) {
        for (int x=0; x<18; x++) {
            pbl_capture_frameBuffer[len++] = *ptr++;
        }
        ptr++; ptr++;
    }
}

static void pbl_capture_start(void *userdata) {
    pbl_capture_make_framebuffer_copy();
    pbl_capture_send_buffer(NULL);
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    Tuple *start_tuple = dict_find(failed, 1396920915);
    if (start_tuple) {
    } else if (previousAppMessageOutboxFailed) {
        previousAppMessageOutboxFailed(failed, reason, context);
    }
}

static void sent_handler(DictionaryIterator *sent, void *context) {
    Tuple *start_tuple = dict_find(sent, 1396920915);
    if (start_tuple) {
        if (pbl_capture_sending && pbl_capture_sentLen < 18*168)
            app_timer_register(10, pbl_capture_send_buffer, NULL);
        else
            pbl_capture_sending = false;
    } else if (previousAppMessageOutboxSent) {
        previousAppMessageOutboxSent(sent, context);
    }
}

// ----------------------------------------------------------------------
// External API

void pbl_capture_init(Window *window, bool has_msg_open) {
    if (window_get_fullscreen(window))
        num_lines = 168;
    else
        num_lines = 168-16;
    pbl_capture_layer = layer_create(GRect(0, 0, 144, num_lines));
    layer_set_update_proc(pbl_capture_layer, capture_layer_update_callback);
    layer_add_child(window_get_root_layer(window), pbl_capture_layer);
    previousAppMessageOutboxFailed = app_message_register_outbox_failed(out_failed_handler);
    previousAppMessageOutboxSent = app_message_register_outbox_sent(sent_handler);
    if (!has_msg_open)
        app_message_open(124, 256);
}

void pbl_capture_deinit() {
    if (pbl_capture_layer)
        layer_destroy(pbl_capture_layer);
    pbl_capture_layer = NULL;
}

void pbl_capture_send(int wait) {
    if (pbl_capture_sending)
        return;
    pbl_capture_sending = true;
    if (wait) {
        app_timer_register(wait, pbl_capture_start, NULL);
    } else {
        pbl_capture_make_framebuffer_copy();
        app_timer_register(10, pbl_capture_send_buffer, NULL);
    }
}
