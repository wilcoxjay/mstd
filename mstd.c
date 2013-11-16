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

int is_mstd1d(char* s, int N, char* sum, char* diff) {
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i, N)) continue;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j, N)) continue;
      bitset_set(sum, i+j, 2*N);
      int d = i - j;
      bitset_set(diff, d + N - 1, 2*N);
      bitset_set(diff, N - d - 1, 2*N);
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

void parallel_exhaustive_search1d(int N) {
  long long max = 1LL << N;
#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    #pragma omp single
    {
      printf("running on %d threads\n", omp_get_num_threads());
    }

    long long scratch[4];
    char* sum = (char*)scratch;
    char* diff = (char*)(scratch + 2);
    long long i;
#pragma omp for schedule(guided), nowait
    for (i = 0; i < max; i++) {
      memset(scratch, 0, 4*sizeof(long long));

      char* s = (char*)&i;
      if (is_mstd1d(s, N, sum, diff)) {
#pragma omp critical
	{
	  print_set(s, N);
	  printf(" %d\n", bitset_count(s, N));
	  print_set(sum, 2*N);
	  printf(" %d\n", bitset_count(sum, 2*N));
	  print_set(diff, 2*N);
	  printf(" %d\n", bitset_count(diff, 2*N));
	  printf("tid %d, seed %lld\n\n", tid, i);
	}
      }
    }
  }
}

void parallel_exhaustive_search2d(int R, int C) {
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

#pragma omp for schedule(guided), nowait
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

void do_stack1d(int tid, int N, long long s, long long r, char* p_sum, char* p_diff, int i) {
  char* t = (char*)&s;
  if (i >= N) {
    //printf("do_stack1d: N=%d, i=%d, s=%lld, r=%lld\n", N, i, s, r);
    if (bitset_count(p_sum, 2*N) > bitset_count(p_diff, 2*N)) {
#pragma omp critical
      {
	// print_set(t, N);
	// printf(" %d\n", bitset_count(t, N));
	// print_set((char*)&r, N);
	// printf(" %d\n", bitset_count((char*)&r, N));
	// print_set(p_sum, 2*N);
	// printf(" %d\n", bitset_count(p_sum, 2*N));
	// print_set(p_diff, 2*N);
	// printf(" %d\n", bitset_count(p_diff, 2*N));

	printf("seed:%lld\n", s);
      }
    }
  } else {
    long long scratch[4];
    char *sum = (char*) scratch;
    char *diff = (char*) (scratch + 2);
    memcpy(sum, p_sum, 2*sizeof(long long));
    memcpy(diff, p_diff, 2*sizeof(long long));

    if (i < N - 1) {
      do_stack1d(tid, N, s, r, sum, diff, i+1);
    }

    s |= 1LL << i;
    ////
    r |= 1LL << (N - i - 1);

    scratch[0] |= s << i;
    if (i > 0) {
      scratch[1] |= s >> (64 - i);
    }

    scratch[2] |= s << (N - i - 1);
    if (N - i - 1 > 0) {
      scratch[3] |= s >> (64 - N + i + 1);
    }

    scratch[2] |= r << i;
    if (i > 0) {
      scratch[3] |= r >> (64 - i);
    }
    //// the following is the straightforward version of the above:
    // int j;
    // for (j = 0; j < N; j++) {
    //   if (!bitset_get(t, j, N)) continue;
    //
    //   int s = i + j;
    //   int d1 = i - j + N;
    //   int d2 = j - i + N;
    //
    //   bitset_set(sum, s, 2*N);
    //   bitset_set(diff, d1, 2*N);
    //   bitset_set(diff, d2, 2*N);
    // }
    ////

    do_stack1d(tid, N, s, r, sum, diff, i+1);
  }
}

void stack_search1d(int N) {
  long long scratch[4];
  memset(scratch, 0, 4 * sizeof(long long));

  do_stack1d(0, N, 0, 0, (char*)scratch, (char*)(scratch + 2), 0);
}

long long flip_about(int N, long long x) {
  long long result = 0;
  int i;
  for (i = 0; i < N; i++) {
    result |= ((x >> i) & 1) << (N - i - 1);
  }
  return result;
}

void parallel_stack_search1d(int N) {

#pragma omp parallel
  {
    int tid = omp_get_thread_num();

    long long scratch[4];

#pragma omp critical
    printf("tid %d starting up.\n", tid);

    long long max = 1LL << (N/3);
#pragma omp single
    printf("using mask of size %d\n", N/3);

    long long i;
#pragma omp for schedule(guided), nowait
    for (i = 1; i < max; i += 2) {
      //#pragma omp critical
      //printf("tid %d beginning mask %lld\n", tid, i);
      memset(scratch, 0, 4 * sizeof(long long));

      is_mstd1d((char*)&i, N, (char*)scratch, (char*)(scratch + 2));

      do_stack1d(tid, N, i, flip_about(N, i), (char*)scratch, (char*)(scratch + 2), N/3);

      //#pragma omp critical
      //printf("tid %d ending mask %lld\n", tid, i);
    }
#pragma omp critical
    printf("tid %d done.\n", tid);
  }
}

void do_stack2d(int tid, int R, int C, int N, long long s, char* p_sum, char* p_diff, int i) {
  if (i >= N) {
    if (bitset_count(p_sum, 4*N) > bitset_count(p_diff, 4*N)) {
#pragma omp critical
      {
	printf("do_stack2d: N=%d, i=%d, s=%lld\n", N, i, s);
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
    do_stack2d(tid, R, C, N, s, sum, diff, i+1);

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

    do_stack2d(tid, R, C, N, s, sum, diff, i+1);

  }
}

void stack_search2d(int R, int C) {
  int N = R * C;

  long long scratch[8];
  memset(scratch, 0, 8 * sizeof(long long));

  do_stack2d(0, R, C, N, 0, (char*)scratch, (char*)(scratch + 4), 0);
}

void parallel_stack_search2d(int R, int C) {
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

      do_stack2d(tid, R, C, N, i, sum, diff, N/3);
#pragma omp critical
      printf("tid %d ending mask %lld\n", tid, i);
    }
#pragma omp critical
    printf("tid %d done.\n", tid);
  }
}


void oned(int argc, char** argv) {
  if (argc < 2) {
    printf("need N on the command line.\n");
    exit(1);
  }
  int N = atoi(argv[1]);
  if (N > 63) {
    printf("N must be no more than 63. (got %d)\n",  N);
    exit(1);
  }

  //parallel_exhaustive_search1d(N);
  //stack_search1d(N);
  parallel_stack_search1d(N);

}

void twod(int argc, char** argv) {
  if (argc < 3) {
    printf("need R and C on the command line.\n");
    exit(1);
  }
  int R = atoi(argv[1]);
  int C = atoi(argv[2]);
  if (R * C > 63) {
    printf("R * C must be no more than 63. (got %d)\n",  R * C);
    exit(1);
  }

  //parallel_exhaustive_search2d(R, C);
  //stack_search2d(R, C);
  parallel_stack_search2d(R, C);

}


int main(int argc, char** argv) {
  //twod(argc, argv);
  oned(argc, argv);

  return 0;
}
