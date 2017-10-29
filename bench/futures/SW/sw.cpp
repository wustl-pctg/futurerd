#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <future.hpp>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include "getoptions.hpp"

#define SIZE_OF_ALPHABETS 4
#define BASE_CASE_LOG 3 // base case = 2^3 * 2^3
// make sure n is divisible by base case
#define BLOCK_ALIGN(n) (((n + (1 << BASE_CASE_LOG)-1) >> BASE_CASE_LOG) << BASE_CASE_LOG)  
#define NUM_BLOCKS(n) (n >> BASE_CASE_LOG) 
#define BLOCK_IND_TO_IND(i) (i << BASE_CASE_LOG)

static inline int max(int a, int b) {
    if( a < b )
        return b;
    else
        return a;
}

/**
 * stor : the storage for solutions to subproblems, where stor[i,j] stores the 
 *        longest common subsequence of a[1,i], and b[1,j] (assume string index is
 *        1-based), so stor[0,*] = 0 because we are not considering a at all and
 *        stor[*,0] = 0 because we are not considering b at all.
 * a, b : input strings of size n-1
 * n    : length of input strings + 1
 **/
static int simple_seq_sw(int* stor, char *a, char *b, int n) {

    // solutions to LCS when not considering input b
    for(int i = 0; i < n; i++) { // vertical strip when j == 0
        stor[i*n] = 0;
    }
    // solutions to LCS when not considering input a
    for(int j = 1; j < n; j++) { // horizontal strip when i == 0
        stor[j] = 0;
    }
    for(int i = 1; i < n; i++) {
        for(int j = 1; j < n; j++) {
            int max_val = 0;
            for(int k = 1; k < i; k++) {
                max_val = max(stor[k*n + j]-(i-k), max_val);
            }
            for(int k = 1; k < j; k++) {
                max_val = max(stor[i*n + k]-(j-k), max_val);
            }
            stor[i*n + j] = max(stor[(i-1)*n + (j-1)] + (a[i-1]==b[j-1]), max_val);
        }
    }

    return stor[(n-1)*n + (n-1)];
}

static inline int 
process_sw_tile(int *stor, char *a, char *b, int n, int iB, int jB) {

    int bSize = 1 << BASE_CASE_LOG;

    for(int i = 0; i < bSize; i++) {
        for(int j = 0; j < bSize; j++) {

            int i_ind = BLOCK_IND_TO_IND(iB) + i;
            int j_ind = BLOCK_IND_TO_IND(jB) + j;

            if(i_ind == 0 || j_ind == 0) {
                stor[i_ind*n + j_ind] = 0;
            } else {
                int max_val = 0;
                for(int k = 1; k < i_ind; k++) {
                    max_val = max(stor[k*n + j_ind]-(i_ind-k), max_val);
                }
                for(int k = 1; k < j_ind; k++) {
                    max_val = max(stor[i_ind*n + k]-(j_ind-k), max_val);
                }
                stor[i_ind*n + j_ind] = 
                    max( stor[(i_ind-1)*n + (j_ind-1)] + (a[i_ind-1]==b[j_ind-1]), 
                         max_val );
            }
        }
    }
    
    return 0;
}

/* Unused
static int iter_sw(int *stor, char *a, char *b, int n) {  

    int nBlocks = NUM_BLOCKS(n);

    for(int iB = 0; iB < nBlocks; iB++) {
        for(int jB = 0; jB < nBlocks; jB++) {
            process_sw_tile(stor, a, b, n, iB, jB);   
        }
    }
    
    return stor[n*(n-1) + n-1];
}
*/

// this is not structured use, because it's not single-touch
/* Unused 
static int wave_sw_with_futures(int *stor, char *a, char *b, int n) {

    int nBlocks = NUM_BLOCKS(n);

    cilk::future<int> * farray[nBlocks * nBlocks];

    // compute the first square
    create_future(int, farray[0], process_sw_tile, stor, a, b, n, 0, 0);
    farray[0]->get();

    // compute the upper triangle (assume nxn SW)
    for(int num_waves = 1; num_waves < nBlocks; num_waves++) {
        for(int jB = 0; jB <= num_waves; jB++) {
            int iB = num_waves - jB;

            if(jB > 0) { farray[iB*nBlocks + jB - 1]->get(); } // left dependencies
            if(iB > 0) { farray[(iB-1)*nBlocks + jB]->get(); } // up dependencies
            create_future(int, farray[iB*nBlocks + jB], process_sw_tile, stor, a, b, n, iB, jB);
        }
    }
    
    // lower half of triangle 
    for(int num_waves = 1; num_waves < nBlocks; num_waves++) {
        int iBase = nBlocks + num_waves - 1;
        for(int jB = num_waves; jB < nBlocks; jB++) {
            int iB = iBase - jB;
            // fprintf(stderr, "updating (%d, %d)\n", iB, jB); 
            if(jB > 0) { farray[iB*nBlocks + jB - 1]->get(); } // left depedency
            if(iB > 0) { farray[(iB-1)*nBlocks + jB]->get(); } // up dependency
            create_future(int, farray[iB*nBlocks + jB], process_sw_tile, stor, a, b, n, iB, jB);
        }
    }

    return stor[n*(n-1) + n-1];
}
*/

static int process_sw_tile_with_get(cilk::future<int> * farray[], int *stor,
                                    char *a, char *b, int n, int iB, int jB) {
    
    int nBlocks = NUM_BLOCKS(n);
    
    if(jB > 0) { farray[iB*nBlocks + jB - 1]->get(); } // left dependency
    if(iB > 0) { farray[(iB-1)*nBlocks + jB]->get(); } // up dependency
    
    process_sw_tile(stor, a, b, n, iB, jB);

    return 0;
}

static int wave_sw_with_futures_par(int *stor, char *a, char *b, int n) {

    int nBlocks = NUM_BLOCKS(n);
    int blocks = nBlocks * nBlocks;
    
    cilk::future<int> * farray[blocks];
    
    // initialize all future handles
    cilk_for(int i=0; i < blocks; i++) {
        create_future_handle(int, farray[i]);
    }

    cilk_for(int i=0; i < blocks; i++) {
        int iB = i / nBlocks; // row block index 
        int jB = i % nBlocks; // col block index
        spawn_proc_with_future_handle(farray[i], process_sw_tile_with_get,
                                      farray, stor, a, b, n, iB, jB);
    }

    return stor[n*(n-1) + n-1];
}

// Ensure that we run serially
static void ensure_serial_execution(void) {
  // assert(1 == __cilkrts_get_nworkers());
  fprintf(stderr, "Forcing CILK_NWORKERS=1.\n");
  char *e = getenv("CILK_NWORKERS");
  if (!e || 0!=strcmp(e, "1")) {
    // fprintf(err_io, "Setting CILK_NWORKERS to be 1\n");
    if( setenv("CILK_NWORKERS", "1", 1) ) {
      fprintf(stderr, "Error setting CILK_NWORKERS to be 1\n");
      exit(1);
    }
  }
}

static void gen_rand_string(char * s, int s_length) {
  for(int i = 0; i < s_length; ++i ) {
    s[i] = (char)(rand() % SIZE_OF_ALPHABETS + 97);
  }
}

const char* specifiers[] = {"-n", "-c", "-h"};
int opt_types[] = {INTARG, BOOLARG, BOOLARG};

int main(int argc, char *argv[]) {

    int n = 1024;
    int check = 0, help = 0;

    get_options(argc, argv, specifiers, opt_types, &n, &check, &help);

    if(help) {
        fprintf(stderr, "Usage: sw [-n size] [-c] [-h]\n");
        fprintf(stderr, "\twhere -n specifies string length - 1");
        fprintf(stderr, "\tcheck results if -c is set\n");
        fprintf(stderr, "\toutput this message if -h is set\n");
        exit(1);
    }

    ensure_serial_execution();
     
    n = BLOCK_ALIGN(n); // round it to be 64-byte aligned
    printf("Compute SmithWaterman with %d x %d table.\n", n, n);

    // str len is n-1, but allocated n to have the last char be \0
    char *a1 = (char *)malloc(n * sizeof(char));
    char *b1 = (char *)malloc(n * sizeof(char));
    int *stor1 = (int *) malloc(sizeof(int) * n * n);

    /* Generate random inputs; a/b[n-1] not used */
    gen_rand_string(a1, n-1);
    gen_rand_string(b1, n-1);
    a1[n-1] = '\0';
    b1[n-1] = '\0';

    int result = wave_sw_with_futures_par(stor1, a1, b1, n);

    if(check) {
        char *a2 = (char *)malloc(n * sizeof(char));
        char *b2 = (char *)malloc(n * sizeof(char));
        int *stor2 = (int *) malloc(sizeof(int) * n * n);

        memcpy(a2, a1, n * sizeof(char));
        memcpy(b2, b1, n * sizeof(char));

        int result2 = simple_seq_sw(stor2, a2, b2, n);
        assert(result2 == result);

        for(int i=0; i < n; i++) {
            for(int j=0; j < n; j++) {
                assert(stor1[i*n + j] == stor2[i*n + j]);
            }
        }

        fprintf(stderr, "Check passed.\n");

        free(a2);
        free(b2);
        free(stor2);
    }
    
    printf("Result: %d\n", result);

    free(a1);
    free(b1);
    free(stor1);

    return 0;
}

