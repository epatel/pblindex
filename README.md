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

## pbl capture ##

Branch `capture` contain a version that takes a screenshot and sends it to [pbl capture](https://github.com/epatel/pblcapture)
