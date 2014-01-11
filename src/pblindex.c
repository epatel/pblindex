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

#define NUM_LINES 6
#define COLUMN2_WIDTH 65

enum {
    KEY_NAMES  = 10,
    KEY_VALUES = 11,
    KEY_READY  = 12
};

Window *window;
TextLayer *textLayer[2][NUM_LINES];

void set_display_fail(char *text) {
    text_layer_set_text(textLayer[0][0], "Failed");
    text_layer_set_text(textLayer[0][1], text);
    for (int i=2; i<NUM_LINES; i++)
        text_layer_set_text(textLayer[0][i], "");
    for (int i=0; i<NUM_LINES; i++)
        text_layer_set_text(textLayer[1][i], "");
}

void request_list(int list) {
    Tuplet list_tuple = TupletInteger(list, 1);

    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    if (!iter) 
        return;

    dict_write_tuplet(iter, &list_tuple);
    dict_write_end(iter);

    app_message_outbox_send();
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *names_tuple = dict_find(iter, KEY_NAMES);
    Tuple *values_tuple = dict_find(iter, KEY_VALUES);
    Tuple *ready_tuple = dict_find(iter, KEY_READY);
    int li = -1;

    if (names_tuple) 
        li = 0;
    if (values_tuple) 
        li = 1;
    if (ready_tuple) {
        request_list(KEY_NAMES);
        return;
    }
    if (li < 0)
        return;
    for (int i=0; i<NUM_LINES; i++) {
        Tuple *value = dict_find(iter, i);
        if (value) {
            static char str[2][NUM_LINES][16];
            strcpy(str[li][i], value->value->cstring);
            text_layer_set_text(textLayer[li][i], str[li][i]);
        } else {
            text_layer_set_text(textLayer[li][i], "-");
        }
    }
    if (!li) {
        request_list(KEY_VALUES);
    } else {
        light_enable_interaction();
    }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
    set_display_fail("(dropped)");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
    set_display_fail("(send)");
}

static void app_message_init() {
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_failed(out_failed_handler);
    app_message_open(124, 256);
}

void handle_init() {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_stack_push(window, true);

    for (int i=0; i<NUM_LINES; i++) {
        textLayer[0][i] = text_layer_create(GRect(5, i*28, 144-5-COLUMN2_WIDTH, 28));
        textLayer[1][i] = text_layer_create(GRect(144-COLUMN2_WIDTH, i*28, COLUMN2_WIDTH, 28));
        text_layer_set_font(textLayer[0][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_font(textLayer[1][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_background_color(textLayer[0][i], GColorBlack);
        text_layer_set_background_color(textLayer[1][i], GColorBlack);
        text_layer_set_text_color(textLayer[0][i], GColorWhite);
        text_layer_set_text_color(textLayer[1][i], GColorWhite);
        text_layer_set_text_alignment(textLayer[1][i], GTextAlignmentRight);
    }

    text_layer_set_text(textLayer[0][0], "pbl-index");
    text_layer_set_text(textLayer[0][1], "by epatel");
    if (bluetooth_connection_service_peek())
        text_layer_set_text(textLayer[0][3], "loading...");
    else
        text_layer_set_text(textLayer[0][3], "not connected...");

    static char batt[8];
    snprintf(batt, 8, "%d %%", battery_state_service_peek().charge_percent);
    text_layer_set_text(textLayer[0][5], "battery");
    text_layer_set_text(textLayer[1][5], batt);

    for (int i=0; i<NUM_LINES; i++) {
        layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayer[0][i]));
        layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayer[1][i]));
    }

    app_message_init();
}

void handle_deinit() {
    for (int i=0; i<NUM_LINES; i++) {
        text_layer_destroy(textLayer[0][i]);
        text_layer_destroy(textLayer[1][i]);
    }
    window_destroy(window);
}

int main() {
    handle_init();
    app_event_loop();
    handle_deinit();
    return 0;
}
