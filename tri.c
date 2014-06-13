#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#define get(s, i) (((s) >> (i)) & 1)

#define T(n) (((n) * ((n) + 1)) / 2)

int row[64] = {
  0,
  1, 1,
  2, 2, 2,
  3, 3, 3, 3,
  4, 4, 4, 4, 4,
  5, 5, 5, 5, 5, 5,
  6, 6, 6, 6, 6, 6, 6,
  7, 7, 7, 7, 7, 7, 7, 7,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9
};

int bitset_get(char* s, int i, int n) {
  //assert(0 <= i && i < n);
  return (s[i / 8] >> (i % 8)) & 1;
}

void bitset_set(char *s, int i, int n) {
  //assert(0 <= i && i < n);
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

int do_tri(long long s, int n, char* sum, char* diff) {
  int Tn = T(n);
  int N = n * n;

  int i, j;
  for (i = 0; i < Tn; i++) {
    if (!get(s, i)) continue;
    int ir = row[i];
    int ic = i - T(ir);

    for (j = i; j < Tn; j++) {
      if (!get(s, j)) continue;
      int jr = row[j];
      int jc = j - T(jr);

      int rs = ir + jr;
      int cs = ic + jc;
      int rd1 = ir - jr + n;
      int cd1 = ic - jc + n;
      int rd2 = jr - ir + n;
      int cd2 = jc - ic + n;


      bitset_set(sum, rs * 2 * n + cs, 4*N);
      bitset_set(diff, rd1 * 2 * n + cd1, 4*N);
      bitset_set(diff, rd2 * 2 * n + cd2, 4*N);
    }
  }

  int sums = bitset_count(sum, 4*N);
  int diffs = bitset_count(diff, 4*N);
  //printf("%lld has %d sums and %d diffs\n", s, sums, diffs);
  
  return sums > diffs;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Need n on command line.\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  if (T(n) >= 63) {
    printf("T(n) must be <= 63, but got %d.\n", T(n));
    exit(1);
  }

  long long scratch[16];


  long long s;

  for (s = 0LL; s < (1LL << T(n)); s++) {
    bzero(scratch, 16 * sizeof(long long));
    if (do_tri(s, n, (char*)scratch, (char*)(scratch + 8))) {
      printf("%lld\n", s);
    }
  }

  return 0;
}
