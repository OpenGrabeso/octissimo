How to use
==========

When you run the application, it places itself in the system tray and shows the GitHub services status.
You may use Login to enter your GitHub token to receive GitHub notifications as SysTray / Windows notifications.


Developer notes
===============

The app was built with mingw-w64 from CLion. 

I have tested from CLion it cannot be built with mingw (missing Winhttp.h) and it cannot be built with Visual Studio 14
(json.hpp does not compile).

It may be possible to user other MinGW versions or other toolchains,
but I did not test that.
