This directory contains files needed to be able to create a reaper extension.

## reaper_plugin_functions.h
This file is created by reaper and tells our code, which functions are available.
The file included is based on Reaper 6.52.

You can save this file which exposes your current Reaper version's functions by executing the "[developer] Write C++ API functions header" action and choosing where to save the gerenated file.
In our case you should select reaper_plugin_functions.h in this directory.

## res.rc
A resource file describing the needed UI elements for your extension under windows environments.

Since this skeleton does not provide any UI features you should probably ignore this file.

## res.rc_mac_dlg
Same as res.rc but for linux and mac os environments.