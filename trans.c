// name: 兰鸥羽 Ouyu Lan
// loginID: 5140309001

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int ii, jj, i, j;
    if (M != N) {
        // visit among bunches
        for (ii = 0; ii < N; ii += 16) {
            for (jj = 0; jj <M; jj += 4) {
                // visit in one bunch
                for (i = ii; i < ii + 16 && i < N; ++i) {
                    for (j = jj; j < jj + 4 && j < M; ++j) {
                        // ignore the diagon
                        if (i != j) {
                            B[j][i] = A[i][j];
                        }
                    }
                    // deal with the diagon
                    if (jj <= i && i < j) {
                        B[i][i] = A[i][i];
                    }
                }
            }
        }
    }
    else {
        int  x0, x1, x2, x3, x4, x5, x6, x7;
        for (ii = 0; ii < N; ii += 8){
            for (jj = 0; jj < M; jj += 8){
                // deal with upper 4 * 8 of A 
                for (i = 0; i < 4; ++i){ 
                    x0 = A[ii+i][jj];
                    x1 = A[ii+i][jj+1];
                    x2 = A[ii+i][jj+2];
                    x3 = A[ii+i][jj+3];
                    x4 = A[ii+i][jj+4];
                    x5 = A[ii+i][jj+5];
                    x6 = A[ii+i][jj+6];
                    x7 = A[ii+i][jj+7];
                    
                    B[jj][ii+i] = x0;
                    B[jj+1][ii+i] = x1;
                    B[jj+2][ii+i] = x2;
                    B[jj+3][ii+i] = x3;
                    B[jj][ii+i+4] = x7;
                    B[jj+1][ii+i+4] = x6;
                    B[jj+2][ii+i+4] = x5;
                    B[jj+3][ii+i+4] = x4;
                }
                
                // deal with bottom 4*8 of A
                // move the upper right sub-bunch of B
                for (i = 0; i < 4; ++i) {
                    x0 = A[ii+4][jj+3-i];
                    x1 = A[ii+5][jj+3-i];
                    x2 = A[ii+6][jj+3-i];
                    x3 = A[ii+7][jj+3-i];
                    x4 = A[ii+4][jj+4+i];
                    x5 = A[ii+5][jj+4+i];
                    x6 = A[ii+6][jj+4+i];
                    x7 = A[ii+7][jj+4+i];

                    B[jj+4+i][ii] = B[jj+3-i][ii+4];
                    B[jj+4+i][ii+1] = B[jj+3-i][ii+5];
                    B[jj+4+i][ii+2] = B[jj+3-i][ii+6];
                    B[jj+4+i][ii+3] = B[jj+3-i][ii+7];
                    B[jj+3-i][ii+4] = x0;
                    B[jj+3-i][ii+5] = x1;
                    B[jj+3-i][ii+6] = x2;
                    B[jj+3-i][ii+7] = x3;
                    B[jj+4+i][ii+4] = x4;
                    B[jj+4+i][ii+5] = x5;
                    B[jj+4+i][ii+6] = x6;
                    B[jj+4+i][ii+7] = x7;
                }
            }
        }              
    }
}


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

