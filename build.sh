#g++ -fPIC -shared -I"./WDL/swell" reaper-plugins/reaper_csurf/csurf_main.cpp reaper-plugins/reaper_csurf/csurf_faderport.cpp ./WDL/swell/swell-modstub-generic.cpp -std=c++11 -o reaper_witti.so -DSWELL_PROVIDED_BY_APP

g++ -fPIC -shared -I"./WDL/swell"src/reaper_witti.cpp src/csurf_novation_slmk3.cpp ./WDL/swell/swell-modstub-generic.cpp -std=c++11 -o reaper_witti.so -DSWELL_PROVIDED_BY_APP | head
