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
void swap(int* x, int* y);
void transpose_q1(int N, int A[N][N], int B[N][N]);
void transpose_q2(int N, int A[N][N], int B[N][N]);
void transpose_q3(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32 && N == 32) {
        transpose_q1(N, A, B);
        return;
    }

    if (M == 64 && N == 64) {
        transpose_q2(N, A, B);
    }

    if (M == 61 && N == 67) {
        transpose_q3(M, N, A, B);
        return;
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */
void swap(int* x, int* y) {
    *y = (*x) ^ (*y);
    *x = (*x) ^ (*y);
    *y = (*x) ^ (*y);
}

void transpose_q1(int N, int A[N][N], int B[N][N]) {
    int i, j, bi, bj;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    for (i = 0; i < N; i += 8) {
        for (j = 0; j < N; j += 8) {
            for (bi = i, bj = j; bi < i + 8; bi++, bj++) {
                t0 = A[bi][j + 0];
                t1 = A[bi][j + 1];
                t2 = A[bi][j + 2];
                t3 = A[bi][j + 3];
                t4 = A[bi][j + 4];
                t5 = A[bi][j + 5];
                t6 = A[bi][j + 6];
                t7 = A[bi][j + 7];
                B[bj][i + 0] = t0;
                B[bj][i + 1] = t1;
                B[bj][i + 2] = t2;
                B[bj][i + 3] = t3;
                B[bj][i + 4] = t4;
                B[bj][i + 5] = t5;
                B[bj][i + 6] = t6;
                B[bj][i + 7] = t7;
            }
            for (bi = 0; bi < 8; bi++) {
                for (bj = bi + 1; bj < 8; bj++) {
                    swap(&B[j + bi][i + bj], &B[j + bj][i + bi]);
                }
            }
        }
    }
}

void transpose_q2(int N, int A[N][N], int B[N][N]) {
    int i, j, bi, bj;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    for (i = 0; i < N; i += 8) {
        for (j = 0; j < N; j += 8) {
            for (bi = 0; bi < 4; bi++) {
                t0 = A[bi + i][j + 0];
                t1 = A[bi + i][j + 1];
                t2 = A[bi + i][j + 2];
                t3 = A[bi + i][j + 3];
                t4 = A[bi + i][j + 4];
                t5 = A[bi + i][j + 5];
                t6 = A[bi + i][j + 6];
                t7 = A[bi + i][j + 7];
                B[j + bi][i + 0] = t0;
                B[j + bi][i + 1] = t1;
                B[j + bi][i + 2] = t2;
                B[j + bi][i + 3] = t3;
                B[j + bi][i + 4] = t4;
                B[j + bi][i + 5] = t5;
                B[j + bi][i + 6] = t6;
                B[j + bi][i + 7] = t7;
            }
            for (bi = 0; bi < 4; bi++) {
                for (bj = bi + 1; bj < 4; bj++) {
                    swap(&B[j + bi][i + bj], &B[j + bj][i + bi]);
                    swap(&B[j + bi][i + bj + 4], &B[j + bj][i + bi + 4]);
                }
            }
            for (bi = 0; bi < 4; bi++) {
                t0 = A[i + 4][j + bi];
                t1 = A[i + 5][j + bi];
                t2 = A[i + 6][j + bi];
                t3 = A[i + 7][j + bi];
                t4 = A[i + 4][j + bi + 4];
                t5 = A[i + 5][j + bi + 4];
                t6 = A[i + 6][j + bi + 4];
                t7 = A[i + 7][j + bi + 4];
                swap(&B[j + bi][i + 4], &t0);
                swap(&B[j + bi][i + 5], &t1);
                swap(&B[j + bi][i + 6], &t2);
                swap(&B[j + bi][i + 7], &t3);
                B[j + bi + 4][i + 0] = t0;
                B[j + bi + 4][i + 1] = t1;
                B[j + bi + 4][i + 2] = t2;
                B[j + bi + 4][i + 3] = t3;
                B[j + bi + 4][i + 4] = t4;
                B[j + bi + 4][i + 5] = t5;
                B[j + bi + 4][i + 6] = t6;
                B[j + bi + 4][i + 7] = t7;
            }
        }
    }
}

void transpose_q3(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, bi, bj;
    for (i = 0; i < N; i += 17) {
        for (j = 0; j < M; j += 17) {
            for (bi = i; bi < i + 17 && bi < N; bi++) {
                for (bj = j; bj < j + 17 && bj < M; bj++) {
                    B[bj][bi] = A[bi][bj];
                }
            }
        }
    }
}

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
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
void registerFunctions() {
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
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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
