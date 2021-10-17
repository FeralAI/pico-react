# Pico React

A sample application for RP2040 MCUs for hosting a React site.

## Requirements

* PlatformIO with the [Wiz-IO Pico](https://github.com/Wiz-IO/wizio-pico) platform module
* NodeJS and NPM to build the React app
* Python for the React app build script
* Perl for some text replacement done by the React app build script

## Development

There are two configured environments for PlatformIO: `rp2040-react` and `rp2040-react-cached`. The `rp2040-react` environment will execute the `build-web-py` script on each build, which can be a long running process. If the content of your React app hasn't changed, switch over to the `rp2040-react-cached` environment to only build the C++ source code and use the current `fsdata.c` file. You can also manually run the `build-web.py` script to update build the React app and regenerate the `lib/httpd/fsdata.c` file. Also of note, `makefsdata` doesn't set the correct `#include` lines for our use. This script will fix this issue.

The lwIP lib in Wiz-IO Pico PlatformIO platform doesn't include the `makefsdata` source. A precompiled version for Windows and Unix are included in the `tools` folder.

Use JPG and PNG files for images, SVG file support requires modification that I haven't done.

## References

Original example:

* <https://forums.raspberrypi.com/viewtopic.php?t=306888>
* <https://github.com/maxnet/pico-webserver>

WizIO example:

* <https://github.com/Wiz-IO/wizio-pico-examples/tree/main/baremetal/pico-usb-webserver>

Convert text to bytes:

* <https://onlineasciitools.com/convert-ascii-to-bytes>
* <https://onlineasciitools.com/convert-bytes-to-ascii>

Create image map:

* <https://www.image-map.net/>
