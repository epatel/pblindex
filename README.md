This project is a watch app for the Pebble. It can fetch and display some online information on the Pebble with the help of httpebble. 

See [http://memention.com/blog/2013/07/12/Throwing-pebbles.html](http://memention.com/blog/2013/07/12/Throwing-pebbles.html)

Setup and build with

<pre>
PEBBLE_SDK=~/pebble-dev/PebbleSDK-1.12/Pebble

$PEBBLE_SDK/tools/create_pebble_project.py --symlink-only $PEBBLE_SDK/sdk/ .

./waf configure
./waf build
</pre>

But don't forget to edit source file `pblindex.c` (see `#error`'s) for personalized use.

### http capture ###

This version has a compile option to take a screenshot and sends it to [httpebble-ios](https://github.com/epatel/httpebble-ios) (my modified version of [httpebble-ios](https://github.com/Katharine/httpebble-ios)). Enable it by setting `#define MAKE_SCREENSHOT 1` in `pblindex.c`

The API is very simple. There is a two step setup, and a function to capture the screen.

```c
// Setup step #1 - Call this is in pbl_main()
void http_capture_main(PebbleAppHandlers *handlers);

// Setup step #2 - Call this in the init_handler
void http_capture_init(AppContextRef app_ctx);

// Call this when you want to capture a screenshot
void http_capture_send(int wait); // in milliseconds
```

Another example can be find [here](https://github.com/epatel/pebble-robotoweather/tree/http_capture), pebble robotoweather with added screenshot calls.
