/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.1
* copyright (c) 2019, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: October 2019
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "initializer.h"

using namespace std;


IRandomUniform::IRandomUniform(float minval, float maxval, int seed) : Initializer("random_uniform") {
    // Todo: Implement
    this->minval = minval;
    this->maxval = maxval;
    this->seed = seed;

}
void IRandomUniform::apply(Tensor* params){}
