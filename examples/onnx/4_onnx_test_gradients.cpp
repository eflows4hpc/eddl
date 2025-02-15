/*
 * EDDL Library - European Distributed Deep Learning Library.
 * Version: 1.0
 * copyright (c) 2021, Universitat Politècnica de València (UPV), PRHLT Research
 * Centre Date: November 2021 Author: PRHLT Research Centre, UPV,
 * (rparedes@prhlt.upv.es), (jon@prhlt.upv.es) All rights reserved
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "eddl/apis/eddl.h"

#include "eddl/serialization/onnx/eddl_onnx.h" // Not allowed

using namespace eddl;

///////////////////////////////////////////
// 4_onnx_test_gradients.cpp:
// An example on how to use the functions
// for exporting weights and gradients
// using the ONNX format
///////////////////////////////////////////

int main(int argc, char **argv) {
  // Read arguments
  bool export_cpu = false;
  bool import_cpu = false;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--export-cpu") == 0)
      export_cpu = true;
    else if (strcmp(argv[i], "--import-cpu") == 0)
      import_cpu = true;
  }

  // Download mnist
  download_mnist();

  // Settings
  int epochs = 1;
  int batch_size = 100;
  int num_classes = 10;
  CompServ *export_CS = export_cpu ? CS_CPU() : CS_GPU({1});
  CompServ *import_CS = import_cpu ? CS_CPU() : CS_GPU({1});

  // Define network
  layer in = Input({784});
  layer l = in; // Aux var

  l = Reshape(l, {1, 28, 28});
  l = ReLu(Conv(l, 32, {3, 3}, {1, 1}, "same", false));
  l = MaxPool(l, {2, 2});
  l = ReLu(Conv(l, 32, {3, 3}, {1, 1}));
  l = MaxPool(l, {2, 2});

  l = Flatten(l);
  l = Dense(l, 128, false);
  layer out = Softmax(Dense(l, num_classes));

  cout << "Creating model" << endl;
  model net = Model({in}, {out});
  cout << "Model created" << endl;

  // Build model
  cout << "Building the model" << endl;
  build(net,
        adam(0.001),              // Optimizer
        {"soft_cross_entropy"},   // Losses
        {"categorical_accuracy"}, // Metrics
        export_CS,                // Computing service
        true                      // Enable parameters initialization
  );
  cout << "Model is correctly built" << endl;

  cout << "Enabling distributed training" << endl;
  net->enable_distributed();
  cout << "Distributed training enabled" << endl;

  // Export the net before training
  void *serialized_net;
  cout << "Serializing net (without training) to pointer" << endl;
  size_t model_size = serialize_net_to_onnx_pointer(net, serialized_net, false);
  cout << "Net serialized to pointer" << endl;

  // View model
  summary(net);

  // Load dataset
  Tensor *x_train = Tensor::load("mnist_trX.bin");
  Tensor *y_train = Tensor::load("mnist_trY.bin");
  Tensor *x_test = Tensor::load("mnist_tsX.bin");
  Tensor *y_test = Tensor::load("mnist_tsY.bin");

  // Preprocessing
  x_train->div_(255.0f);
  x_test->div_(255.0f);

  // Train model
  cout << "Training the first model" << endl;
  fit(net, {x_train}, {y_train}, batch_size, epochs);

  // Evaluate
  cout << "Evaluating the first model" << endl;
  evaluate(net, {x_test}, {y_test}, batch_size);

  // Export gradients
  void *serialized_gradients;
  string path("mnist.onnx");
  cout << "Exporting gradients" << endl;
  size_t gradients_size = serialize_net_to_onnx_pointer(net, serialized_gradients, true);
  cout << "Gradients exported" << endl;

  // Export trained model
  void *serialized_net_once_trained;
  cout << "Exporting trained weights" << endl;
  size_t snet_size = serialize_net_to_onnx_pointer(net, serialized_net_once_trained, false);
  cout << "Trained weights exported" << endl;

  // Import net topology without trained weights
  cout << "Importing original net topology (without training)" << endl;
  Net *imported_net = import_net_from_onnx_pointer(serialized_net, model_size);
  cout << "Untrained net imported" << endl;

  // Build model
  cout << "Building the loaded topology" << endl;
  build(imported_net,
        adam(0.001),              // Optimizer
        {"soft_cross_entropy"},   // Losses
        {"categorical_accuracy"}, // Metrics
        import_CS,                // Computing service
        false                     // Disable parameters initialization
  );
  cout << "Model is correctly built" << endl;

  // View loaded model
  summary(imported_net);

  // Evaluate with untrained model
  cout << "Evaluating test with the untrained weights" << endl;
  evaluate(imported_net, {x_test}, {y_test}, batch_size);

  // Apply grads
  cout << "Applying grads from training" << endl;
  apply_grads_from_onnx_pointer(imported_net, serialized_gradients, gradients_size);
  cout << "Grads applied" << endl;

  // Evaluate net with accumulated gradients applied
  cout << "Evaluating test after applying gradients" << endl;
  evaluate(imported_net, {x_test}, {y_test}, batch_size);

  // Set trained weights
  cout << "Putting the trained weights" << endl;
  set_weights_from_onnx_pointer(imported_net, serialized_net_once_trained, snet_size);
  cout << "Trained weights set" << endl;

  // Evaluate with trained weights
  cout << "Evaluating test after putting the trained weights" << endl;
  evaluate(imported_net, {x_test}, {y_test}, batch_size);

  return 0;
}
