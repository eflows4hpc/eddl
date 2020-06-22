/*
* FPGA support for EDDL Library - European Distributed Deep Learning Library.
* Version: 0.6
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: June 2020
* Author: GAP Research Group (UPV), contact: carlherlu@gap.upv.es, jflich@disca.upv.es
* All rights reserved
*/

#include <cstdio>      /* printf, scanf, NULL */
#include <cstdlib>     /* malloc, free, rand */
#include <iostream>

#include "eddl/hardware/fpga/nn/fpga_nn.h"
#include "eddl/hardware/cpu/nn/cpu_nn.h"
#include "eddl/hardware/fpga/fpga_hw.h"

extern cl::Kernel kernel_accuracy;
extern cl::CommandQueue q;
extern cl::Context context;

// -----------------------------------------------------------------
// accuracy
//
int fpga_cpuemu_accuracy(Tensor *A, Tensor *B) {
  int Asize = A->size * sizeof(float);
  int Bsize = B->size * sizeof(float);
  if (A->ptr == NULL) A->ptr = (float *)malloc(Asize);
  if (B->ptr == NULL) B->ptr = (float *)malloc(Bsize);
  fpga_copy_from_fpga(A, A->ptr);
  fpga_copy_from_fpga(B, B->ptr);
  int acc = cpu_accuracy(A, B);
  return acc;
}

int fpga_accuracy(Tensor *A, Tensor *B){
  int acc;
  _profile_fpga(_FPGA_ACCURACY, 0);

#ifndef K_ENABLED_ACCURACY
  acc = fpga_cpuemu_accuracy(A, B);
  return acc;
#else
   cl_int err;
   cl::Event event, result_ready;

   int *accu;
   posix_memalign((void **)&accu,4096,sizeof(int));
   OCL_CHECK(err, cl::Buffer buffer_acc(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int) ,accu, &err));

   OCL_CHECK(err, err = kernel_accuracy.setArg(0, *(A->fpga_ptr)));
   OCL_CHECK(err, err = kernel_accuracy.setArg(1, *(B->fpga_ptr)));
   OCL_CHECK(err, err = kernel_accuracy.setArg(2, A->shape[0]));
   OCL_CHECK(err, err = kernel_accuracy.setArg(3, A->shape[1]));
   OCL_CHECK(err, err = kernel_accuracy.setArg(4, buffer_acc));

   OCL_CHECK(err, err = q.enqueueTask(kernel_accuracy, NULL, &event));
   q.finish();

   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_acc},CL_MIGRATE_MEM_OBJECT_HOST, NULL, &result_ready));
   result_ready.wait();
     return *accu;
#endif

}
