# About the gui\_xxx.c and gui\_xxx.h files

These files contain the handlers for search results page. For simplicity,
mostly the same logic is used to construct all the search results 
pages: albums, tracks, genres, etc.

The basic entry point for building a web page that contains search
results is to call `gui_get_results_page()` or 
`gui_get_limited_results_page()` in `gui.c`. The difference between these
two methds is that the former can display full navigation controls to
page through a large set of results; the latter only displays the first
few results. 

Both these functions take a large number of paramters that define what to
search. They also 

Each of the `gui_xxx.c` modules calls `gui_get_xxx()` with the parameters
apppropriate for the specific data type. Prototypes for the relevant
function are in `gui_xxx.h`.

Also in `gui_xxx.h` are the prototypes for the functions that will be
called to emit a specific database row for a particular data type.
So, fo example, to display a particular track, `gui_get_results_page()`
calls `gui_tracks_track_cell()`. The function `gui_get_results_page()`
takes as one of its paramters a pointer to this row-emitting function.

This complexity results from the fact that, although the search logic
is essentially indendent of whether we're searching for albums, genres,
or whatever, the data does have ot be displayed differently in each case.

