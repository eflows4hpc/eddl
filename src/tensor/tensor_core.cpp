/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.2
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/
#include <utility>

#include "tensor.h"
#include "../hardware/cpu/cpu_hw.h"

#ifdef cGPU
#include "../hardware/gpu/gpu_tensor.h"
#include "../hardware/gpu/gpu_hw.h"
#include "../hardware/gpu/nn/gpu_nn.h"
#endif

using namespace std;

// ***** Core (in-place) *****************************
void Tensor::fill_(float v) {
    if (this->isCPU()) {
        cpu_fill_(this, v);
    }
#ifdef cGPU
    else if (this->isGPU())
      {
        gpu_fill_(this,v);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif
}

//Tensor* Tensor::fill(Tensor *A, float v){
//    Tensor *t_new = A->clone();
//    t_new->fill_(v);
//    return t_new;
//}

void Tensor::reshape_(const vector<int> &shape){
    int new_size = 1;
    for(auto i : shape) new_size *= i;

    // Check if the new size is compatible
    if(new_size!=this->size){
        msg("Not compatible shapes", "Tensor::reshape_");
    }

    // Update attributes
    this->ndim = shape.size();
    this->shape = vector<int>(shape);

    // Use eigen for 2 dimensions
    if (this->ndim == 2) {
        this->ptr2=(Eigen::MatrixXf*)new Eigen::Map<Eigen::MatrixXf>(ptr, this->shape[1], this->shape[0]);
    }

    // Update strides
    this->stride = vector<int>();
    int s=this->size;
    for(int i=0;i<ndim;i++) {
        s/=shape[i];
        this->stride.push_back(s);
    }
}

Tensor* Tensor::reshape(Tensor *A, const vector<int> &shape){
    Tensor *t_new = A->clone();
    t_new->reshape_(shape);
    return t_new;
}

void Tensor::squeeze_(){
    // Remove single dimension entries from the array
    vector<int> new_shape;
    for(auto &d : this->shape){
        if(d>1){
            new_shape.push_back(d);
        }
    }
    this->reshape_(new_shape);
}

Tensor* Tensor::squeeze(Tensor *A){
    Tensor *t_new = A->clone();
    t_new->squeeze_();
    return t_new;
}

void Tensor::unsqueeze_(){
    vector<int> new_shape(this->shape);
    new_shape.insert(new_shape.begin(), 1); // Add one dimension to the beginning
    this->reshape_(new_shape);
}

Tensor* Tensor::unsqueeze(Tensor *A){
    Tensor *t_new = A->clone();
    t_new->unsqueeze_();
    return t_new;
}

Tensor* Tensor::permute(Tensor* t, const vector<int>& dims){
    // Build descriptor
    auto *sd = new PermuteDescriptor(dims);
    sd->build(t->shape);
    sd->build_indices();

    // Initialize new tensor
    auto *new_t = new Tensor(sd->oshape, t->device);

    // Fill new tensor
    Tensor::select(t, new_t, sd);

    return new_t;
}

Tensor* Tensor::moveaxis(Tensor* t, int source, int destination){
    // Check values
    if(source<-1 || destination <-1){
        msg("Invalid axis", "Tensor::moveaxis");
    }

    // User "-1" as alias for the last dimension
    if(source == -1){source = t->ndim-1; }
    if(destination == -1){destination = t->ndim-1; }

    // Build axes to permute [0, 3] => (0,1,2,3) => (3,1,2,0)
    vector<int> dims;
    for(int i=0; i<t->ndim;i++){ dims.emplace_back(i); }
    dims[source] = destination;
    dims[destination] = source;

    // Permute tensor
    Tensor* t2 = Tensor::permute(t, dims);
    return t2;
}


int Tensor::get_address_rowmajor(vector<int> indices){
    int address=0;
    for(int i=0; i<this->ndim; i++){ address +=  indices[i] * this->stride[i];}  //*(indices.begin()+i)
    return address;
}

vector<int> Tensor::get_indices_rowmajor(int address){
    vector<int> indices;
    indices.reserve(this->shape.size());
    for(int i=0; i<this->shape.size(); i++){
        indices.push_back(address / this->stride[i] % this->shape[i]);
    }
    return indices;
}

float Tensor::get_(vector<int> indices){
    return this->ptr[get_address_rowmajor(std::move(indices))];
}

void Tensor::set_(vector<int> indices, float value){
    this->ptr[get_address_rowmajor(std::move(indices))] = value;
}

bool Tensor::valid_indices(vector<int> indices){
    for (int i=0; i<indices.size(); i++){
        if (indices[i] <0 || indices[i] >= this->shape[i]){
            return false;
        }
    }
    return true;
}
// ***** Core (static) *****************************
void Tensor::transpose(Tensor *A, Tensor *B, vector<int> dims) {
    // Transpose
    // TODO: Review correctness
    B->tsem->lock();
    if (!Tensor::eqsize(A, B))
        msg("Tensors with different shape", "Tensor::transpose");

    if (A->device != B->device) msg("Tensors in different devices", "Tensor::transpose");

    Tensor *N;
    if (A == B) N = new Tensor(A->getShape(), A->device);
    else N = B;


    // Copy tensor data
    if (A->isCPU()) {
        cpu_transpose(A, N);
    }
#ifdef cGPU
    else if (A->isGPU())
      {

      }
#endif
#ifdef cFPGA
    else {

    }
#endif
    B->tsem->unlock();

    if (A == B) delete N;

}

void Tensor::copy(Tensor *A, Tensor *B) {
    ///////////////////////////////////////
    /// Copy from A to B
    //////////////////////////////////////
    // TODO: Review correctness for ndim==2

    if (!Tensor::eqsize(A, B)) {
        A->info();
        B->info();
        msg("Tensors with different shape", "Tensor::copy");
    }

    B->tsem->lock();
    if ((A->isCPU()) && (B->isCPU())) {
        cpu_copy(A, B);
    }
#ifdef cGPU
        else if ((A->isGPU())&&(B->isGPU())) {
          gpu_copy_gpu(A,B);
        }
        else if ((A->isCPU())&&(B->isGPU()))
          {
            gpu_copy_to_gpu(A->ptr,B);
          }
        else if ((A->isGPU())&&(B->isCPU()))
          {
            gpu_copy_from_gpu(A,B->ptr);
          }
#endif
    else {
        fprintf(stderr, "(%d %d)\n", A->device, B->device);
        msg("unsupported copy between devices", "Tensor::copy");
    }
    B->tsem->unlock();
}


void Tensor::fill(Tensor *A, int aini, int aend, Tensor *B, int bini, int bend, int inc) {
    ///////////////////////////////////////
    /// Partial copy ndim=1
    //////////////////////////////////////
    if (A->ndim != B->ndim)
        msg("Tensors with different shape", "Tensor::fill");

    B->tsem->lock();
    if ((A->isCPU()) && (B->isCPU())) {
        cpu_fill(A, aini, aend, B, bini, bend, inc);
    }
#ifdef cGPU
        else if ((A->isGPU())&&(B->isGPU())) {
          gpu_fill(A,aini,aend,B,bini,bend,inc);
        }
#endif
    else {
        fprintf(stderr, "(%d %d)\n", A->device, B->device);
        msg("unsupported copy between devices", "Tensor::copy");
    }
    B->tsem->unlock();
}

Tensor* Tensor::select(const vector<string>& indices){
    Tensor* t = nullptr;

    auto *sd = new SelDescriptor(indices);
    sd->build(this->shape);
    sd->build_indices();

    // Initialize tensor
    t = new Tensor(sd->oshape, this->device);

    // Perform select
    Tensor::select(this, t, sd);
    return t;
}

void Tensor::select(Tensor *A, Tensor* B, SelDescriptor *sd){
    if (A->isCPU() && B->isCPU()) {
        cpu_select(A, B, sd);
    }
#ifdef cGPU
    else if (A->isGPU() && B->isGPU())
      {
        gpu_select(A, B, sd);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif

}

void Tensor::select_back(Tensor *A, Tensor* B, SelDescriptor *sd){
    if (A->isCPU() && B->isCPU()) {
        cpu_select_back(A, B, sd);
    }
#ifdef cGPU
    else if (A->isGPU() && B->isGPU())
      {
        gpu_select_back(A, B, sd);
      }
#endif
#ifdef cFPGA
    else {

    }
#endif

}

void Tensor::select(Tensor *A, Tensor *B, vector<int> sind, int ini, int end) {
    ///////////////////////////////////////
    /// Select from A to B, A is bigger
    //////////////////////////////////////

    if ((A->size / A->shape[0]) != (B->size / B->shape[0])) {
        A->info();
        B->info();
        msg("Incompatible shape", "Tensor::select");
    }

    //B->tsem->lock();
    if ((A->isCPU()) && (B->isCPU())) {
        cpu_select(A, B, sind, ini, end);
    }
    else if ((A->isGPU()) && (B->isCPU())) {
        Tensor *Ac=A->clone();
        Ac->toCPU();

        cpu_select(Ac, B, sind, ini, end);

        delete Ac;
    }else if ((A->isCPU()) && (B->isGPU())) {
        Tensor *Bc=B->clone();
        Bc->toCPU();
        cpu_select(A, Bc, sind, ini, end);

        Tensor::copy(Bc,B);

        delete Bc;
    }
    else if ((A->isGPU()) && (B->isGPU())) {
        Tensor *Ac=A->clone();
        Ac->toCPU();

        Tensor *Bc=B->clone();
        Bc->toCPU();

        cpu_select(Ac, Bc, sind, ini, end);

        Tensor::copy(Bc,B);

        delete Ac;
        delete Bc;
    }
    else {
        msg("unsuppoted select", "Tensor::select");
    }
    //B->tsem->unlock();
}
void Tensor::deselect(Tensor *A, Tensor *B, vector<int> sind, int ini, int end) {
    ///////////////////////////////////////
    /// deSelect from A to B, B is bigger
    //////////////////////////////////////

    if ((A->size / A->shape[0]) != (B->size / B->shape[0])) {
        A->info();
        B->info();
        msg("Incompatible shape", "Tensor::select");
    }

    //B->tsem->lock();
    if ((A->isCPU()) && (B->isCPU())) {
        cpu_deselect(A, B, sind, ini, end);
    }
    else if ((A->isGPU()) && (B->isCPU())) {
        Tensor *Ac=A->clone();
        Ac->toCPU();

        cpu_deselect(Ac, B, sind, ini, end);

        delete Ac;
    }else if ((A->isCPU()) && (B->isGPU())) {
        Tensor *Bc=B->clone();
        Bc->toCPU();
        cpu_deselect(A, Bc, sind, ini, end);

        Tensor::copy(Bc,B);

        delete Bc;
    }
    else if ((A->isGPU()) && (B->isGPU())) {
        Tensor *Ac=A->clone();
        Ac->toCPU();

        Tensor *Bc=B->clone();
        Bc->toCPU();

        cpu_deselect(Ac, Bc, sind, ini, end);
        Tensor::copy(Bc,B);

        delete Ac;
        delete Bc;
    }
    else {
        msg("unsuppoted select", "Tensor::select");
    }
    //B->tsem->unlock();
}

void Tensor::tile(Tensor *A, Tensor *B)
{

    int Asize=A->shape[0];
    int Bsize=B->shape[0];


    if (Bsize>Asize) {
        vector<int> sind(Bsize);
        int start,end;
        for(int i=0;i<Bsize;i++) sind[i]=i;
        for(int i=0;i<Bsize/Asize;i++) {
            start = i * Asize;
            end = start + Asize;
            Tensor::deselect(A, B, sind, start, end);
        }
        if (Bsize%Asize) {
            Tensor::deselect(A, B, sind, end, end+(Bsize%Asize));
        }
    }
    else {
        vector<int> sind(Bsize);
        for(int i=0;i<Bsize;i++) sind[i]=i;
        Tensor::select(A, B, sind, 0, Bsize);
    }
}
