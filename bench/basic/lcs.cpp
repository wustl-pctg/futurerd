#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <future.hpp>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include "../util/getoptions.hpp" 
#include "../util/util.hpp" 

#define SIZE_OF_ALPHABETS 4
#define BASE_CASE_LOG 3 // base case = 2^3 * 2^3
// make sure n is divisible by base case
#define BLOCK_ALIGN(n) (((n + (1 << BASE_CASE_LOG)-1) >> BASE_CASE_LOG) << BASE_CASE_LOG)  
#define NUM_BLOCKS(n) (n >> BASE_CASE_LOG) 
#define BLOCK_IND_TO_IND(i) (i << BASE_CASE_LOG)

#define COMPUTE_LCS 0

#define DIAGONAL 0
#define LEFT 1
#define UP   2

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
static int 
simple_seq_solve(int* stor, int *where, char *a, char *b, char *res, int n) {

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
            if(a[i-1] == b[j-1]) {
                stor[i*n + j] = stor[(i-1)*n + j-1] + 1;
                where[i*n+j] = DIAGONAL;
            } else {
                stor[i*n + j] = max(stor[(i-1)*n + j], stor[i*n + j-1]);
                if(stor[(i-1)*n + j] > stor[i*n + j-1]) {
                    where[i*n+j] = UP;
                } else { 
                    where[i*n+j] = LEFT;
                }
            }
        }
    }

    int leng = stor[n*(n-1) + n-1];
#if COMPUTE_LCS
    int i = n-1, j = n-1, k = leng;
    res[k--] = '\0';

    while(k >= 0) {
        switch(where[i*n + j]) {
            case DIAGONAL: 
                assert(a[i-1] == b[j-1]);
                res[k--] = a[i-1];
                i--;    
                j--;
                break;
            case LEFT: 
                j--;
                break;
            case UP: 
                i--;
                break;
        }
    }
    assert(k<0);
#endif

    return leng;
}

static int process_lcs_tile(int *stor, char *a, char *b, int n, int iB, int jB) {

    int bSize = 1 << BASE_CASE_LOG;

    for(int i = 0; i < bSize; i++) {
        for(int j = 0; j < bSize; j++) {

            int i_ind = BLOCK_IND_TO_IND(iB) + i;
            int j_ind = BLOCK_IND_TO_IND(jB) + j;

            if(i_ind == 0 || j_ind == 0) {
                stor[i_ind*n + j_ind] = 0;
            } else {
                if(a[i_ind-1] == b[j_ind-1]) {
                    stor[i_ind*n + j_ind] = stor[(i_ind-1)*n + j_ind-1] + 1;
                } else {
                    stor[i_ind*n + j_ind] = max(stor[(i_ind-1)*n + j_ind], 
                                                stor[i_ind*n + j_ind-1]);
                }
            }
        }
    }

    return 0;
}

/* Unused
static int iter_lcs(int *stor, char *a, char *b, int n) {

    int nBlocks = NUM_BLOCKS(n);

    for(int iB = 0; iB < nBlocks; iB++) {
        for(int jB = 0; jB < nBlocks; jB++) {
            process_lcs_tile(stor, a, b, n, iB, jB);
        }
    }
    
    return stor[n*(n-1) + n-1];
}
*/

#ifdef STRUCTURED_FUTURES
int wave_lcs_with_futures(int *stor, char *a, char *b, int n) {

    int nBlocks = NUM_BLOCKS(n);
        
    // create an array of future objects
    //cilk::future<int> *farray = new cilk::future<int> [nBlocks * nBlocks];
    cilk::future<int> *farray = (cilk::future<int>*)
      malloc(sizeof(cilk::future<int>) * nBlocks * nBlocks);
    
    // walk the upper half of triangle, including the diagonal (we assume square NxN LCS) 
    for(int wave_front = 0; wave_front < nBlocks; wave_front++) {
        for(int jB = 0; jB <= wave_front; jB++) {
            int iB = wave_front - jB;
            if(iB > 0) { farray[(iB-1)*nBlocks + jB].get(); } // up dependency
            // since we are walking the wavefront serially, no need to get
            // left dependency --- already gotten by previous square.
            cilk::future<int> *f = &farray[iB*nBlocks + jB];
            reuse_future(int, f, process_lcs_tile, stor, a, b, n, iB, jB);
        }
    }

    // walk the lower half of triangle 
    for(int wave_front = 1; wave_front < nBlocks; wave_front++) {
        int iBase = nBlocks + wave_front - 1;
        for(int jB = wave_front; jB < nBlocks; jB++) {
            int iB = iBase - jB;
            // need to get both up and left dependencies for the last row, 
            // but otherwise just the up dependency. 
            if(iB == (nBlocks - 1) && jB > 0) { farray[iB*nBlocks + jB - 1].get(); } // left dependency
            if(iB > 0) { farray[(iB-1)*nBlocks + jB].get(); } // up dependency
            cilk::future<int> *f = &farray[iB*nBlocks + jB];
            reuse_future(int, f, process_lcs_tile, stor, a, b, n, iB, jB);
        }
    }
    // make sure the last square finishes before we move onto returning
    farray[nBlocks * nBlocks - 1].get();

    //delete[] farray;
    free(farray);

    return stor[n*(n-1) + n-1];
}
#endif

#ifdef NONBLOCKING_FUTURES
static int process_lcs_tile_with_get(cilk::future<int> *farray, int *stor, 
                                     char *a, char *b, int n, int iB, int jB) {

    int nBlocks = NUM_BLOCKS(n);
        
    if(jB > 0) { farray[iB*nBlocks + jB - 1].get(); } // left depedency
    if(iB > 0) { farray[(iB-1)*nBlocks + jB].get(); } // up dependency

    process_lcs_tile(stor, a, b, n, iB, jB);

    return 0;
}

int wave_lcs_with_futures_par(int *stor, char *a, char *b, int n) {

    int nBlocks = NUM_BLOCKS(n);
    int blocks = nBlocks * nBlocks;
        
    // create an array of future handles 
    //cilk::future<int> *farray = new cilk::future<int> [blocks];
    cilk::future<int> *farray = (cilk::future<int>*)
      malloc(sizeof(cilk::future<int>) * blocks);

    
    // now we spawn off the function that will call get 
    cilk_for(int i=0; i < blocks; i++) {
        int iB = i / nBlocks; // row block index
        int jB = i % nBlocks; // col block index
        cilk::future<int> *f = &(farray[i]);
        reuse_future(int, f, process_lcs_tile_with_get, farray, stor, a, b, n, iB, jB);
    }
    // make sure the last square finishes before we move onto returning
    farray[blocks-1].get();

    //delete[] farray;
    free(farray);

    return stor[n*(n-1) + n-1];
}
#endif

const char* specifiers[] = {"-n", "-c", "-h"};
int opt_types[] = {INTARG, BOOLARG, BOOLARG};

static void do_check(int *stor1, char *a1, char *b1, int n, int result) {
    char *a2 = (char *)malloc(n * sizeof(char));
    char *b2 = (char *)malloc(n * sizeof(char));
    char *res = (char *)malloc(n * sizeof(char)); // for storing LCS
    int *stor2 = (int *) malloc(sizeof(int) * n * n);
    int *where = (int *) malloc(sizeof(int) * n * n);

    memcpy(a2, a1, n * sizeof(char));
    memcpy(b2, b1, n * sizeof(char));
    res[n-1] = '\0';

    int result2 = simple_seq_solve(stor2, where, a2, b2, res, n);
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
    free(res);
    free(where);
}

int main(int argc, char *argv[]) {

    int n = 1024;
    int check = 0, help = 0;

    get_options(argc, argv, specifiers, opt_types, &n, &check, &help);

    if(help) {
        fprintf(stderr, "Usage: lcs [-n size] [-c] [-h]\n");
        fprintf(stderr, "\twhere -n specifies string length - 1");
        fprintf(stderr, "\tcheck results if -c is set\n");
        fprintf(stderr, "\toutput this message if -h is set\n");
        exit(1);
    }

    ensure_serial_execution();

    n = BLOCK_ALIGN(n); // make sure it's divisible by base case 
    printf("Compute LCS with %d x %d table.\n", n, n);

    // str len is n-1, but allocated n to have the last char be \0
    char *a1 = (char *)malloc(n * sizeof(char));
    char *b1 = (char *)malloc(n * sizeof(char));
    int *stor1 = (int *) malloc(sizeof(int) * n * n);
    int result = 0;

    /* Generate random inputs; a/b[n-1] not used */
    gen_rand_string(a1, n-1, SIZE_OF_ALPHABETS);
    gen_rand_string(b1, n-1, SIZE_OF_ALPHABETS);
    a1[n-1] = '\0';
    b1[n-1] = '\0';

#ifdef NONBLOCKING_FUTURES
    printf("Performing LCS with non-structured future.\n");
    result = wave_lcs_with_futures_par(stor1, a1, b1, n);
    if(check) { do_check(stor1, a1, b1, n, result); }
#endif

#ifdef STRUCTURED_FUTURES
    printf("Performing LCS with structured future.\n");
    result = wave_lcs_with_futures(stor1, a1, b1, n);
    if(check) { do_check(stor1, a1, b1, n, result); }
#endif
    
    printf("Result: %d\n", result);

    free(a1);
    free(b1);
    free(stor1);

    return 0;
}

