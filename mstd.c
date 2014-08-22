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

int is_restricted_mstd1d(char* s, int N, char* sum, char* diff) {
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i, N)) continue;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j, N)) continue;
      if (i != j) bitset_set(sum, i+j, 2*N);
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

void init_sumdiff(char* s, int N, char* sum, char* diff) {
  int i, j;
  for (i = 0; i < N; i++) {
    if (!bitset_get(s, i, N)) continue;
    for (j = i; j < N; j++) {
      if (!bitset_get(s, j, N)) continue;
      bitset_set(sum, i+j, 2*N);
      int d = i - j;
      bitset_set(diff, d + N, 2*N);
    }
  }
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

void parallel_exhaustive_search1d_restricted(int N) {
  long long max = 1LL << N;
  printf("executing 1d naive parallel search for restricted MSTD sets\n");

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
      if (is_restricted_mstd1d(s, N, sum, diff)) {
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

long long longset_count(long long s) {
  long long result;
  asm("popcnt %1, %0"
      : "=r" (result)
      : "r" (s)
      );
  return result;
}

long long do_stack1d(int tid, int N, long long s, char* p_sum, char* p_diff, int i) {
  char* t = (char*)&s;
  if (i >= N) {
    long long* scratch = (long long *) p_sum;
    return
      ((longset_count(scratch[0]) + longset_count(scratch[1])) >
       2 * longset_count(scratch[2]) - 1);

    //return bitset_count(p_sum, 2*N) > bitset_count(p_diff, 2*N);

    //printf("do_stack1d: N=%d, i=%d, s=%lld, r=%lld\n", N, i, s, r);
    // if (bitset_count(p_sum, 2*N) > bitset_count(p_diff, 2*N)) {
    //   //#pragma omp critical
    //   // {
    //   // 	 print_set(t, N);
    //   // 	 printf(" %d\n", bitset_count(t, N));
    //   // 	 print_set((char*)&r, N);
    //   // 	 printf(" %d\n", bitset_count((char*)&r, N));
    //   // 	 print_set(p_sum, 2*N);
    //   // 	 printf(" %d\n", bitset_count(p_sum, 2*N));
    //   // 	 print_set(p_diff, 2*N);
    //   // 	 printf(" %d\n", bitset_count(p_diff, 2*N));
    //   // 
    //   // 	printf("seed:%lld\n", s);
    //   // }
    //   // return 1;
    // }
    // return 0;
  } else {
    long long scratch[3];
    char *sum = (char*) scratch;
    char *diff = (char*) (scratch + 2);
    memcpy(sum, p_sum, 2*sizeof(long long));
    memcpy(diff, p_diff, sizeof(long long));

    long long result = 0;
    if (i < N - 1) {
      // we require the last element to be in the set, cf. ".... += 2"
      // (requiring first element to be in set) in for loop in
      // parallel_stack_search1d
      result = do_stack1d(tid, N, s, sum, diff, i+1);
    }


    s |= 1LL << i;

    scratch[0] |= s << i;
    if (i > 0) {
      scratch[1] |= s >> (64 - i);
    }

    scratch[2] |= (s << (N - i));


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

    result += do_stack1d(tid, N, s, sum, diff, i+1);
    return result;
  }
}

long long flip_about(int N, long long x) {
  long long result = 0;
  int i;
  for (i = 0; i < N; i++) {
    result |= ((x >> i) & 1) << (N - i - 1);
  }
  return result;
}


void stack_search1d(int N) {
  long long scratch[4];
  memset(scratch, 0, 4 * sizeof(long long));

  scratch[0] = 1;
  scratch[2] = 1LL << N;

  // complicated by fact that we require first element to be in set...
  long long result = do_stack1d(0, N, 1, (char*)scratch, (char*)(scratch + 2), 1);

  printf("%lld\n", result);
}



void parallel_stack_search1d(int N) {

  long long global_result = 0;

#pragma omp parallel
  {
    int tid = omp_get_thread_num();

    long long result = 0;

    long long scratch[4];

#pragma omp critical
    printf("tid %d starting up.\n", tid);

    long long max = 1LL << (N/3);
#pragma omp single
    printf("using mask of size %d\n", N/3);

    long long mod = max / 1000;
    if (mod == 0) {
      mod = 1;
    }

    long long i;
#pragma omp for schedule(guided), nowait
    for (i = 1; i < max; i += 2) {
      // += 2 because we require the 0th bit to be set, ie, we require the first element to be in the set.


      // if ((i/2) % (max / 100) == 0) {
      // 	//#pragma omp critical
      // 	printf("tid %d beginning mask %lld\n", tid, i);
      // }
      memset(scratch, 0, 4 * sizeof(long long));

      //is_mstd1d((char*)&i, N, (char*)scratch, (char*)(scratch + 2));
      init_sumdiff((char*)&i, N, (char*)scratch, (char*)(scratch + 2));

      result += do_stack1d(tid, N, i, (char*)scratch, (char*)(scratch + 2), N/3);

      //#pragma omp critical
      //printf("tid %d ending mask %lld\n", tid, i);
    }
#pragma omp critical
    {
      printf("tid %d done. found %lld MSTD sets.\n", tid, result);
      global_result += result;
    }
  }

  printf("total: %lld\n", global_result);
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

#if defined(MSTD_1D_SEQ)
  stack_search1d(N);
#elif defined(MSTD_1D_PAR_NAIVE)
  parallel_exhaustive_search1d(N);
#elif defined(MSTD_1D_PAR_NAIVE_RESTRICTED)
  parallel_exhaustive_search1d_restricted(N);
#elif defined(MSTD_1D_PAR_STACK)
  parallel_stack_search1d(N);
#elif defined(MSTD_1D)
  #error "You must define one of MSTD_1D directives"
#endif
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

#if defined(MSTD_2D_SEQ)
  stack_search2d(R, C);
#elif defined(MSTD_2D_PAR_NAIVE)
  parallel_exhaustive_search2d(R, C);
#elif defined(MSTD_2D_PAR_STACK)
  parallel_stack_search2d(R, C);
#elif defined(MSTD_2D)
  #error "You must define one of MSTD_2D directives"
#endif
}


int main(int argc, char** argv) {
#if defined(MSTD_1D)
  oned(argc, argv);
#elif defined(MSTD_2D)
  twod(argc, argv);
#else
  #error "You must define MSTD_1D or MSTD_2D"
#endif
  return 0;
}
