mstd
====

To build the 2d MSTD set finder, type `make mstd2d`. To run it after
building, type, e.g., `./mstd2d 12 2 > log` to exhaustively enumerate
all sets in the 12x2 rectangle, printing the MSTD ones. I usually pipe
this into a log for later processing. To count the sets after the
fact, say `grep seed log | wc`. The log also contains the sets
themselves, as well as their sum and difference sets so that you can
check them by hand if desired.
