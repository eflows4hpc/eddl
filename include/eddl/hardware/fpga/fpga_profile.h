/*
* FPGA support for EDDL Library - European Distributed Deep Learning Library.
* Version: 1.0
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), GAP research group
* Date: December 2021
* Author: GAP Research Group (UPV), contact: jflich@disca.upv.es
* All rights reserved
*/

#ifndef __PROFILE
#define __PROFILE

#include "eddl/tensor/tensor.h"

#define _FPGA_TRANSFORM             0
#define _FPGA_HLSINF                1

#define _NUM_FPGA_FUNCS             2

extern int num_instances_fpga[_NUM_FPGA_FUNCS];
void _profile_fpga(int f_id, int end);
void _profile_fpga_tensor(const char str[], Tensor *T, int format_tensor);
void _profile_fpga_tensor_print(Tensor *T);
#endif
