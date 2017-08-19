#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cilk/cilk_api.h>

#include "getoptions.hpp" 

#define SIZE_OF_ALPHABETS 4
#define BASE_CASE_LOG 3 // base case = 2^3 * 2^3
#define BLOCK_ALIGN(n) ((n >> (BASE_CASE_LOG*2)) << (BASE_CASE_LOG*2)) // n % 64 == 0
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
int simple_seq_solve(int* stor, int *where, char *a, char *b, char *res, int n) {

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

static void process_lcs_tile(int *stor, char *a, char *b, int n, int iB, int jB) {  

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
}

int wave_lcs(int *stor, char *a, char *b, int n) {  

    int iBlocks = NUM_BLOCKS(n);
    int jBlocks = NUM_BLOCKS(n);

    for(int iB = 0; iB < iBlocks; iB++) {
        for(int jB = 0; jB < jBlocks; jB++) {
            process_lcs_tile(stor, a, b, n, iB, jB);
        }
    }
    
    return stor[n*(n-1) + n-1];
}

void gen_rand_string(char * s, int s_length) {
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
        fprintf(stderr, "Usage: lcs [-n size] [-c] [-h]\n");
        fprintf(stderr, "\twhere -n specifies string length - 1");
        fprintf(stderr, "\tcheck results if -c is set\n");
        fprintf(stderr, "\toutput this message if -h is set\n");
        exit(1);
    }

    n = BLOCK_ALIGN(n); // round it to be 64-byte aligned
    printf("Compute LCS with %d x %d table.\n", n, n);

    // str len is n-1, but allocated n to have the last char be \0
    char *a1 = (char *)malloc(n * sizeof(char));
    char *b1 = (char *)malloc(n * sizeof(char));
    int *stor1 = (int *) malloc(sizeof(int) * n * n);

    /* Generate random inputs; a/b[n-1] not used */
    gen_rand_string(a1, n-1);
    gen_rand_string(b1, n-1);
    a1[n-1] = '\0';
    b1[n-1] = '\0';

    int result = wave_lcs(stor1, a1, b1, n);

    if(check) {
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
    
    printf("Result: %d\n", result);
    /*
    printf("First input:  %s.\n", a);
    printf("Second input: %s.\n", b);
    printf("Result: %d: %s\n", result, res);
    */

    free(a1);
    free(b1);
    free(stor1);

    return 0;
}

