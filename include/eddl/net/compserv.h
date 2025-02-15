/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 1.1
* copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
* Date: March 2022
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/

#ifndef EDDL_COMPSERV_H
#define EDDL_COMPSERV_H

#include <cstdio>
#include <string>
#include <vector>


using namespace std;

class CompServ {
public:
    string type; // "local" or "distributed"
    string hw; // CPU, GPU, FPGA
    vector<string> hw_supported; // List of supported hardware for which this library is compiled

    int threads_arg; // The value passed to the constructor
    int local_threads;
    vector<int> local_gpus;
    vector<int> local_fpgas;
    int lsb; // local sync batches
    bool isshared;

    // memory requirements level
    // 0: full memory. better performance in terms of speed
    // 1: mid memory. some memory improvements to save memory
    // 2: low memory. save memory as much as possible
    int mem_level;

    CompServ();
    CompServ * share();
    CompServ * clone();

    // for local
    CompServ(int threads, const vector<int>& gpus, const vector<int> &fpgas, int lsb=1, int mem=0);

    // for Distributed
    explicit CompServ(const string& filename);
};

#endif  //EDDL_COMPSERV_H
