This project is a watch app for the Pebble. It can fetch and display some online information on the Pebble with the help of httpebble. 

See [http://memention.com/blog/2013/07/12/Throwing-pebbles.html](http://memention.com/blog/2013/07/12/Throwing-pebbles.html)

Setup and build with

<pre>
pebble build
</pre>

But don't forget to edit source file `pblindex.c` (see `#error`'s) for personalized use.

### http capture ###

This version includes the possibility to take a screenshot and sends it to [httpebble-ios](https://github.com/epatel/httpebble-ios) (my modified version of [httpebble-ios](https://github.com/Katharine/httpebble-ios)). 

The API is very simple. There is a two step setup, and a function to capture the screen.

```c
// Call this to setup screen capture handling
void http_capture_set_window(Window *window);

// Call this when deiniting the app
void http_capture_deinit();

// Call this to force a screen capture from the app
void http_capture_send(int wait); // in milliseconds
```
