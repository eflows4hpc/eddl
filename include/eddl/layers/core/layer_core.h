/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 1.1
* copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
* Date: March 2022
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#ifndef EDDL_LAYER_CORE_H
#define EDDL_LAYER_CORE_H

#include <string>
#include <cstdio>

#include "eddl/layers/layer.h"

#define TRMODE 1
#define TSMODE 0

using namespace std;

/// Tensor Layer
class LTensor : public LinLayer {
public:

    Tensor *data;

    static int total_layers;

    LTensor(string fname);
    ~LTensor() override;

    LTensor(vector<int> shape, int dev, int mem);

    LTensor(const vector<int> shape, float *fptr,int dev, int mem);

    LTensor *fromCSV(string fname);

    explicit LTensor(Layer *l);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void info() override {}

    void forward() override {}

    void backward() override {}

    string plot(int c) override { return ""; }

    LTensor operator+(LTensor L);


};

/// INPUT Layer
class LInput : public LinLayer {
public:
    static int total_layers;

    LInput(Tensor *in, string name, int dev, int mem);
    ~LInput() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void free_delta() override;

    void forward() override;

    void backward() override;

    string plot(int c) override;

};

/// EMBEDDING Layer
class LEmbedding : public LinLayer {
public:
    int dim;
    int vocsize;
    int length;
    bool mask_zeros;
    Tensor *E;
    Tensor *gE;
    Tensor *acc_gE;
    vector<int> sind;
    static int total_layers;

    LEmbedding(Layer *parent, int vocsize, int lenght, int dim, bool mask_zeros, string name, int dev, int mem);

    ~LEmbedding() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void forward() override;

    void backward() override;

    void update_weights(vector<Tensor*> weights) override;

    void accumulate_accumulated_gradients(vector<Tensor*> grads) override;

    void reset_accumulated_gradients() override;

    void apply_accumulated_gradients() override;

    void enable_distributed() override;

    string plot(int c) override;

};

/// Dense Layer
class LDense : public LinLayer {
public:
    static int total_layers;
    int ndim;
    bool use_bias;  // TODO: Implement
    vector<int> inshape, outshape;

	// Params
	Tensor *W;
	Tensor *gW;
	Tensor *acc_gW;
	Tensor *bias;
	Tensor *gbias;
	Tensor *acc_gbias;

    LDense(Layer *parent, int ndim, bool use_bias, string name, int dev, int mem);

    ~LDense() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;


    void forward() override;

    void backward() override;

    void resize(int batch) override;

	// Sets the weights to the values of the parameter w
	void update_weights(vector<Tensor*> weights) override;

	// Adds the values of gw to the current weights of the layer
	void accumulate_accumulated_gradients(vector<Tensor*> grads) override;

	// Sets to 0.0 the tensors with the accumulated gradients for W and bias
	void reset_accumulated_gradients() override;

	void apply_accumulated_gradients() override;

    string plot(int c) override;

	static void reset_name_counter();

	void enable_distributed() override;

};

/// Activation Layer
class LActivation : public LinLayer {
public:
    string act;
    static int total_layers;
    vector<float> params;
#ifdef cCUDNN
    //Softmax
    cudnnSoftmaxAlgorithm_t  algorithm;
    cudnnSoftmaxMode_t  softmax_mode;

    //Other Activations
    cudnnActivationDescriptor_t activationDesc;
    cudnnActivationMode_t mode;
    cudnnNanPropagation_t reluNanOpt;
    double coef;

    //BOTH softmax and activations
    cudnnTensorDescriptor_t    xDesc;
    cudnnTensorDescriptor_t    yDesc;

    cudnnDataType_t data_type;
    cudnnTensorFormat_t tensor_format;

#endif
    LActivation(Layer *parent, string act, vector<float> params, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void save(std::ofstream &ofs, string format) override;
    void load(std::ifstream &ifs, string format) override;

#ifdef cCUDNN
    void resize(int batch) override;
#endif

    void forward() override;

    void backward() override;

    string plot(int c) override;

};

/// Reshape Layer
class LReshape : public LinLayer {
public:
    static int total_layers;
    vector<int> ls;

    // constructors and clones
    LReshape(Layer *parent, vector<int> shape, string name, int dev, int mem);
    ~LReshape() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    // implementation
    void mem_delta() override;
    void free_delta() override;

    void forward() override;

    void backward() override;

    void resize(int batch) override;

    string plot(int c) override;

};


class LRepeat : public LinLayer {
public:
    static int total_layers;
    RepeatDescriptor *rd;

    // constructors and clones
    LRepeat(Layer *parent, const vector<unsigned int>& repeats, unsigned int axis, string name, int dev, int mem);
    LRepeat(Layer *parent, unsigned int repeats, unsigned int axis, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    ~LRepeat() override;

    void forward() override;

    void backward() override;

    string plot(int c) override;

};

class LTile : public LinLayer {
public:
    static int total_layers;
    TileDescriptor *td;

    // constructors and clones
    LTile(Layer *parent, const vector<int>& repeats, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    ~LTile() override;

    void forward() override;

    void backward() override;

    string plot(int c) override;

};


class LBroadcast : public LinLayer {
public:
    static int total_layers;
    TileDescriptor *td;
    bool shapes_swapped;
    Layer *p1;  // Small
    Layer *p2;  // Big

    // constructors and clones
    LBroadcast(Layer *parent1, Layer *parent2, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    ~LBroadcast() override;

    void forward() override;

    void backward() override;

    string plot(int c) override;

};

class LBypass : public LinLayer {
public:
    static int total_layers;
    string bypass_name;

    // constructors and clones
    LBypass(Layer *parent, string bypass_name, string name, int dev, int mem);
    ~LBypass() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    // implementation
    void mem_delta() override;
    void free_delta() override;

    void forward() override;

    void backward() override;

    string plot(int c) override;

};

/// UpSampling3D Layer
class LUpSampling3D : public LinLayer {
public:
    static int total_layers;
    vector<int> new_shape;
    bool reshape;
    WrappingMode da_mode;
    float cval;
    TransformationMode coordinate_transformation_mode;

    LUpSampling3D(Layer *parent, vector<int> new_shape, bool reshape, WrappingMode da_mode, float cval, TransformationMode coordinate_transformation_modem, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void forward() override;

    void backward() override;

    string plot(int c) override;
};

/// Resize Layer
class LResize : public LinLayer {
public:
    static int total_layers;
    vector<int> new_shape;
    bool reshape;
    WrappingMode da_mode;
    float cval;
    TransformationMode coordinate_transformation_mode;

    LResize(Layer *parent, vector<int> new_shape, bool reshape, WrappingMode da_mode, float cval, TransformationMode coordinate_transformation_modem, string name, int dev, int mem);

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    void forward() override;

    void backward() override;

    string plot(int c) override;
};

/// Squeeze Layer
class LSqueeze : public LinLayer {
public:
    static int total_layers;
    int axis;

    // constructors and clones
    LSqueeze(Layer *parent, int axis, string name, int dev, int mem);
    ~LSqueeze() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    // implementation
    void mem_delta() override;
    void free_delta() override;

    void forward() override;

    void backward() override;

    void resize(int batch) override;

    string plot(int c) override;

};

/// Unsqueeze Layer
class LUnsqueeze : public LinLayer {
public:
    static int total_layers;
    int axis;

    // constructors and clones
    LUnsqueeze(Layer *parent, int axis, string name, int dev, int mem);
    ~LUnsqueeze() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    // implementation
    void mem_delta() override;
    void free_delta() override;

    void forward() override;

    void backward() override;

    void resize(int batch) override;

    string plot(int c) override;

};


/// Drop-out Layer
class LDropout : public LinLayer {
public:
    static int total_layers;
    bool iw; //inference weighting
    
    // constructors and clones
    LDropout(Layer *parent, float df, bool iw, string name, int dev, int mem);
    ~LDropout() override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;

    float df;
    Tensor *mask;

    // implementation
    void forward() override;

    void backward() override;
    void resize(int batch) override;
    string plot(int c) override;

};


/// Select Layer
class LSelect : public LinLayer {
public:
    static int total_layers;
    SelDescriptor *sd;

    LSelect(Layer *l, vector<string> indices, string name, int dev, int mem);

    ~LSelect() override;

    void forward() override;

    void backward() override;

    void resize(int b) override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;
};


/// Permute Layer
class LPermute : public LinLayer {
public:
    static int total_layers;

    PermuteDescriptor *sd;

    LPermute(Layer *l, vector<int> dims, string name, int dev, int mem);

    ~LPermute() override;

    void forward() override;

    void backward() override;

    void resize(int b) override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;
};

/// Transform Layer
class LTransform : public LinLayer {
public:
    static int total_layers;
    int copy_cpu_to_fpga;
    int copy_fpga_to_cpu;
    int transform;

    LTransform(Layer *l, int copy_cpu_to_fpga, int copy_fpga_to_cpu, int transform, string name, int dev, int mem);

    ~LTransform() override;

    void forward() override;

    void backward() override;

    void resize(int b) override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;
};

/// Split Layer
class LSplit : public LinLayer {
public:
    static int total_layers;
    vector<int> indexes;
    int axis;
    vector<Layer*> split_layers;
    bool merge_sublayers;

    LSplit(Layer *l, vector<int> indexes, int axis,  bool merge_sublayers, string name, int dev, int mem);

    ~LSplit() override;

    void forward() override;

    void backward() override;

    void resize(int b) override;

    Layer *share(int c, int bs, vector<Layer *> p) override;

    Layer *clone(int c, int bs, vector<Layer *> p, int todev) override;
};
#endif //EDDL_LAYER_CORE_H
