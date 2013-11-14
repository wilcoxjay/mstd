#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <omp.h>

int bitset_get(char* s, int i, int n) {
  assert(0 <= i && i < n);
  return (s[i / 8] >> (i % 8)) & 1;
}

void bitset_set(char *s, int i, int n) {
  assert(0 <= i && i < n);
  s[i / 8] |= 1 << (i % 8);
}

void print_set(char* s, int n) {
  printf("{");
  int i;
  int started = 0;
  for (i = 0; i < n; i++) {
    if (bitset_get(s, i, n)) {
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
    if (bitset_get(s, i, N)) {
      count++;
    }
  }

  return count;
}

int is_mstd(char* s, int N, char* sum, char* diff) {
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i, N)) continue;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j, N)) continue;
      bitset_set(sum, i+j, 2*N);
      int d = i - j;
      bitset_set(diff, d + N, 2*N);
      bitset_set(diff, N - d, 2*N);
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
    if (!bitset_get(s, i, N)) continue;
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
  
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i, N)) continue;
    int ir = i / cols;
    int ic = i % cols;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j, N)) continue;
      int jr = j / cols;
      int jc = j % cols;

      int rs = ir + jr;
      int cs = ic + jc;
      int rd1 = ir - jr + rows;
      int cd1 = ic - jc + cols;
      int rd2 = jr - ir + rows;
      int cd2 = jc - ic + cols;

      bitset_set(sum, rs * 2 * cols + cs, 4*N);
      bitset_set(diff, rd1 * 2 * cols + cd1, 4*N);
      bitset_set(diff, rd2 * 2 * cols + cd2, 4*N);
    }
  }

  return bitset_count(sum, 4*N) > bitset_count(diff, 4*N);
}


void check(long long x, int R, int C) {
  long long scratch[8];
  memset(scratch, 0, 8*sizeof(long long));

  char* sum = (char*)scratch;
  char* diff = (char*)(scratch+4);

  char* s = (char*)&x;
  long long N = R * C;

  printf("N = %lld\n", N);

  print_set2d(s, R, C);
  printf(" %d\n", bitset_count(s, N));
  print_set2d(sum, 2*R, 2*C);
  printf(" %d\n", bitset_count(sum, 4*N));
  print_set2d(diff, 2*R, 2*C);
  printf(" %d\n\n", bitset_count(diff, 4*N));

  is_mstd2d(s, R, C, sum, diff);

  print_set2d(s, R, C);
  printf(" %d\n", bitset_count(s, N));
  print_set2d(sum, 2*R, 2*C);
  printf(" %d\n", bitset_count(sum, 4*N));
  print_set2d(diff, 2*R, 2*C);
  printf(" %d\n\n\n\n", bitset_count(diff, 4*N));
}

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
    int tid = omp_get_thread_num();

    long long scratch[8];
    char* sum = (char*)scratch;
    char* diff = (char*)(scratch+4);
    long long i;

#pragma omp for schedule(guided)
    for (i = 0; i < max; i++) {
      memset(scratch, 0, 8*sizeof(long long));
    
      char* s = (char*) &i;
      if (is_mstd2d(s, R, C, sum, diff)) {
#pragma omp critical
	{
	  print_set2d(s, R, C);
	  printf(" %d\n", bitset_count(s, N));
	  print_set2d(sum, 2*R, 2*C);
	  printf(" %d\n", bitset_count(sum, 4*N));
	  print_set2d(diff, 2*R, 2*C);
	  printf(" %d\n\n", bitset_count(diff, 4*N));
	  printf("**tid %d, seed: %lld\n\n\n\n\n", tid, i);
	}
      }
    }
  }

  return 0;
}
