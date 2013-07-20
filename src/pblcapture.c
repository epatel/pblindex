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

#include "pebble_os.h"
#include "pebble_app.h"

#include "pblcapture.h"

#define PBL_CAPTURE_COOKIE 0x70626c63 // 'pblc'

static void *pbl_capture_app_context_ref;
static bool pbl_capture_callbacks_registered;
static AppMessageCallbacksNode pbl_capture_app_callbacks;
static bool pbl_capture_sending = false;
static unsigned char pbl_capture_frameBuffer[18*168];
static int pbl_capture_sentLen;
static void (*pbl_capture_timer_next_handler)(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie);

static void pbl_capture_send_buffer() {
	int len = 64;
	if (pbl_capture_sentLen+len > 18*168)
		len = 18*168 - pbl_capture_sentLen;
	if (len <= 0)
		return;
		
	Tuplet cmd = TupletInteger(0, 1);
	Tuplet start = TupletInteger(1000, pbl_capture_sentLen);
	Tuplet buf = TupletBytes(1001, &pbl_capture_frameBuffer[pbl_capture_sentLen], len);

	DictionaryIterator *iter;
	app_message_out_get(&iter);

	if (iter == NULL)
		return;

	dict_write_tuplet(iter, &cmd);
	dict_write_tuplet(iter, &start);
	dict_write_tuplet(iter, &buf);
	dict_write_end(iter);

	pbl_capture_sentLen += len;

	app_message_out_send();
	app_message_out_release();	
}

static void pbl_capture_out_sent(DictionaryIterator *sent, void *context) {
	if (pbl_capture_sentLen < 18*168)
		app_timer_send_event(pbl_capture_app_context_ref, 10, PBL_CAPTURE_COOKIE);
	else
		pbl_capture_sending = false;
}
 
static void pbl_capture_out_failed(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	// TODO: Add error handling
}
 
static void pbl_capture_in_received(DictionaryIterator *received, void *context) {
	// TODO: Add error handling
}
 
static void pbl_capture_in_dropped(void *context, AppMessageResult reason) {	
	// TODO: Add error handling
}

static bool pbl_capture_register_callbacks() {
	if (pbl_capture_callbacks_registered) {
		if (app_message_deregister_callbacks(&pbl_capture_app_callbacks) == APP_MSG_OK)
			pbl_capture_callbacks_registered = false;
	}
	if (!pbl_capture_callbacks_registered) {
		pbl_capture_app_callbacks = (AppMessageCallbacksNode) {
			.callbacks = {
				.out_sent = pbl_capture_out_sent,
				.out_failed = pbl_capture_out_failed,
				.in_received = pbl_capture_in_received,
				.in_dropped = pbl_capture_in_dropped
			},
			.context = NULL
		};
		if (app_message_register_callbacks(&pbl_capture_app_callbacks) == APP_MSG_OK) {
			pbl_capture_callbacks_registered = true;
		}
	}
	return pbl_capture_callbacks_registered;
}

static void pbl_capture_timer_handler(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie) {
	if (cookie == PBL_CAPTURE_COOKIE)
		pbl_capture_send_buffer();
	else if (pbl_capture_timer_next_handler)
		pbl_capture_timer_next_handler(app_ctx, handle, cookie);
}

// ----------------------------------------------------------------------
// External API

// The *hack* to get to the framebuffer
struct GContext {
	void **ptr;
};

void pbl_capture_send() {
	if (pbl_capture_sending)
		return;
	struct GContext *gctx = app_get_current_graphics_context();
	unsigned char *ptr = (unsigned char *)(*gctx->ptr);
	int len = 0;
	pbl_capture_sentLen = 0;
	for (int y=0; y<168; y++) {
		for (int x=0; x<18; x++) {
			pbl_capture_frameBuffer[len++] = *ptr++;
		}
		ptr++; ptr++;
	}
	pbl_capture_sending = true;
	pbl_capture_send_buffer();
}

void prepare_pbl_capture_init(AppContextRef app_ctx) {
	pbl_capture_app_context_ref = app_ctx;
	pbl_capture_register_callbacks();
}

void prepare_pbl_capture_main(PebbleAppHandlers *handlers) {
	handlers->messaging_info.buffer_sizes.inbound = 124;
	handlers->messaging_info.buffer_sizes.outbound = 256;
	pbl_capture_timer_next_handler = handlers->timer_handler;
	handlers->timer_handler = pbl_capture_timer_handler;
}
