/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 1.1
* copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
* Date: March 2022
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/

#include <cstdio>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cublas_v2.h>

#include "eddl/hardware/gpu/nn/gpu_tensor_nn.h"
#include "eddl/hardware/gpu/nn/gpu_tensor_nn_kernels.h"

#include "eddl/hardware/gpu/gpu_hw.h"
#include "eddl/hardware/gpu/gpu_tensor.h"
#include "eddl/hardware/gpu/gpu_kernels.h"

#include "eddl/tensor/tensor.h"
#include "eddl/descriptors/descriptors.h"


void gpu_mpool2D(PoolDescriptor *D){
    int device=D->I->gpu_device;
    cudaSetDevice(device);
#ifndef cCUDNN
    setDims(D->O);
    maxpool2d<<<dimGrid,dimBlock>>>(D->I->ptr, D->I->shape[0],D->ir,D->ic,D->iz,D->kr,D->kc,D->O->ptr,D->r,D->c,D->z, D->sr,D->sc,D->padrt,D->padrb,D->padcl,D->padcr, D->indX->ptr, D->indY->ptr);

    check_cuda(cudaDeviceSynchronize(),"gpu_mpool");
#else
    float alpha=1.0;
    float beta=0.0;
    check_cudnn(cudnnPoolingForward(hdnn[device], D->poolingDesc,
                                    &alpha, D->xDesc, D->I->ptr,
                                    &beta, D->yDesc, D->O->ptr),"cudnnPoolingForward MAX2D",__FILE__,__LINE__);
#endif
}


void gpu_mpool2D_back(PoolDescriptor *D){
    int device=D->I->gpu_device;
    cudaSetDevice(device);
#ifndef cCUDNN
    setDims(D->D)
    maxpool2d_back<<<dimGrid,dimBlock>>>(D->D->ptr, D->ID->ptr, D->I->shape[0],D->ir,D->ic,D->iz,D->kr,D->kc,D->O->ptr,D->r,D->c,D->z, D->sr,D->sc,D->padrt,D->padrb,D->padcl,D->padcr, D->indX->ptr, D->indY->ptr);

    check_cuda(cudaDeviceSynchronize(),"gpu_mpool_back");
#else
    float alpha=1.0;
    float beta=0.0;

    check_cudnn(cudnnPoolingBackward(hdnn[device], D->poolingDesc, &alpha, D->yDesc, D->O->ptr,
                                     D->yDesc, D->D->ptr, D->xDesc, D->I->ptr,
                                     &beta, D->xDesc, D->ID->ptr),"cudnnPoolingBackward MAX2D",__FILE__,__LINE__);
#endif
}

void gpu_mpool3D(PoolDescriptor3D *D){

int device=D->I->gpu_device;
    cudaSetDevice(device);

#ifndef cCUDNN
    setDims(D->O);
    maxpool3d<<<dimGrid,dimBlock>>>(D->I->ptr, D->I->shape[0], D->iz, D->id, D->ir,D->ic, D->kd, D->kr, D->kc, D->O->ptr, D->z, D->d, D->r,D->c, D->sd, D->sr, D->sc, D->paddf, D->paddb, D->padrt, D->padrb, D->padcl, D->padcr, D->indX->ptr, D->indY->ptr, D->indZ->ptr);
    check_cuda(cudaDeviceSynchronize(),"gpu_mpool3d");
#else
  float alpha=1.0;
    float beta=0.0;
    check_cudnn(cudnnPoolingForward(hdnn[device], D->poolingDesc,
                                    &alpha, D->xDesc, D->I->ptr,
                                    &beta, D->yDesc, D->O->ptr),"cudnnPoolingForward MAX3D",__FILE__,__LINE__);
#endif

}

void gpu_mpool3D_back(PoolDescriptor3D *D){
int device=D->I->gpu_device;
    cudaSetDevice(device);

#ifndef cCUDNN
    setDims(D->D)
    maxpool3d_back<<<dimGrid,dimBlock>>>(D->D->ptr, D->ID->ptr, D->I->shape[0], D->iz, D->id, D->ir,D->ic, D->kd, D->kr, D->kc, D->O->ptr, D->z, D->d, D->r,D->c, D->sd, D->sr, D->sc, D->paddf, D->paddb, D->padrt, D->padrb, D->padcl, D->padcr, D->indX->ptr, D->indY->ptr, D->indZ->ptr);

    check_cuda(cudaDeviceSynchronize(),"gpu_mpool3d_back");
#else
  float alpha=1.0;
    float beta=0.0;

    check_cudnn(cudnnPoolingBackward(hdnn[device], D->poolingDesc, &alpha, D->yDesc, D->O->ptr,
                                     D->yDesc, D->D->ptr, D->xDesc, D->I->ptr,
                                     &beta, D->xDesc, D->ID->ptr),"cudnnPoolingBackward MAX3D",__FILE__,__LINE__);
#endif

}

void gpu_avgpool2D(PoolDescriptor *D){
    int device=D->I->gpu_device;
    cudaSetDevice(device);
#ifndef cCUDNN
    setDims(D->O);
    avgpool2d<<<dimGrid,dimBlock>>>(D->I->ptr, D->I->shape[0], D->ir,D->ic,D->iz,D->kr,D->kc,D->O->ptr,D->r,D->c,D->z, D->sr,D->sc,D->padrt,D->padrb,D->padcl,D->padcr);

    check_cuda(cudaDeviceSynchronize(),"gpu_avgpool");
#else
    float alpha=1.0;
    float beta=0.0;
    check_cudnn(cudnnPoolingForward(hdnn[device], D->poolingDesc,
                                    &alpha, D->xDesc, D->I->ptr,
                                    &beta, D->yDesc, D->O->ptr),"cudnnPoolingForward AVG2D",__FILE__,__LINE__);

#endif
}

void gpu_avgpool2D_back(PoolDescriptor *D){
    int device=D->I->gpu_device;
    cudaSetDevice(device);
#ifndef cCUDNN
    setDims(D->D)
    avgpool2d_back<<<dimGrid,dimBlock>>>(D->D->ptr, D->ID->ptr, D->I->shape[0], D->ir,D->ic,D->iz,D->kr,D->kc,D->O->ptr,D->r,D->c,D->z, D->sr,D->sc,D->padrt,D->padrb,D->padcl,D->padcr);

    check_cuda(cudaDeviceSynchronize(),"gpu_avgpool_back");
#else
    float alpha=1.0;
    float beta=0.0;

    check_cudnn(cudnnPoolingBackward(hdnn[device], D->poolingDesc, &alpha, D->yDesc, D->O->ptr,
                                     D->yDesc, D->D->ptr, D->xDesc, D->I->ptr,
                                     &beta, D->xDesc, D->ID->ptr),"cudnnPoolingBackward AVG2D",__FILE__,__LINE__);

#endif
}

void gpu_avgpool3D(PoolDescriptor3D *D){

int device=D->I->gpu_device;
    cudaSetDevice(device);

#ifndef cCUDNN
    setDims(D->O);
    avgpool3d<<<dimGrid,dimBlock>>>(D->I->ptr, D->I->shape[0], D->iz, D->id, D->ir,D->ic, D->kd, D->kr, D->kc, D->O->ptr, D->z, D->d, D->r,D->c, D->sd, D->sr, D->sc, D->paddf, D->paddb, D->padrt, D->padrb, D->padcl, D->padcr);
    check_cuda(cudaDeviceSynchronize(),"gpu_avgpool3d");
#else
  float alpha=1.0;
    float beta=0.0;
    check_cudnn(cudnnPoolingForward(hdnn[device], D->poolingDesc,
                                    &alpha, D->xDesc, D->I->ptr,
                                    &beta, D->yDesc, D->O->ptr),"cudnnPoolingForward AVG3D",__FILE__,__LINE__);
#endif

}

void gpu_avgpool3D_back(PoolDescriptor3D *D){
int device=D->I->gpu_device;
    cudaSetDevice(device);

#ifndef cCUDNN
    setDims(D->D)
    avgpool3d_back<<<dimGrid,dimBlock>>>(D->D->ptr, D->ID->ptr, D->I->shape[0], D->iz, D->id, D->ir,D->ic, D->kd, D->kr, D->kc, D->O->ptr, D->z, D->d, D->r,D->c, D->sd, D->sr, D->sc, D->paddf, D->paddb, D->padrt, D->padrb, D->padcl, D->padcr);

    check_cuda(cudaDeviceSynchronize(),"gpu_avgpool3d_back");
#else
  float alpha=1.0;
    float beta=0.0;

    check_cudnn(cudnnPoolingBackward(hdnn[device], D->poolingDesc, &alpha, D->yDesc, D->O->ptr,
                                     D->yDesc, D->D->ptr, D->xDesc, D->I->ptr,
                                     &beta, D->xDesc, D->ID->ptr),"cudnnPoolingBackward avg3D",__FILE__,__LINE__);
#endif

}
