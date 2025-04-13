
#define PACKAGE "aggsum"
#define VERSION "0.01"

#define SEEDNO 1024   /* default seed for randomizer */
#define ARG1 "<arrsz>"
#define ARG2 "<tnum>"
#define ARG3 "<seednum>"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define THRSL_MIN 5      /* minimum threshold of computation size per thread */
#define UPBND_DATA_VAL 100 /* upper bound (maximum threshold) of generated data value*/
#define LWBND_DATA_VAL 0   /* lower bound (minimum threshold) of generated data value*/

int tonum(const char* nptr, int* num);

struct _appconf {
  int arrsz;
  int tnum;
  int seednum;
} appconf;

struct _range {
  int start;
  int end;
};

/** process command line argument.
 *  values are made available through the 'conf' struct.
 *  using the parsed conf to get arguments: the arrsz, tnum, and seednum
 */
int processopts(int argc, char** argv, struct _appconf* conf) {
  if (tonum(argv[1], &conf->arrsz) != 0) {
    fprintf(stderr, "Invalid array size.\n");
    exit(EXIT_FAILURE);
  }
  if (tonum(argv[2], &conf->tnum) != 0) {
    fprintf(stderr, "Invalid thread number.\n");
    exit(EXIT_FAILURE);
  }
  if (argc == 4) {
    if (tonum(argv[3], &conf->seednum) != 0) {
      fprintf(stderr, "Invalid seed number.\n");
      exit(EXIT_FAILURE);
    }
  } else {
    conf->seednum = SEEDNO;
  }
  return 0;
}

/** process string to number.
 *  string is stored in 'nptr' char array.
 *  'num' is returned the valid integer number.
 *  return 0 valid number stored in num
 *        -1 invalid and num is useless value.
 */
int tonum(const char* nptr, int* num) {
  char* endptr;
  long result = strtol(nptr, &endptr, 10);
  if (*nptr == '\0' || *endptr != '\0') {
    return -1;
  }
  if (result > INT_MAX || result < INT_MIN) {
    return -1;
  }
  *num = (int)result;
  return 0;
}


/** validate the array size argument.
 *  the size must be splitable "num_thread".
 */
int validate_and_split_argarray(int arraysize, int num_thread, struct _range* thread_idx_range) {
  int base = arraysize / num_thread;
  int rem = arraysize % num_thread;

  if (base < THRSL_MIN) {
    return -1;
  }

  int i, start = 0;
  for (i = 0; i < num_thread; i++) {
    thread_idx_range[i].start = start;
    int count = base + (i < rem ? 1 : 0);
    thread_idx_range[i].end = start + count - 1;
    start += count;
  }
  return 0;
}


/** generate "arraysize" data for the array "buf"
 *  validate the array size argument.
 *  the size must be splitable "num_thread".
 */
int generate_array_data(int* buf, int arraysize, int seednum) {
  srand(seednum);
  int i;
  for (i = 0; i < arraysize; i++) {
    buf[i] = LWBND_DATA_VAL + rand() % (UPBND_DATA_VAL - LWBND_DATA_VAL + 1);
  }
  return 0;
}

/** display help */
void help(int xcode) {
  fprintf(stdout, "Usage: %s %s %s [%s]\n", PACKAGE, ARG1, ARG2, ARG3);
  exit(xcode);
}