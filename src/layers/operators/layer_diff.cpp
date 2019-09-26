
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
//      Salva Carrión: salcarpo@prhlt.upv.es
//      Mario Parreño: maparla@prhlt.upv.es
//
//
// To collaborate please contact rparedes@prhlt.upv.es
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "layer_operators.h"


using namespace std;

int LDiff::total_layers = 0;

/**
  @brief Computes the subtraction operation between two layers

  @param l1 a Layer.
  @param l2 a Layer.
  @param name a name for the operation (predefined as 'diff+TotalDiffLayers')
  @param dev which computing service utilize

  @returns the result of l1-l2 element-wise

  */
LDiff::LDiff(Layer *l1, Layer *l2, string name, int dev): OperatorLayer(name, dev) {
    if(name.empty()) this->name = "diff_" + to_string(++total_layers);
    binary=1;

    input=l1->output;

    output = new Tensor(l1->output->getShape(), dev);
    delta = new Tensor(l1->output->getShape(), dev);

    l1->addchild(this);
    l2->addchild(this);
    addparent(l1);
    addparent(l2);


}

/**
  @brief Computes the subtraction operation between a layer and a float

  @param l a Layer.
  @param k a float.
  @param name a name for the operation (predefined as 'diff+TotalDiffLayers')
  @param dev which computing service utilize

  @returns the result of l-k element-wise over l

  */
LDiff::LDiff(Layer *l, float k, string name, int dev): OperatorLayer(name, dev) {
    if(name.empty()) this->name = "diff" + to_string(++total_layers);
    val=k;

    input=l->output;

    output = new Tensor(l->output->getShape(), dev);
    delta = new Tensor(l->output->getShape(), dev);

    l->addchild(this);
    addparent(l);
}

void LDiff::forward(){


    if (binary) Tensor::add(1.0, parent[0]->output, -1.0, parent[1]->output, output, 0);
    else {
        Tensor::copy(parent[0]->output,output);
        output->add_(-val);

    }
}

void LDiff::backward(){
    Tensor::inc(delta,parent[0]->delta);
    if (binary) {
        delta->mult_(-1.0);
        Tensor::inc(delta,parent[1]->delta);
    }
}

Layer *LDiff::share(int c, int bs, vector<Layer *> p) {
  return clone(c,bs,p,dev);
}

Layer *LDiff::clone(int c, int bs, vector<Layer *> p, int todev) {
    LDiff *n;
    if (binary)
        n = new LDiff(p[0], p[1],"share_" + to_string(c) + name, todev);
    else
        n = new LDiff(p[0],val,"share_" + to_string(c) + name, todev);
    n->orig = this;
    return n;
}
