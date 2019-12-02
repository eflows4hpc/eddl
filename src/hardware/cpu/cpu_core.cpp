/*
 * EDDL Library - European Distributed Deep Learning Library.
 * Version: 0.2
 * copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
 * Date: October 2019
 * Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
 * All rights reserved
 */


#include "cpu_hw.h"

void cpu_transpose(Tensor * A, Tensor * B) {
    #pragma omp parallel for
    for (int i = 0; i < A->size; i++)
        B->ptr[i] = A->ptr[i];
}

void cpu_copy(Tensor * A, Tensor * B){
    #pragma omp parallel for
    for (int i = 0; i < A->size; i++)
        B->ptr[i] = A->ptr[i];
}

void cpu_fill_(Tensor *A, float v){
    #pragma omp parallel for
    for (int i = 0; i < A->size; ++i)
        A->ptr[i] = v;
}

void cpu_fill(Tensor * A, int aini, int aend, Tensor * B, int bini, int bend, int inc){
    int at = A->size / A->shape[0];
    int bt = B->size / B->shape[0];

    int t = 1;


    for (int i = 2; i < A->ndim; i++)
        t *= A->shape[i];

    #pragma omp parallel for
    for (int i = 0; i < A->shape[0]; i++) {
        int ap = (i * at) + (aini * t);
        int bp = (i * bt) + (bini * t);

        for (int j = aini; j < aend; j++) {
            for (int k = 0; k < t; k++, ap++, bp++)
                if (inc) B->ptr[bp] += A->ptr[ap];
                else B->ptr[bp] = A->ptr[ap];
        }
    }
}


void cpu_select(Tensor *A, Tensor *B, const int* indices){
    #pragma omp parallel for
    for (int b=0; b < B->shape[0]; b++) {  // walk batches
        int A_str_batch = b * A->stride[0];
        int B_str_batch = b * B->stride[0];

        for (int i = 0; i < B->stride[0]; i++) {  // walk c,h,w
            int A_pos = A_str_batch + indices[i];
            int B_pos = B_str_batch + i;

            B->ptr[B_pos] = A->ptr[A_pos];
        }
    }
}

void cpu_select(Tensor * A, Tensor * B, vector<int> sind, int ini, int end)
{
    int s = A->size / A->shape[0];


    #pragma omp parallel for
    for (int i = ini; i < end; i++) {
        int p  = sind[i] * s;
        int pb = (i - ini) * s;
        for (int j = 0; j < s; j++, p++, pb++)
            B->ptr[pb] = A->ptr[p];
    }
}

void cpu_deselect(Tensor * A, Tensor * B, vector<int> sind, int ini, int end){
    int s = A->size / A->shape[0];

    #pragma omp parallel for
    for (int i = ini; i < end; i++) {
        int p  = sind[i] * s;
        int pb = (i - ini) * s;
        for (int j = 0; j < s; j++, p++, pb++)
            B->ptr[p] = A->ptr[pb];
    }
}
