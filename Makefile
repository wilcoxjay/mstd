mstd2d: mstd.c
	gcc -DMSTD_2D -DMSTD_2D_PAR_STACK -fopenmp -O3 -march=native -mtune=native -Wall -Wextra -o mstd2d mstd.c

mstd1d: mstd.c
	gcc -DMSTD_1D -DMSTD_1D_PAR_STACK -fopenmp -O3 -march=native -mtune=native -Wall -Wextra -o mstd1d mstd.c
