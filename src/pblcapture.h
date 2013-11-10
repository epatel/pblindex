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

 ---------------------------------------------------------------------------
 Add this part in [pebble-js-app.js]

 Pebble.addEventListener("appmessage",
                         function(e) {
                           if (e.payload[1396920900]) { // 'SCRD'
                             var req = new XMLHttpRequest();
                             req.open('POST', "http://127.0.0.1:9898", true);
                             req.send(JSON.stringify(e.payload));
                           }
                         });

 =========================================================================== */

// Call this to setup screen capture handling
void pbl_capture_init(Window *window, bool has_msg_open);

// Call this when deiniting the app
void pbl_capture_deinit();

// Call this to make a screen capture
void pbl_capture_send(int wait); // in milliseconds
