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
#include "pebble_fonts.h"

#include "resource_ids.auto.h"

#include "http.h"
#include "httpcapture.h"

PBL_APP_INFO(HTTP_UUID,
             "pbl-index", "Edward Patel",
             1, 0,
             RESOURCE_ID_WATCH_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define NUM_LINES 5
#define COLUMN2_WIDTH 65

#error Set values below, should be different, one unique for each request
#define PBLINDEX_NAMES_COOKIE some-unique-value
#define PBLINDEX_VALUES_COOKIE some-unique-value

Window window;
TextLayer textLayer[2][NUM_LINES];

void set_display_fail(char *text) {
    text_layer_set_text(&textLayer[0][0], "Failed");
    text_layer_set_text(&textLayer[0][1], text);
    for (int i=2; i<NUM_LINES; i++)
        text_layer_set_text(&textLayer[0][i], "");
    for (int i=0; i<NUM_LINES; i++)
        text_layer_set_text(&textLayer[1][i], "");
}

void request_names() {
    DictionaryIterator *body;
#error Set URL below
    // http://nnnn/nnnn.names should return something like (not over 76 bytes)
    // {"0":"OMXS30","1":"Dow Jones","2":"Nasdaq","3":"DAX","4":"Nikkei"}
    if (http_out_get("http://nnnn/nnnn.names", false, PBLINDEX_NAMES_COOKIE, &body) != HTTP_OK ||
        http_out_send() != HTTP_OK) {
        set_display_fail("names..");
    }
}

void request_values() {
    DictionaryIterator *body;
#error Set URL below
    // http://nnnn/nnnn.values should return something like (not over 76 bytes)
    // {"0":"1210","1":"15464","2":"3600","3":"8212","4":"14506"}
    if (http_out_get("http://nnnn/nnnn.values", false, PBLINDEX_VALUES_COOKIE, &body) != HTTP_OK ||
        http_out_send() != HTTP_OK) {
        set_display_fail("values..");
    }
}

void failed(int32_t cookie, int http_status, void *ctx) {
    if (cookie == 0 ||
        cookie == PBLINDEX_NAMES_COOKIE ||
        cookie == PBLINDEX_VALUES_COOKIE) {
        set_display_fail("failed()");
    }
}

void success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {
    if (cookie != PBLINDEX_NAMES_COOKIE && cookie != PBLINDEX_VALUES_COOKIE)
        return;
    int li = cookie == PBLINDEX_NAMES_COOKIE ? 0 : 1;
    for (int i=0; i<NUM_LINES; i++) {
        Tuple *value = dict_find(dict, i);
        if (value) {
            static char str[2][NUM_LINES][16];
            strcpy(str[li][i], value->value->cstring);
            text_layer_set_text(&textLayer[li][i], str[li][i]);
        } else {
            text_layer_set_text(&textLayer[li][i], "-");
        }
    }
    if (!li) {
        request_values();
    } else {
        light_enable_interaction();
		http_capture_send(20);
	}
}

void reconnect(void *ctx) {
    request_names();
}

void init_handler(AppContextRef ctx) {
    window_init(&window, "pbl-index");
    window_set_background_color(&window, GColorBlack);
    window_stack_push(&window, true);
    
    for (int i=0; i<NUM_LINES; i++) {
        text_layer_init(&textLayer[0][i], GRect(5, 7+i*30, 144-5-COLUMN2_WIDTH, 30));
        text_layer_init(&textLayer[1][i], GRect(144-COLUMN2_WIDTH, 7+i*30, COLUMN2_WIDTH, 30));
        text_layer_set_font(&textLayer[0][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_font(&textLayer[1][i], fonts_get_system_font(FONT_KEY_GOTHIC_24));
        text_layer_set_background_color(&textLayer[0][i], GColorBlack);
        text_layer_set_background_color(&textLayer[1][i], GColorBlack);
        text_layer_set_text_color(&textLayer[0][i], GColorWhite);
        text_layer_set_text_color(&textLayer[1][i], GColorWhite);
        text_layer_set_text_alignment(&textLayer[1][i], GTextAlignmentRight);
    }
    
    text_layer_set_text(&textLayer[0][0], "pbl-index");
    text_layer_set_text(&textLayer[0][1], "by epatel");
    text_layer_set_text(&textLayer[0][3], "loading...");
    
    for (int i=0; i<NUM_LINES; i++) {
        layer_add_child(&window.layer, &textLayer[0][i].layer);
        layer_add_child(&window.layer, &textLayer[1][i].layer);
    }
    
    http_set_app_id(PBLINDEX_NAMES_COOKIE);
    
    http_register_callbacks((HTTPCallbacks){
        .failure = failed,
        .success = success,
        .reconnect = reconnect,
    }, NULL);

	http_capture_init(ctx);
	
	request_names();
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &init_handler,
        .messaging_info = {
            .buffer_sizes = {
                .inbound = 124, // 124 safe for Android
                .outbound = 256,
            }
        },
    };
    
	http_capture_main(&handlers);
	
    app_event_loop(params, &handlers);
}
