# winhide
Allows streamers to hide sensible information, specifically when doing live [scambaiting](https://en.wikipedia.org/wiki/Scam_baiting).

Winhide consists of two parts, an OBS Studio plugin and a client program. The client
is intended to run inside a virtual machine and connects to the plugin running 
in OBS Studio somewhere on the same network. The client will frequently check all
open windows in the VM and compare them against a list of configurable programs
that could contain sensible information.

If any programs are found and they're not minimized, their position and size
is sent to the plugin which will create a rectangle to blank out the window.
This process isn't perfect, but it should take some load off of the streamer.

![screenshot](https://i.imgur.com/BruzF3Z.png)
Winhide running in a Windows 7 VM connected to OBS Studio on linux
