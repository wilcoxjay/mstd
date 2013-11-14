#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bitset_get(char* s, int i) {
  //printf("(*** accessing index %d ***)\n", i);
  return (s[i / 8] >> (i % 8)) & 1;
}

void bitset_set(char *s, int i) {
  s[i / 8] |= 1 << (i % 8);
}

void bitset_clear(char* s, int i) {
  s[i / 8] &= ~(1 << (i % 8));
}

void print_set(char* s, int n) {
  printf("{");
  int i;
  int started = 0;
  for (i = 0; i < n; i++) {
    if (bitset_get(s, i)) {
      if (started) {
	printf(", ");
      }
      started = 1;
      printf("%d", i);
    }
  }
  printf("}");
}

int bitset_count(char* s, int N) {
  int count = 0;
  int i;
  for (i = 0; i < N; i++) {
    if (bitset_get(s, i)) {
      count++;
    }
  }

  return count;
}

int is_mstd(char* s, int N, char* sum, char* diff) {
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i)) continue;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j)) continue;
      bitset_set(sum, i+j);
      int d = i - j;
      bitset_set(diff, d + N);
      bitset_set(diff, N - d);
    }
  }

  int result = bitset_count(sum, 2*N) > bitset_count(diff, 2*N);

  // if (result) {
  //   print_set(s, N);
  //   printf(" %d\n", bitset_count(s, N));
  //   print_set(sum, 2*N);
  //   printf(" %d\n", bitset_count(sum, 2*N));
  //   print_set(diff, 2*N);
  //   printf(" %d\n", bitset_count(diff, 2*N));
  //   printf("\n\n");
  // }

  return result;
}

void print_set2d(char* s, int rows, int cols) {
  int N = rows * cols;
  int started = 0;
  printf("{");
  int i;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i)) continue;
    int ir = i / cols;
    int ic = i % cols;

    if (started) {
      printf(", ");
    }
    started = 1;
    printf("(%d,%d)", ir, ic);
  }
  printf("}");
}

int is_mstd2d(char* s, int rows, int cols, char* sum, char* diff) {
  int N = rows * cols;
  // printf("r = %d, c = %d, N = %d\n", rows, cols, N);
  // print_set2d(s, rows, cols);
  // printf(" %d\n", bitset_count(s, N));
  // print_set2d(sum, 2*rows, 2*cols);
  // printf(" %d\n", bitset_count(sum, 2*N));
  // print_set2d(diff, 2*rows, 2*cols);
  // printf(" %d\n\n", bitset_count(diff, 2*N));

  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i)) continue;
    int ir = i / cols;
    int ic = i % cols;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j)) continue;
      int jr = j / cols;
      int jc = j % cols;

      int rs = ir + jr;
      int cs = ic + jc;
      int rd1 = ir - jr + rows;
      int cd1 = ic - jc + cols;
      int rd2 = jr - ir + rows;
      int cd2 = jc - ic + cols;

      bitset_set(sum, rs * 2 * cols + cs);
      bitset_set(diff, rd1 * 2 * cols + cd1);
      bitset_set(diff, rd2 * 2 * cols + cd2);
    }
  }

  int result = bitset_count(sum, 4*N) > bitset_count(diff, 4*N);

  if (result) {
#pragma omp critical
    {
    print_set2d(s, rows, cols);
    printf(" %d\n", bitset_count(s, N));
    print_set2d(sum, 2*rows, 2*cols);
    printf(" %d\n", bitset_count(sum, 4*N));
    print_set2d(diff, 2*rows, 2*cols);
    printf(" %d\n\n\n\n\n", bitset_count(diff, 4*N));
    }
  }

  return result;
}



#include <omp.h>
int main(int argc, char** argv) {
  if (argc < 3) {
    printf("need R and C on the command line.\n");
    return 1;
  }
  int R = atoi(argv[1]);
  int C = atoi(argv[2]);
  int N = R * C;
  if (N > 63) {
    printf("N must be no more than 63. (got %d)\n", N);
    return 1;
  }
  long long max = 1LL << N;
#pragma omp parallel
  {
#pragma omp master
    printf("running on %d threads\n", omp_get_num_threads());
    long long scratch[4];
    long long i;
#pragma omp for schedule(dynamic,1)
    for (i = 0; i < max; i++) {
      memset(scratch, 0, 4*sizeof(long long));
    
      char* s = (char*) &i;
      if (is_mstd2d(s, R, C, (char*)scratch, (char*)(scratch+2))) {
#pragma omp critical
	printf("**seed: %lld\n\n\n", i);
	//print_set2d(s, R, C); printf("\n");
      }

    }
  }
  return 0;
}
