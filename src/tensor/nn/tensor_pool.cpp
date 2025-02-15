/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 1.1
* copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
* Date: March 2022
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/
#include "eddl/tensor/nn/tensor_nn.h"
#include "eddl/hardware/cpu/nn/cpu_tensor_nn.h"
#include "eddl/profiling.h"

#ifdef cGPU
#include "eddl/hardware/gpu/gpu_tensor.h"
#include "eddl/hardware/gpu/gpu_hw.h"
#include "eddl/hardware/gpu/nn/gpu_tensor_nn.h"
#endif

PROFILING_ENABLE_EXTERN(MPool2D);
PROFILING_ENABLE_EXTERN(MPool2D_back);

PROFILING_ENABLE_EXTERN(MPool3D);
PROFILING_ENABLE_EXTERN(MPool3D_back);

PROFILING_ENABLE_EXTERN(AvgPool2D);
PROFILING_ENABLE_EXTERN(AvgPool2D_back);

namespace tensorNN {


    void MPool2D(PoolDescriptor *D) {
        /////////////////////////////////////////////////////////////////////
        //// MPool2D
        //// Dimensions must be compatible
        //// A is input 4D Tensor, Batch x Channels x Rows x Cols
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::MPool2D");

        PROFILING_HEADER(MPool2D);


        if (D->I->isCPU()) {
            cpu_mpool2D(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_mpool2D(D);
        }
#endif


        PROFILING_FOOTER(MPool2D);
    }

    void MPool2D_back(PoolDescriptor *D) {
        /////////////////////////////////////////////////////////////////////
        //// MPool2D
        //// Dimensions must be compatible
        //// A is input 4D Tensor, Batch x Channels x Rows x Cols
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::MPool2D_back");

        PROFILING_HEADER(MPool2D_back);


        if (D->I->isCPU()) {
            cpu_mpool2D_back(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_mpool2D_back(D);
        }
#endif

        PROFILING_FOOTER(MPool2D_back);
    }


    void MPool3D(PoolDescriptor3D *D) {
        /////////////////////////////////////////////////////////////////////
        //// MPool3D
        //// Dimensions must be compatible
        //// A is input 5D Tensor, batch_shape + (channels, pool_dim1, pool_dim2, pool_dim3)
        //// D is a PoolDescriptor3D
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 5)) msg("Tensors are not 5D", "Tensor::MPool5D");

//        PROFILING_HEADER(MPool3D);


        if (D->I->isCPU()) {
            cpu_mpool3D(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_mpool3D(D);
        }
#endif

//        PROFILING_FOOTER(MPool3D);
    }

    void MPool3D_back(PoolDescriptor3D *D) {
        /////////////////////////////////////////////////////////////////////
        //// MPool3D
        //// Dimensions must be compatible
        //// A is input 5D Tensor, batch_shape + (channels, pool_dim1, pool_dim2, pool_dim3)
        //// D is a PoolDescriptor3D
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 5)) msg("Tensors are not 5D", "Tensor::MPool2D_back");

//        PROFILING_HEADER(MPool3D_back);


        if (D->I->isCPU()) {
            cpu_mpool3D_back(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_mpool3D_back(D);
        }
#endif

//        PROFILING_FOOTER(MPool3D_back);
    }

    void AvgPool2D(PoolDescriptor *D) {
        /////////////////////////////////////////////////////////////////////
        //// AvgPool2D
        //// Dimensions must be compatible
        //// A is input 4D Tensor, Batch x Channels x Rows x Cols
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::AvgPool2D");

        PROFILING_HEADER(AvgPool2D);


        if (D->I->isCPU()) {
            cpu_avgpool2D(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_avgpool2D(D);
        }
#endif

        PROFILING_FOOTER(AvgPool2D);
    }

    void AvgPool2D_back(PoolDescriptor *D) {
        /////////////////////////////////////////////////////////////////////
        //// AvgPool2D_back
        //// Dimensions must be compatible
        //// A is input 4D Tensor, Batch x Channels x Rows x Cols
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 4)) msg("Tensors are not 4D", "Tensor::AvgPool2D_back");

        PROFILING_HEADER(AvgPool2D_back);


        if (D->I->isCPU()) {
            cpu_avgpool2D_back(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_avgpool2D_back(D);
        }
#endif

        PROFILING_FOOTER(AvgPool2D_back);
    }

    void AvgPool3D(PoolDescriptor3D *D) {
        /////////////////////////////////////////////////////////////////////
        //// AvgPool3D
        //// Dimensions must be compatible
        //// A is input 5D Tensor, batch_shape + (channels, pool_dim1, pool_dim2, pool_dim3)
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 5)) msg("Tensors are not 5D", "Tensor::AvgPool3D");


        if (D->I->isCPU()) {
            cpu_avgpool3D(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_avgpool3D(D);
        }
#endif

    }

    void AvgPool3D_back(PoolDescriptor3D *D) {
        /////////////////////////////////////////////////////////////////////
        //// AvgPool3D_back
        //// Dimensions must be compatible
        //// A is input 5D Tensor, batch_shape + (channels, pool_dim1, pool_dim2, pool_dim3)
        //// D is a PoolDescriptor
        /////////////////////////////////////////////////////////////////////
        if ((D->I->ndim != 5)) msg("Tensors are not 5D", "Tensor::AvgPool3D_back");


        if (D->I->isCPU()) {
            cpu_avgpool3D_back(D);
        }
#ifdef cGPU
        else if (D->I->isGPU())
        {
            gpu_avgpool3D_back(D);
        }
#endif
    }

}
