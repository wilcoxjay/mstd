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

void parallel_exhaustive_search(int R, int C) {
  int N = R * C;
  long long max = 1LL << N;
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    #pragma omp single
    {
      printf("running on %d threads\n", omp_get_num_threads());
    }

    #pragma omp critical
    {
      printf("tid %d starting up.\n", tid);
    }

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

#pragma omp critical
    {
      printf("tid %d done.\n", tid);
    }
  }
}

void do_stack(int tid, int R, int C, int N, long long s, char* p_sum, char* p_diff, int i) {
  if (i >= N) {
    if (bitset_count(p_sum, 4*N) > bitset_count(p_diff, 4*N)) {
#pragma omp critical
      {
	printf("do_stack: N=%d, i=%d, s=%lld\n", N, i, s);
	print_set2d((char*)&s, R, C);
	printf(" %d\n", bitset_count((char*)&s, N));
	print_set2d(p_sum, 2*R, 2*C);
	printf(" %d\n", bitset_count(p_sum, 4*N));
	print_set2d(p_diff, 2*R, 2*C);
	printf(" %d\n", bitset_count(p_diff, 4*N));
	printf("**tid %d, seed: %lld\n\n", tid, s);
      }
    }
  } else {
    long long scratch[8];
    memcpy(scratch, p_sum, 4 * sizeof(long long));
    memcpy(scratch + 4, p_diff, 4 * sizeof(long long));
    char* sum = (char*)scratch;
    char* diff = (char*)(scratch + 4);
    do_stack(tid, R, C, N, s, sum, diff, i+1);

    int ir = i / C;
    int ic = i % C;

    s |= (1LL << i);

    int j;
    for (j = 0; j < N; j++) {
      if (!bitset_get((char*)&s, j, N)) continue;
      int jr = j / C;
      int jc = j % C;

      int sr = ir + jr;
      int sc = ic + jc;
      int dr1 = ir - jr + R;
      int dc1 = ic - jc + C;
      int dr2 = jr - ir + R;
      int dc2 = jc - ic + C;
      
      bitset_set(sum, sr * 2 * C + sc, 4*N);
      bitset_set(diff, dr1 * 2 * C + dc1, 4*N);
      bitset_set(diff, dr2 * 2 * C + dc2, 4*N);
    }

    do_stack(tid, R, C, N, s, sum, diff, i+1);
    
  }    
}

void stack_search(int R, int C) {
  int N = R * C;

  long long scratch[8];
  memset(scratch, 0, 8 * sizeof(long long));

  do_stack(0, R, C, N, 0, (char*)scratch, (char*)(scratch + 4), 0);
}

void parallel_stack_search(int R, int C) {
  int N = R * C;

#pragma omp parallel
  {
    int tid = omp_get_thread_num();
#pragma omp critical
    printf("tid %d starting up.\n", tid);

    long long scratch[8];
    char* sum = (char*)scratch;
    char* diff = (char*)(scratch+4); 

    long long max = 1LL << (N/3);
#pragma omp single
    printf("using mask of size %d\n", N/3);
    long long i;
#pragma omp for schedule(guided), nowait
    for (i = 0; i < max; i++) {
#pragma omp critical
      printf("tid %d beginning mask %lld\n", tid, i);
      memset(scratch, 0, 8 * sizeof(long long));

      // compute the sum and difference sets as a side effect.
      is_mstd2d((char*)&i, R, C, (char*)scratch, (char*)(scratch+4));

      do_stack(tid, R, C, N, i, sum, diff, N/3);
#pragma omp critical      
      printf("tid %d ending mask %lld\n", tid, i);
    }
#pragma omp critical
    printf("tid %d done.\n", tid);
  }
}


int main(int argc, char** argv) {
  if (argc < 3) {
    printf("need R and C on the command line.\n");
    return 1;
  }
  int R = atoi(argv[1]);
  int C = atoi(argv[2]);
  if (R * C > 63) {
    printf("R * C must be no more than 63. (got %d)\n",  R * C);
    return 1;
  }

  //parallel_exhaustive_search(R, C);
  //stack_search(R, C);
  parallel_stack_search(R, C);

  return 0;
}
