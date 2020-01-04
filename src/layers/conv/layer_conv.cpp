/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.3
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "layer_conv.h"

using namespace std;


int LConv::total_layers = 0;

// constructors and clones

LConv::LConv(Layer *parent, const vector<int> &ks, const vector<int> &st,
             const vector<int> &p, string name, int dev) : LConv(parent, new ConvolDescriptor(ks, st, p), name, dev) {}

LConv::LConv(Layer *parent, int filters, const vector<int> &kernel_size, const vector<int> &strides, string padding,
int groups, const vector<int> &dilation_rate, bool use_bias, string name, int dev) : LConv(parent, new ConvolDescriptor(filters, kernel_size, strides, padding), name, dev) {
    // TODO: Implement (Fix initialization)
};

LConv::LConv(Layer *parent, ConvolDescriptor *D, string name, int dev) : LinLayer(name, dev) {
    if (parent->output->ndim != 4) msg("LConv only works over 4D tensors", "LConv::LConv");

    // Check dev with tensor dev

    // Set default name
    if(name.empty()) this->name = "conv" + to_string(++total_layers);

    cd = D;

    input = parent->output;
    cd->build(input);

    output = cd->O;
    delta = cd->D;
    cd->ID = parent->delta;

    params.push_back(cd->K);
    params.push_back(cd->bias);

    gradients.push_back(cd->gK);
    gradients.push_back(cd->gbias);

    parent->addchild(this);
    addparent(parent);

}


// virtual
void LConv::resize(int batch){
    cd->resize(batch);
}

void LConv::forward() {
    Conv2D(this->cd);
}

void LConv::backward() {

    //get gradients with provided delta
    if (trainable) Conv2D_grad(this->cd);
    
    // backprop delta
    if (this->parent.size()) {
        Conv2D_back(this->cd);
    }

    // Regularizer
    if (trainable) if(reg!= nullptr) {reg->apply(cd->K);}

}

Layer *LConv::share(int c, int bs, vector<Layer *> p) {
    LConv *n = new LConv(p[0], cd->ksize, cd->stride, cd->pad, "share_" + to_string(c) + name, dev);
    n->orig = this;

    //share params
    for (int i = 0; i < n->params.size(); i++) delete n->params[i];
    n->params.clear();

    n->cd->K = cd->K;
    n->cd->bias = cd->bias;
    new(&n->cd->matK) Eigen::Map<Eigen::MatrixXf>(n->cd->K->ptr, cd->kr * cd->kc * cd->kz, cd->nk);

    n->params.push_back(n->cd->K);
    n->params.push_back(n->cd->bias);

    n->reg=reg;
    n->init=init;

    return n;
}

Layer *LConv::clone(int c, int bs, vector<Layer *> p, int todev) {
    LConv *n = new LConv(p[0], cd->ksize, cd->stride, cd->pad, "clone_" + to_string(todev) + name, todev);
    n->orig = this;

    n->reg=reg;
    n->init=init;

    return n;
}


string LConv::plot(int c) {
    string s;

    if (c) s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=gray,shape=box]";
    else s = name + " [label=" + "\"" + name + "\",style=filled,fontsize=12,fillcolor=White,shape=box]";

    return s;
}
