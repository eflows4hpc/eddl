
/////////////////////////////////////////////////////////////////////////////
// This file is part of EDDLL an European Distributed Deep Learning Library.
// Developed within the DeepHealth project.
// Boosting AI in Europe.
//
// Main authors and developers:
//      Roberto Paredes: rparedes@prhlt.upv.es
//      Joan Ander Gómez: jon@prhlt.upv.es
//
//
// Collaborators:
//      Juan Maroñas: jmaronas@prhlt.upv.es
//
//
// To collaborate please contact rparedes@prhlt.upv.es
//
/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cuda.h>

#include "gpu_kernels.h"


__global__ void fill(float *aptr,float *bptr,int t,int aini,int at,int bini,int bt,int tot,int inc)
{
  int i=blockIdx.x;
  int j=threadIdx.x;
  int k=blockIdx.y;

  int ap=(i*at)+((aini+j)*t)+k;
  int bp=(i*bt)+((bini+j)*t)+k;

  if (bp<tot)
    if (inc) bptr[bp]+=aptr[ap];
    else bptr[bp]=aptr[ap];

}


__global__ void mask(float* a, float v, long int rows, long int cols)
{
 long int ops=rows*cols;
 long int thread_id_x = threadIdx.x+blockIdx.x*blockDim.x;

 if (thread_id_x < ops)
   a[thread_id_x]=a[thread_id_x]<v;

}


__global__ void set(float* a, float v, long int rows, long int cols)
{
    long int ops=rows*cols;
    long int thread_id_x = threadIdx.x+blockIdx.x*blockDim.x;

    if (thread_id_x < ops)
        a[thread_id_x]=v;

}