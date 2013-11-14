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

function fetchList(list, url) {
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        response = JSON.parse(req.responseText);
        if (response) {
          response[list] = 1;
          Pebble.sendAppMessage(response);
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  }
  req.send(null);
}

Pebble.addEventListener("ready",
                        function(e) {
                          Pebble.sendAppMessage({"ready":1});
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          if (e.payload.names) {
                            // http://nnnn/nnnn.names should return something like
                            // {"0":"OMXS30","1":"Dow Jones","2":"Nasdaq","3":"DAX","4":"Nikkei"}
                            fetchList("names", "http://nnnn/nnnn.names");
                          }
                          if (e.payload.values) {
                            // http://nnnn/nnnn.values should return something like
                            // {"0":"1210","1":"15464","2":"3600","3":"8212","4":"14506"}
                            fetchList("values", "http://nnnn/nnnn.values");
                          }
                        });
