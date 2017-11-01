/** 
 * Matrix multiplication using futures 
 * C[i,j] = A[i,k] * B[k, j]
 * The matrix layout is morton Z at the block level, and row-major in
 * the base case.
 **/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <future.hpp>

#include "../util/getoptions.hpp"
#include "../util/util.hpp"

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif

// Don't make base case too large --- tmp matrices allocated on stack
#define POWER 4 // the base case size = (2*POWER)
#define BASE_CASE (1 << POWER) // the base case size = (2*POWER)

// The following three should match
#define DATA float 
#if DATA == float
#define do_check_iter(C,I,n) do_check_iter_float(C,I,n) // use what DATA defined to be
#define init(A,n) init_float(A,n)                       // use what DATA defined to be
#else
#define do_check_iter(C,I,n) do_check_iter_int(C,I,n) // use what DATA defined to be
#define init(A,n) init_int(A,n)                       // use what DATA defined to be
#endif

#define ERR_THRESH 0.0001

static const unsigned int Q[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
static const unsigned int S[] = {1, 2, 4, 8};

unsigned long rand_nxt = 0;

static int cilk_rand(void) {
    int result;
    rand_nxt = rand_nxt * 1103515245 + 12345;
    result = (rand_nxt >> 16) % ((unsigned int) RAND_MAX + 1);
    return result;
}


// provides a look up for the Morton Number of the z-order curve given the x and y coordinate
// every instance of an (x,y) lookup must use this function
static unsigned int z_convert(int row, int col) {

    unsigned int z; // z gets the resulting 32-bit Morton Number.  
    // x and y must initially be less than 65536.
    // The top and the left boundary 

    col = (col | (col << S[3])) & Q[3];
    col = (col | (col << S[2])) & Q[2];
    col = (col | (col << S[1])) & Q[1];
    col = (col | (col << S[0])) & Q[0];

    row = (row | (row << S[3])) & Q[3];
    row = (row | (row << S[2])) & Q[2];
    row = (row | (row << S[1])) & Q[1];
    row = (row | (row << S[0])) & Q[0];

    z = col | (row << 1);

    return z;
}

// converts (x,y) position in the array to the mixed z-order row major layout
static int block_convert(int row, int col) {
    int block_index = z_convert(row >> POWER, col >> POWER);
    return (block_index * BASE_CASE << POWER)  // BASE_CASE << POWER == BASE_CASE * BASE_CASE
        + ((row - ((row >> POWER) << POWER)) << POWER) 
        + (col - ((col >> POWER) << POWER));
}

// init the matrix to random numbers
static void init_int(DATA *M, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            M[block_convert(i,j)] = (DATA) cilk_rand();
        }
    }
}

static void init_float(DATA *M, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            M[block_convert(i,j)] = (DATA) ((float)cilk_rand() / (float)RAND_MAX) - 0.5f;
        }
    }
}

// zero the matrix
template <typename T>
static void zero(T *M, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            M[block_convert(i,j)] = 0;
        }
    }
}

// iterative solution for matrix multiplication
static void seq_iter_matmul(DATA *A, DATA *B, DATA *C, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            DATA c = 0.0;
            for(int k = 0; k < n; k++) {
                c += A[block_convert(i,k)] * B[block_convert(k,j)];
            }
            C[block_convert(i, j)] = c;
        }
    }
}

static void do_check_iter_float(DATA *C, DATA *I, int n) {
    double max_err = 0.0;
    for(int i=0; i < n; i++) {
        for(int j=0; j < n; j++) {
            double diff = (C[i*n+j] - I[i*n+j]);
            if(diff < 0) { diff = -diff; }
            if(diff > max_err) { max_err = diff; }
            assert(diff < ERR_THRESH);
        }
    }
    fprintf(stderr, "Check passed; max error: %g.\n", max_err);
}

static void do_check_iter_int(DATA *C, DATA *I, int n) {

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            assert(C[i*n + j] == I[i*n + j]);
        }
    }

    fprintf(stderr, "Check passed.\n");
}

// fhandles are a 3D array with nBlocks x nBlocks x nBlocks dimensions, with
// the kB dimension being the outer most, followed by iB, followed by jB. 
// i.e. fhandles[kB][iB][jB]
static int fh_index(int kB, int iB, int jB, int nBlocks) {
    int nB2 = nBlocks * nBlocks;
    return (kB * nB2 + iB * nBlocks + jB);
}

#ifdef STRUCTURED_FUTURES
static void do_matmul_structured(DATA *A, DATA *B, DATA *C, int n) {
    
    int nBlocks = n >> POWER; // number of blocks per dimension
    int num_futures = nBlocks * nBlocks * nBlocks;
    fhandles = new cilk::future<int> *[num_futures];

    cilk_for(int iB = 0; iB < nBlocks; iB++) {
        cilk_for(int jB = 0; jB < nBlocks; jB++) {
            for(int kB = 0; kB < nblocks; kB++) {
            }
        }
    }
    delete[] fhandles;
}
#endif // STRUCTURED_FUTURES 

#ifdef NONBLOCKING_FUTURES
// use static global to avoid parameter proliferation
static int nBlocks; // number of blocks in the original matrices
static cilk::future<int> **fhandles; // future handles

static int matmul_base(DATA *A, DATA *B, DATA *C, int n, int iB, int kB, int jB) {
    DATA tmp[n*n];

    // fprintf(stderr, "C(%d, %d) = A(%d, %d) x B(%d, %d)\n", iB, jB, iB, kB, kB, jB);
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            DATA c = 0.0;
            for(int k = 0; k < n; k++) {
                c += A[i*n + k] * B[k*n + j];
            }
            tmp[i*n + j] = c;
        }
    }
    // make sure the previous kB that wrote to the same block C[iB, jB] is done
    if(kB > 0) {
        fhandles[fh_index(kB-1, iB, jB, nBlocks)]->get();
    }
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            C[i*n + j] += tmp[i*n + j];
        }
    }

    return 0;
}

// matmul using unstructured future; divide and conquer untill we reach base case, 
// C[i,j] = A[i,k] x B[k,j]
// iB: block index for the i dimension 
// kB: block index for the k dimension 
// jB: block index for the j dimension 
int matmul(DATA *A, DATA *B, DATA *C, int n, int iB, int kB, int jB) { 
            // the last two could have been global var

    // Base case uses row-major order; switch to iterative traversal 
    if(n == BASE_CASE) {
        spawn_proc_with_future_handle(fhandles[fh_index(kB, iB, jB, nBlocks)],
            matmul_base, A, B, C, n, iB, kB, jB);
        return 0;
    }

    int block_inc = (n >> POWER) >> 1;

    // partition each matrix into 4 sub matrices
    // each sub-matrix points to the start of the z pattern
    DATA *A1 = &A[block_convert(0,0)];
    DATA *A2 = &A[block_convert(0, n >> 1)]; //bit shift to divide by 2
    DATA *A3 = &A[block_convert(n >> 1,0)];
    DATA *A4 = &A[block_convert(n >> 1, n >> 1)];

    DATA *B1 = &B[block_convert(0,0)];
    DATA *B2 = &B[block_convert(0, n >> 1)];
    DATA *B3 = &B[block_convert(n >> 1, 0)];
    DATA *B4 = &B[block_convert(n >> 1, n >> 1)];
    
    DATA *C1 = &C[block_convert(0,0)];
    DATA *C2 = &C[block_convert(0, n >> 1)];
    DATA *C3 = &C[block_convert(n >> 1,0)];
    DATA *C4 = &C[block_convert(n >> 1, n >> 1)];

    // recrusively call the sub-matrices for evaluation in parallel
    cilk_spawn matmul(A1, B1, C1, n >> 1, iB, kB, jB);
    cilk_spawn matmul(A1, B2, C2, n >> 1, iB, kB, jB + block_inc);
    cilk_spawn matmul(A3, B1, C3, n >> 1, iB + block_inc, kB, jB);
    cilk_spawn matmul(A3, B2, C4, n >> 1, iB + block_inc, kB, jB + block_inc);

    cilk_spawn matmul(A2, B3, C1, n >> 1, iB, kB + block_inc, jB);
    cilk_spawn matmul(A2, B4, C2, n >> 1, iB, kB + block_inc, jB + block_inc);
    cilk_spawn matmul(A4, B3, C3, n >> 1, iB + block_inc, kB + block_inc, jB);
    cilk_spawn matmul(A4, B4, C4, n >> 1, iB + block_inc, kB + block_inc, jB + block_inc);
    cilk_sync; // this just sync for the spawning of futures but don't wait for them to finish
    
    return 0;
}

static void do_matmul_unstructured(DATA *A, DATA *B, DATA *C, int n) {
    
    // initialize the static global vars
    nBlocks = n >> POWER; // number of blocks per dimension
    int num_futures = nBlocks * nBlocks * nBlocks;
    fhandles = new cilk::future<int> *[num_futures];

    cilk_for(int i = 0; i < num_futures; i++) {
        create_future_handle(int, fhandles[i]);
    }

    // clockmark_t begin_rm = ktiming_getmark(); 
    matmul(A, B, C, n, 0, 0, 0);
    // clockmark_t end_rm = ktiming_getmark();

    // make sure we get the last kB layer of futures before we return
    cilk_for(int i = 0; i < (nBlocks * nBlocks); i++) {
        fhandles[(nBlocks-1)*(nBlocks * nBlocks) + i]->get();
    }

    delete[] fhandles;
}
#endif // NONBLOCKING_FUTURES

const char * specifiers[] = {"-n", "-c", "-h", 0};
int opt_types[] = {INTARG, BOOLARG, BOOLARG, 0};

int main(int argc, char *argv[]) {

    int n = 1024; // default n value
    int help = 0;
    int check = 0;

    get_options(argc, argv, specifiers, opt_types, &n, &check, &help);
    ensure_serial_execution();

    if(help) {   
        fprintf(stderr, "%s [-n <n>|-c|-h]\n", argv[0]);
        exit(0); 
    }

    DATA *A, *B, *C, *I = NULL;

    printf("Performing matmul with z-layout, %d x %d with base case %d x %d.\n",
           n, n, BASE_CASE, BASE_CASE);

    A = (DATA *) malloc(n * n * sizeof(DATA)); // source matrix 
    B = (DATA *) malloc(n * n * sizeof(DATA)); // source matrix
    C = (DATA *) malloc(n * n * sizeof(DATA)); // result matrix
    
    init(A, n);
    init(B, n);

    if(check) {
        I = (DATA *) malloc(n * n * sizeof(DATA)); //iter result matrix 
        zero<DATA>(I, n); 
        seq_iter_matmul(A, B, I, n);
    }

#ifdef NONBLOCKING_FUTURES
    zero<DATA>(C, n);
    do_matmul_unstructured(A, B, C, n);
    if(check) { do_check_iter(C, I, n); }
#endif

#ifdef STRUCTURED_FUTURES
    zero<DATA>(C, n);
    do_matmul_structured(A, B, C, n);
    if(check) { do_check_iter(C, I, n); }
#endif

    // clean up memory
    free(A);
    free(B);
    free(C);
    if(check) { free(I); }

    return 0;
}
