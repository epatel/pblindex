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

#include "pblcapture.h"

PBL_APP_INFO(PBL_CAPTURE_UUID,
             "pbl-index", "Edward Patel",
             1, 0,
             RESOURCE_ID_WATCH_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define NUM_LINES 5
#define COLUMN2_WIDTH 65

Window window;
TextLayer textLayer[2][NUM_LINES];

void timer_handler(AppContextRef app_ctx, AppTimerHandle handle, uint32_t cookie) {
	pbl_capture_send();
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

	text_layer_set_text(&textLayer[0][0], "OMXS30");
	text_layer_set_text(&textLayer[0][1], "Dow Jones");
	text_layer_set_text(&textLayer[0][2], "Nasdaq");
	text_layer_set_text(&textLayer[0][3], "DAX");
	text_layer_set_text(&textLayer[0][4], "Nikkei");
	text_layer_set_text(&textLayer[1][0], "+0,30 %");
	text_layer_set_text(&textLayer[1][1], "-0,03 %");
	text_layer_set_text(&textLayer[1][2], "-0,66 %");
	text_layer_set_text(&textLayer[1][3], "-0,07 %");
	text_layer_set_text(&textLayer[1][4], "-1,48 %");
    
    for (int i=0; i<NUM_LINES; i++) {
        layer_add_child(&window.layer, &textLayer[0][i].layer);
        layer_add_child(&window.layer, &textLayer[1][i].layer);
    }

	prepare_pbl_capture_init(ctx);

	app_timer_send_event(ctx, 1000, 1);
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &init_handler,
		.timer_handler = timer_handler,
    };
    
	prepare_pbl_capture_main(&handlers);
	
    app_event_loop(params, &handlers);
}
