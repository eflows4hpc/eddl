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

#include "eddl/layers/core/layer_core.h"

using namespace std;

int LSqueeze::total_layers = 0;

LSqueeze::LSqueeze(Layer *parent, int axis, string name, int dev, int mem) : LinLayer(name, dev, mem) {
    if(name.empty()) this->name = "squeeze" + to_string(++total_layers);

    this->axis = axis;
    input = parent->output;

    vector<int> oshape = compute_squeeze(this->input->shape, axis, true);
    output = new Tensor(oshape, input->ptr, input->device);  // Virtual tensor with new shape

    parent->addchild(this);
    addparent(parent);
}

LSqueeze::~LSqueeze(){
    output=delta=nullptr;
}

// virtual
void LSqueeze::resize(int batch){
    output->resize(batch, parent[0]->output->ptr, nullptr, false);
}


void LSqueeze::mem_delta() {
    if (this->delta == nullptr) {
        // Reserve parent's delta AND assign it to this layer
        parent[0]->mem_delta();

        // Problem: Delta is always created, regardless of the low_mem
        delta = new Tensor(this->output->shape, parent[0]->delta);

        if(this->verbosity_level >= 2){
            std::cout << "Booked delta for: " + this->name << std::endl;
        }
    }
}


void LSqueeze::free_delta() {
    if(this->delta != nullptr) {
        // Do not delete its delta directly (It's pointer points to parent's delta)
        delta->ptr = nullptr;
        delete delta;
        delta = nullptr;

        if(this->verbosity_level >= 2){
            std::cout << "Deleted delta for: " + this->name << std::endl;
        }
    }
}

void LSqueeze::forward() {

}


void LSqueeze::backward() {

}


Layer *LSqueeze::share(int c, int bs, vector<Layer *> p) {
    auto *n = new LSqueeze(p[0], this->axis, "share_"+to_string(c)+this->name, this->dev, this->mem_level);
    n->orig = this;
    return n;
}

Layer *LSqueeze::clone(int c, int bs, vector<Layer *> p, int todev) {
    auto *n = new LSqueeze(p[0], this->axis,  name, todev, mem_level);
    n->orig = this;
    return n;
}


string LSqueeze::plot(int c) {
    string s;

    if (c) s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=White,shape=box]";
    else s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=gray75,shape=box]";

    return s;
}
