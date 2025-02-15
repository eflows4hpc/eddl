/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 1.1
* copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
* Date: March 2022
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "eddl/layers/pool/layer_pool.h"


using namespace std;


int LPool::total_layers = 0;

LPool::LPool(Layer *parent, PoolDescriptor *D, string name, int dev, int mem) : LinLayer(name, dev, mem) {
    if (parent->output->ndim != 4) msg("LPool only works over 4D tensors", "LPool::LPool");
    if(name.empty()) this->name = "pool" + to_string(++total_layers);

    input = parent->output;
    pd = D;

    pd->build(input);

    output = pd->O;

    parent->addchild(this);
    addparent(parent);

}

LPool::~LPool(){
   delete pd;
}

void LPool::mem_delta() {
    if (this->delta == nullptr) {
        // Reserve parent's delta
        parent[0]->mem_delta();
        pd->ID = parent[0]->delta;

        delta = Tensor::zeros(pd->O->shape, pd->O->device);
        pd->D = delta;

        if (this->verbosity_level >= 2) {
            std::cout << "Booked delta for: " + this->name << std::endl;
        }
    } else if (this->delta->shape[0] != this->output->shape[0]) {
        this->delta->resize(this->output->shape[0]);
    }
}


void LPool::resize(int batch){
    pd->resize(batch);
    
}
