/*
 * EDDL Library - European Distributed Deep Learning Library.
 * Version: 1.1
 * copyright (c) 2022, Universitat Politècnica de València (UPV), PRHLT Research Centre
 * Date: March 2022
 * Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
 * 
 * MPI support for EDDL Library - European Distributed Deep Learning Library.
 * Version: 
 * copyright (c) 2022, Universidad Politécnica de Valencia (UPV), GAP research group
 * Date: May 2022
 * Author: GAP Research Group (UPV), contact: plopez@disca.upv.es
 * All rights reserved
 */


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>



#include "eddl/apis/eddl.h"
#include "eddl/serialization/onnx/eddl_onnx.h" // Not allowed

#include "utils.h"

using namespace eddl;
using namespace std;
using namespace std::chrono;

layer DataAugmentation(layer l) {
    // Data augmentation/
    l = RandomCropScale(l,{0.95f, 1.0f});
    l = RandomVerticalFlip(l);
    //l = RandomShift(l, {-0.05,0.05},{-0.05, 0.05});    
    l = RandomRotation(l,{-5, 5});
    return l;
}


//////////////////////////////////
// mlp
//////////////////////////////////

model mlp(vector<int> in_shape, int num_classes, int size1, int size2) {
    layer in = Input(in_shape, "da");
    //layer in = Input({3, 224, 224});
    layer l = in;


    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"bypass");layer in = Input({channels, height, width});

    l = Flatten(l);
    l = LeakyReLu(Dense(l, size1));
    l = LeakyReLu(Dense(l, size1));
    l = LeakyReLu(Dense(l, size2));

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}

//////////////////////////////////
// mlp
//////////////////////////////////

model mlpbn(vector<int> in_shape, int num_classes, int size1, int size2) {
    layer in = Input(in_shape, "da");
    //layer in = Input({3, 224, 224});
    layer l = in;


    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"bypass");layer in = Input({channels, height, width});
    l = BatchNormalization(l);

    l = Flatten(l);
    l = LeakyReLu(Dense(l, size1));
    l = LeakyReLu(Dense(l, size1));
    l = LeakyReLu(Dense(l, size2));

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}
/////
/////////////////////////////
// vgg16 with BatchNorm
//////////////////////////////////

layer Normalization(layer l) {
    return BatchNormalization(l);
    //return LayerNormalization(l);
    //return GroupNormalization(l,8);
}

layer Block1(layer l, int filters) {
    return ReLu(Normalization(Conv(l, filters,{1, 1},
    {
        1, 1
    }, "same", false)));
}

layer Block3_2(layer l, int filters) {
    l = ReLu(Normalization(Conv(l, filters,{3, 3},
    {
        1, 1
    }, "same", false)));
    l = ReLu(Normalization(Conv(l, filters,{3, 3},
    {
        1, 1
    }, "same", false)));
    return l;
}

//////////////////////////////////
// 
//////////////////////////////////

model vgg16bn(vector<int> in_shape, int num_classes, int size1, int size2) {
    layer in = Input(in_shape, "da");
    //layer in = Input({3, 224, 224});
    layer l = in;


    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"bypass");
    //l = BatchNormalization(l);

    l = MaxPool(Block3_2(l, 64));
    l = MaxPool(Block3_2(l, 128));
    l = MaxPool(Block1(Block3_2(l, 256), 256));
    l = MaxPool(Block1(Block3_2(l, 512), 512));
    l = MaxPool(Block1(Block3_2(l, 512), 512));

    l = Reshape(l,{-1});
    l = Activation(Dense(l, size1), "relu");
    l = Activation(Dense(l, size2), "relu");

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}

//////////////////////////////////
// BN at input
//////////////////////////////////

model vgg16bnbn(vector<int> in_shape, int num_classes, int size1, int size2) {
    layer in = Input(in_shape, "da");
    //layer in = Input({3, 224, 224});
    layer l = in;


    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"bypass");
    l = BatchNormalization(l);

    l = MaxPool(Block3_2(l, 64));
    l = MaxPool(Block3_2(l, 128));
    l = MaxPool(Block1(Block3_2(l, 256), 256));
    l = MaxPool(Block1(Block3_2(l, 512), 512));
    l = MaxPool(Block1(Block3_2(l, 512), 512));

    l = Reshape(l,{-1});
    l = Activation(Dense(l, size1), "relu");
    l = Activation(Dense(l, size2), "relu");

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}

//////////////////////////////////
// Resnet50 with
// BatchNorm
//////////////////////////////////

layer BN(layer l) {
    return BatchNormalization(l);
    //return l;
}

layer BG(layer l) {
    //return GaussianNoise(BN(l),0.3);
    return BN(l);
}

layer ResBlock(layer l, int filters, int half, int expand = 0) {
    layer in = l;

    l = ReLu(BG(Conv(l, filters,{1, 1},
    {
        1, 1
    }, "same", false)));

    if (half)
        l = ReLu(BG(Conv(l, filters,{3, 3},
        {
            2, 2
        }, "same", false)));
    else
        l = ReLu(BG(Conv(l, filters,{3, 3},
        {
            1, 1
        }, "same", false)));

    l = BG(Conv(l, 4 * filters,{1, 1},
    {
        1, 1
    }, "same", false));

    if (half)
        return ReLu(Add(BG(Conv(in, 4 * filters,{1, 1},
        {
            2, 2
        }, "same", false)), l));
    else
        if (expand) return ReLu(Add(BG(Conv(in, 4 * filters,{1, 1},
        {
            1, 1
        }, "same", false)), l));
    else return ReLu(Add(in, l));
}

// Resnet-50

model resnet50bn(vector<int> in_shape, int num_classes, int size1, int size2) {

    layer in = Input(in_shape, "da");
    layer l = in;

    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"by");

    l = ReLu(BG(Conv(l, 64,{3, 3},
    {
        1, 1
    }, "same", false))); //{1,1}
    //l=MaxPool(l,{3,3},{1,1},"same");

    // Add explicit padding to avoid the asymmetric padding in the Conv layers
    l = Pad(l,{0, 1, 1, 0});

    for (int i = 0; i < 3; i++)
        l = ResBlock(l, 64, 0, i == 0); // not half but expand the first

    for (int i = 0; i < 4; i++)
        l = ResBlock(l, 128, i == 0);

    for (int i = 0; i < 6; i++)
        l = ResBlock(l, 256, i == 0);

    for (int i = 0; i < 3; i++)
        l = ResBlock(l, 512, i == 0);

    l = MaxPool(l,{4, 4}); // should be avgpool

    l = Reshape(l,{-1});
    l = Activation(Dense(l, size1), "relu");
    l = Activation(Dense(l, size2), "relu");

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}

// Resnet-50

model resnet50bnbn(vector<int> in_shape, int num_classes, int size1, int size2) {

    layer in = Input(in_shape, "da");
    layer l = in;

    // Data augmentation
    //l= DataAugmentation(l);
    //l=Bypass(l,"by");
     l = BatchNormalization(l);

    l = ReLu(BG(Conv(l, 64,{3, 3},
    {
        1, 1
    }, "same", false))); //{1,1}
    //l=MaxPool(l,{3,3},{1,1},"same");

    // Add explicit padding to avoid the asymmetric padding in the Conv layers
    l = Pad(l,{0, 1, 1, 0});

    for (int i = 0; i < 3; i++)
        l = ResBlock(l, 64, 0, i == 0); // not half but expand the first

    for (int i = 0; i < 4; i++)
        l = ResBlock(l, 128, i == 0);

    for (int i = 0; i < 6; i++)
        l = ResBlock(l, 256, i == 0);

    for (int i = 0; i < 3; i++)
        l = ResBlock(l, 512, i == 0);

    l = MaxPool(l,{4, 4}); // should be avgpool

    l = Reshape(l,{-1});
    l = Activation(Dense(l, size1), "relu");
    l = Activation(Dense(l, size2), "relu");

    layer out = Softmax(Dense(l, num_classes), -1, "output");
    // net define input and output layers list
    model net = Model({in},
    {
        out
    });
    return net;
}

model select_model(int choice, vector<int> in_shape, int num_classes, int size1, int size2) {
    if (in_shape[0] != 3) {
        printf("ERROR: Only 3 channels are supported\n");
        exit(1);
    }
    /*
     if (in_shape[1]!=224) {
         printf("ERROR: Only height 224 is supported\n");
         exit(1);
     }
     if (in_shape[2]!=224) {
         printf("ERROR: Only width 224 is supported\n");
         exit(1);
     }
     */
    model pretrained_model;
    switch (choice) {
        case 0: pretrained_model = download_vgg16(true, in_shape);
            printf("model: vgg16\n");
            break;
        case 1: pretrained_model = download_vgg16_bn(true, in_shape);
            printf("model: vgg16bn\n");
            break;
        case 2: pretrained_model = download_vgg19(true, in_shape);
            printf("model: vgg19\n");
            break;
        case 3: pretrained_model = download_vgg19_bn(true, in_shape);
            printf("model: vgg19bn\n");
            break;
        case 4: pretrained_model = download_resnet18(true, in_shape);
            printf("model: resnet18\n");
            break;
        case 5: pretrained_model = download_resnet34(true, in_shape);
            printf("model: resnet34\n");
            break;
        case 6: pretrained_model = download_resnet50(true, in_shape);
            printf("model: resnet50\n");
            break;
        case 7: pretrained_model = download_resnet101(true, in_shape);
            printf("model: resnet101\n");
            break;
        case 8: pretrained_model = download_resnet152(true, in_shape);
            printf("model: resnet152\n");
            break;
        case 9: pretrained_model = download_densenet121(true, in_shape);
            printf("model: densenet121\n");
            break;
        default:
            printf("Model %d unknown\n", choice);
            exit(1);

    }

    // Get the input layer of the pretrained model
    layer in_ = getLayer(pretrained_model, "input");
    // Get the last layer of the pretrained model

    layer top_layer = getLayer(pretrained_model, "top");
    // Create the new densely connected part
    // std::vector<std::string> layers2init{"dense1", "dense_out"};
    //const int input_units = top_layer->output->shape[1];
    //eddl::layer l = eddl::Dense(top_layer, input_units / 2, true, layers2init[0]);
    layer l = nullptr;
    l = Reshape(top_layer,{-1});
    l = Activation(Dense(l, size1, true), "relu");
    l = Activation(Dense(l, size2, true), "relu");
    // l = eddl::Dropout(l, 0.4);
    // Output layer
    layer out_ = Softmax(Dense(l, num_classes, true, "dense_out"), -1, "output");

    model net = Model({in_},
    {
        out_
    });
    return net;
}

void save_image_from_tensor(Tensor* X, Tensor* Y, unsigned long int from, unsigned long int to, int aux, char* name) {
    char jpgfile[256];
    char txtfile[256];
    int id = get_id_distributed();

    for (int i = from; i < to; i++) {
        std::string _range_ = std::to_string(i);
        Tensor* xout = X->select({_range_});
        Tensor* yout = Y->select({_range_});
        xout->mult_(255.0f);
        sprintf(jpgfile, "p%d_%s_x_b%d_%d.jpg", id, name, aux, i);
        sprintf(txtfile, "p%d_%s_y_b%d_%d.txt", id, name, aux, i);
        xout->save(jpgfile);
        yout->save(txtfile);
        delete xout;
        delete yout;
    }

}

void train_epoch(model danet, model net, Tensor* x_train, Tensor* y_train, Tensor* xbatch, Tensor* ybatch, int nbpp, int local_batch, bool data_generator, bool distr_dataset = false) {

    int i, j;
    int id = get_id_distributed();
    /*
    int n_procs = get_n_procs_distributed();

    tshape s = x_train->getShape();
    int dataset_size = s[0];
    int channels = s[1];
    int width = s[2];
    int height = s[3];
    int num_classes = y_train->getShape()[1];

    int num_batches;
    int local_batch;
    int global_batch;
    int nbpp;
     */

    int channels = xbatch->getShape()[1];
    int height = xbatch->getShape()[2];
    int width = xbatch->getShape()[3];

    int num_classes = ybatch->getShape()[1];
    int epochs = 1;
    //printf("channels=%d height=%d width=%d classes=%d\n", channels, height, width, num_classes);
    /*
    if (distr_dataset){
        mpi_id0(printf("custom_fit. dataset_size %d  Distributed dataset\n", dataset_size));
        dataset_size=dataset_size*n_procs;
    } else {
       // nbpp=num_batches/n_procs;
    }
    
    num_batches=dataset_size/batch;
    if (divide_batch) {
        global_batch=batch;
        local_batch=global_batch/n_procs;
        nbpp=num_batches;
        mpi_id0(printf("custom_fit. Divide batch: global: %d, local: %d\n", batch, local_batch));
    } else {
        global_batch=batch*n_procs;
        local_batch=global_batch/n_procs;
        nbpp=num_batches/n_procs;
        mpi_id0(printf("custom_fit. Mul batch: global: %d, local: %d\n", local_batch*n_procs, local_batch));
    }
     */
    Tensor* xout = new Tensor({1, channels, height, width});
    Tensor* yout = new Tensor({1, num_classes});

    for (i = 0; i < epochs; i++) {
        high_resolution_clock::time_point e1 = high_resolution_clock::now();
        reset_loss(net);
        mpi_id0(printf("train_epoch. Epoch %d/%d\n", i + 1, epochs));
        double tbsecs = 0;
        double tbgsecs = 0;
        double awsecs = 0;
        for (j = 0; j < nbpp; j++) {
            TIME_POINT1(load);

            if (data_generator) {
                get_batch(xbatch, ybatch);
                xbatch->div_(255.0f);
            } else {
                next_batch({x_train, y_train},
                {
                    xbatch, ybatch
                });
            }
            TIME_POINT2(load, tbgsecs);
            TIME_POINT1(train);

            if ((1)&(j < 2)) {
                save_image_from_tensor(xbatch, ybatch, 0, 5, j, "input");
            }

            // DA
            forward(danet, vector<Tensor *>{xbatch});
            // get COPIES of tensors from DA
            layer output_da = getLayer(danet, "bypass1");
            Tensor* xbatch_da = getOutput(output_da);

            if ((1)&(j < 2)) {
                save_image_from_tensor(xbatch, ybatch, 0, 5, j, "input_da");
            }

            train_batch(net,{xbatch_da},
            {
                ybatch
            });
            //sync_batch
            TIME_POINT2(train, tbsecs);
            TIME_POINT1(avg_w);
            avg_weights_distributed(net, j + 1, nbpp);
            TIME_POINT2(avg_w, awsecs);
            print_loss(net, j + 1, false);
            int batches_processed = j + 1;
            mpi_id0(printf(" Avg elapsed time: load % 1.4f train %1.4f secs; comms %1.4f\n", tbgsecs / batches_processed, tbsecs / batches_processed, awsecs / batches_processed));
            mpi_id0(printf("\r"););
            delete xbatch_da;
            //printf("Proc %d, j=%d\n", id, j);
        }
        mpi_id0(printf("\n"));
        // if (early_stopping_on_loss_var (net, 0, 10, 0.1, i)) break;
        //if (early_stopping_on_metric_var (net, 0, 0.0001, 2, i)) break;
        //if (early_stopping_on_metric (net, 0, 0.97, 2, i)) break;
        high_resolution_clock::time_point e2 = high_resolution_clock::now();
        duration<double> epoch_time_span = e2 - e1;
        if (id == 0) {
            fprintf(stdout, "\n%1.4f secs/epoch: load % 1.4f secs; train: %1.4f secs; comms: %1.4f\n", epoch_time_span.count(), tbgsecs, tbsecs, awsecs);
            fflush(stdout);
        }
        set_batch_avg_overhead_distributed(tbsecs, awsecs, nbpp, 0.1);
    }
    mpi_id0(printf("\n"));
    mpi_id0(fflush(stdout));

    delete xout;
    delete yout;

}

void eval_epoch(model net, Tensor* x_test, Tensor* y_test, Tensor* xbatch, Tensor* ybatch, int nbpp, int local_batch, bool data_generator, bool distr_dataset = false) {

    int i, j, k;
    int id = get_id_distributed();



    vind sind;
    if (data_generator == false) {
        for (k = 0; k < local_batch; k++)
            sind.push_back(0);
    }

    reset_loss(net);
    mpi_id0(printf("eval_epoch\n"));
    for (j = 0; j < nbpp; j++) {
        if (data_generator) {
            get_batch(xbatch, ybatch);
            xbatch->div_(255.0f);
            eval_batch(net,{xbatch},{ybatch});
        } else {

            for (k = 0; k < local_batch; k++) {
                if (distr_dataset)
                    sind [k] = (j * local_batch) + k;
                else
                    sind [k] = (((id * nbpp) + j) * local_batch) + k;
                //printf("id=%d  %5d",id, sind[k]);
            }
            eval_batch(net,{x_test},
            {
                y_test
            }, sind);



        }

        //         print_loss(batches, num_batches);
        print_loss(net, j + 1, false);
        mpi_id0(fprintf(stdout, "\r"));
        mpi_id0(fflush(stdout));
    }
    // sync processes and print final results
    avg_metrics_distributed(net);
    print_loss(net, nbpp, false);
    mpi_id0(printf("\n"));
    mpi_id0(fflush(stdout));

    // if (early_stopping_on_loss_var (net, 0, 10, 0.1, i)) break;
    //if (early_stopping_on_metric_var (net, 0, 0.0001, 2, i)) break;
    //if (early_stopping_on_metric (net, 0, 0.97, 2, i)) break;
}

void custom_evaluate(model net, Tensor* x_test, Tensor* y_test, Tensor* xbatch, Tensor* ybatch, int batch, bool divide_batch, bool distr_dataset = false) {
    int i, j, k;
    int id = get_id_distributed();
    int n_procs = get_n_procs_distributed();

    tshape s = x_test->getShape();
    int dataset_size = s[0];
    /*
    int channels = s[1];
    int width = s[2];
    int height = s[3];
    int num_classes = y_test->getShape()[1];
     */
    int num_batches;
    int local_batch;
    int global_batch;
    int nbpp;


    int channels = xbatch->getShape()[1];
    int height = xbatch->getShape()[2];
    int width = xbatch->getShape()[3];

    int num_classes = ybatch->getShape()[1];

    Tensor* xout = new Tensor({1, channels, height, width});
    Tensor* yout = new Tensor({1, num_classes});

    set_batch_distributed(&global_batch, &local_batch, batch, divide_batch);
    nbpp = set_NBPP_distributed(dataset_size, local_batch, distr_dataset);

    vind sind;
    for (k = 0; k < local_batch; k++)
        sind.push_back(0);

    reset_loss(net);
    mpi_id0(printf("custom_evaluate\n"));
    for (j = 0; j < nbpp; j++) {
        /*
        for (k = 0; k < local_batch; k++) {
            if (distr_dataset)
                sind [k] = (j * local_batch) + k;
            else
                sind [k] = (((id * nbpp) + j) * local_batch) + k;
            //printf("id=%d  %5d",id, sind[k]);
        }
        eval_batch(net,{x_test},
        {
            y_test
        }, sind);
         */
        next_batch({x_test, y_test},
        {
            xbatch, ybatch
        });

        if ((1)&(j < 5)) {
            save_image_from_tensor(xbatch, ybatch, 0, 5, j, "eval");
        }

        eval_batch(net,{xbatch},
        {
            ybatch
        });
        //         print_loss(batches, num_batches);
        print_loss(net, j + 1, false);
        mpi_id0(fprintf(stdout, "\r"));
        mpi_id0(fflush(stdout));
    }
    // sync processes and print final results
    avg_metrics_distributed(net);
    print_loss(net, nbpp, false);
    mpi_id0(printf("\n"));
    mpi_id0(fflush(stdout));
    delete xout;
    delete yout;

    // if (early_stopping_on_loss_var (net, 0, 10, 0.1, i)) break;
    //if (early_stopping_on_metric_var (net, 0, 0.0001, 2, i)) break;
    //if (early_stopping_on_metric (net, 0, 0.97, 2, i)) break;
}

int main(int argc, char **argv) {
    int id;
    char model_name[256];
    char pdf_name[256];
    char import_onnx[256];
    char test_file[256];


    char path[256] = "";
    char tr_images[256];
    char tr_labels[256];
    char ts_images[256];
    char ts_labels[256];
    char classes[256];


    int epochs = 32;
    int batch_size = 100;
    int num_classes = 4;
    int channels = 1;
    int width = 256;
    int height = 256;
    float lr = 0.001;
    int method = FIXED;
    int initial_mpi_avg = 1;
    int chunks = 1;
    int use_bi8 = 0;
    int use_distr_dataset = 0;
    int ptmodel = 1;
    bool use_cpu = false;
    int use_mpi = false;
    int dgt = 0;
    bool use_dg;

    int dataset_size = 0;
    int num_batches;
    int local_batch;
    int global_batch;
    int nbpp;

    double secs;
    double secs_epoch;
    
    int dg_version=1;

    int test_only = 0;
    int initialize=true;
    
    struct DG_Data Train;
    struct DG_Data Val;
    

    // Auxiliary variables to store the results
    vector<float> losses, accs, val_losses, val_accs;
    // To track the best models to store them in ONNX
    float best_loss = std::numeric_limits<float>::infinity();
    float best_acc = 0.f;
    float curr_loss = std::numeric_limits<float>::infinity();
    float curr_acc = 0.f;
    // Paths to the current best checkpoints
    std::string best_model_byloss;
    std::string best_model_byacc;

    process_arguments(argc, argv,
            path, tr_images, tr_labels, ts_images, ts_labels,
            &epochs, &batch_size, &num_classes, &channels, &width, &height, &lr,
            &method, &initial_mpi_avg,
            &chunks, &use_bi8, &use_distr_dataset, &ptmodel, test_file,
            &use_cpu, &use_mpi, &dgt);

    // Init distribuited training
    if (use_mpi == 1)
        id = init_distributed("MPI");
    else if (use_mpi == 2)
        id = init_distributed("MPI-NCA");
    else
        id = init_distributed();



    use_dg = dgt > 0;



    sprintf(classes, "%s/%s", path, "classes.txt");

    sprintf(model_name, "generic%d", ptmodel);
    sprintf(pdf_name, "%s.pdf", model_name);
    //sprintf(onnx_name, "%s%d.onnx", model_name, ptmodel);




    // Sync method
    set_avg_method_distributed(method, initial_mpi_avg);
    //set_method_distributed(FIXED, initial_mpi_avg, 1);


    // network
    auto in_shape = {channels, height, width};
    /*
    layer in = Input({channels, width, height});
    //layer in = Input({3, 224, 224});
    layer l = in;


    // Data augmentation
    //l = RandomCropScale(l, {0.8f, 1.0f});
    //l = RandomFlip(l,1);

    l=MaxPool(Block3_2(l,64));
    l=MaxPool(Block3_2(l,128));
    l=MaxPool(Block1(Block3_2(l,256),256));
    l=MaxPool(Block1(Block3_2(l,512),512));
    l=MaxPool(Block1(Block3_2(l,512),512));

    l=Reshape(l,{-1});
    l = Activation(Dense(l, 4096), "relu");
    l = Activation(Dense(l, 4096), "relu");

    layer out = Softmax(Dense(l, num_classes));
    // net define input and output layers list
    model net = Model({in},{out});
     */

    // Data augmentation network
    layer in = Input({channels, height, width});
    layer l = in;
    l = DataAugmentation(l);
    layer out = Bypass(l);
    model danet = Model({in},
    {
        out
    });
    build(danet);

    // NN
    int dense_size1 = 512;
    int dense_size2 = 512;
    model net;
    if (ptmodel == 100) {// load ONNX file for testing only
        printf("model: custom ONNX, testing\n");
        net = import_net_from_onnx_file(test_file);
        initialize = false;
        test_only = 1;
    } else if (ptmodel == 101) {// load ONNX file
        printf("model: custom ONNX\n");
        net = import_net_from_onnx_file(test_file);
        initialize = false;
    } else if (ptmodel == 10) {
         printf("model: vgg16bn \n");
        net = vgg16bn(in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    } else if (ptmodel == 11) {// resnet50
          printf("model: resnet50bn\n");
        net = resnet50bn(in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    } else if (ptmodel == 12) {// mlp
         printf("model: mlp\n");
        net = mlp(in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    }     else if (ptmodel == 13) {
         printf("model: vgg16bn with  BN at input\n");
        net = vgg16bnbn(in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    }     else if (ptmodel == 14) {
         printf("model: resnet50bn with BN at input\n");
        net = resnet50bnbn(in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    } else if (ptmodel == 15) {// mlp
         printf("model: mlp with BN at input\n");
        net = mlpbn (in_shape, num_classes, dense_size1, dense_size2);
        initialize = true;
    } else { // onnx models
        net = select_model(ptmodel, in_shape, num_classes, dense_size1, dense_size2);
        initialize = false;
    }

    compserv cs = nullptr;
    if (use_cpu) {
        cs = CS_CPU();
    } else {
        cs = CS_GPU(); // one GPU
    }
    
    // Build model
    build(net,
            //            adam(0.0001), // Optimizer
            adam(lr), // Optimizer
            //sgd(lr), // Optimizer
    {"softmax_cross_entropy"}, // Losses
    {"categorical_accuracy"}, // Metrics
    cs, initialize);

    setlogfile(net, model_name);

    // plot the model
    if (id == 0)
        plot(net, pdf_name);

    // get some info from the network
    if (id == 0) {
        summary(danet);
        summary(net);
    }

    // Load val dataset
    Tensor* x_test = Tensor::load(ts_images);
    Tensor* y_test = Tensor::load(ts_labels);
    x_test->div_(255.0f);

    // This code will be removed once onnx import is working
    //if (strlen(test_file)!=0) { // Load weights and test
    //    load(net,test_file);
    //    test_only=1;
    //}
    set_batch_distributed(&global_batch, &local_batch, batch_size, DIV_BATCH);
    Tensor* xbatch = new Tensor({local_batch, channels, height, width});
    Tensor* ybatch = new Tensor({local_batch, num_classes});

       
    if (test_only == 0) {
        // Later, we fill the training dataset
        Tensor* x_train;
        Tensor* y_train;

        bcast_weights_distributed(net);
       
        if (chunks == 1) {
            /* Load dataset */
            if (use_dg == 0) {
                x_train = Tensor::load(tr_images);
                y_train = Tensor::load(tr_labels);
                x_train->div_(255.0f);
                tshape s = x_train->getShape();
                dataset_size = s[0];
            }

            if (use_dg) {
                int num_batches;
                prepare_data_generator(DG_TRAIN, tr_images, tr_labels, local_batch, use_distr_dataset, &dataset_size, &nbpp, DG_PERFECT, dgt, 8);
            } else {
                nbpp = set_NBPP_distributed(dataset_size, local_batch, use_distr_dataset);
            }

            for (int epoch = 0; epoch < epochs; epoch++) {
                mpi_id0(printf("== Epoch %d/%d ===\n", epoch + 1, epochs));
                //for (int chunk = 0; chunk < chunks; chunk++) {
                if (use_dg)
                    start_data_generator();

                int chunk = 0;
                //    mpi_id0(printf("-- Chunk %d/%d ---\n", chunk + 1, chunks));
                if (use_distr_dataset) { /* Distribute dataset into processes */
                    sprintf(tr_images, "%s/%03d/train-images.bi8", path, id * chunks + chunk);
                    sprintf(tr_labels, "%s/%03d/train-labels.bi8", path, id * chunks + chunk);
                } else {
                    //if (chunks == 1) {
                    sprintf(tr_images, "%s/train-images.bi8", path);
                    sprintf(tr_labels, "%s/train-labels.bi8", path);
                    //} else { 
                    sprintf(tr_images, "%s/%03d/train-images.bi8", path, chunk);
                    sprintf(tr_labels, "%s/%03d/train-labels.bi8", path, chunk);
                    //}
                }
                
                if (id == 0) {
                    //printf("Train: %s, %s\n ", tr_images, tr_labels);
                    //printf("Val: %s, %s\n", ts_images, ts_labels);
                }


                // Train
                // printf("FIT:\n");
                //fit(net,{x_train},{y_train}, batch_size, epochs);

                //mpi_id0(printf("CUSTOM FIT Mul :\n"));        
                //custom_fit(net,{x_train},{y_train},batch_size, epochs, false, use_distr_dataset);

                TIMED_EXEC("CUSTOM TRAIN_BATCH", train_epoch(danet, net,{x_train},
                {
                    y_train
                },
                {
                    xbatch
                },
                {
                    ybatch
                }, nbpp, local_batch, use_dg, use_distr_dataset), secs_epoch);

                /*
                TIMED_EXEC("CUSTOM FIT MUL_BATCH", custom_fit(danet, net,{x_train},
                {
                    y_train
                }, batch_size, 1, MUL_BATCH, use_distr_dataset), secs_epoch);
                 */


                update_batch_avg_distributed(epoch, secs_epoch, 1000);
                barrier_distributed();

                TIMED_EXEC("CUSTOM EVALUATE", custom_evaluate(net,{x_test},
                {
                    y_test
                },
                {
                    xbatch
                },
                {
                    ybatch
                }, batch_size, DIV_BATCH, false), secs);
                
                barrier_distributed();
       
                TIMED_EXEC("EVALUATE", evaluate(net,{x_test},
                {
                y_test
                }), secs);

                //TIMED_EXEC("DISTR EVALUATE", evaluate_distr(net,{x_test},{y_test}), secs);              
                barrier_distributed();

                if (id == 0) {
                    // Get the current losses and metrics
                    curr_loss = get_losses(net)[0];
                    curr_acc = get_metrics(net)[0];
                    // Check if we have to save the current model as ONNX
                    if (curr_loss < best_loss || curr_acc > best_acc) {
                        // Prepare the onnx file name
                        char onnx_name[128];
                        sprintf(onnx_name, "%s_epoch%d_loss-%1.2f_acc-%.3f", model_name, epoch, curr_loss, curr_acc);

                        // Update the current best metrics and finish ONNX file name
                        char onnx_fname[128];
                        char weights_fname[128];
                        if (curr_loss >= best_loss) { // Only improves acc
                            best_acc = curr_acc;
                            sprintf(weights_fname, "%s_by-acc.bin", onnx_name);
                            sprintf(onnx_fname, "%s_by-acc.onnx", onnx_name);
                            std::cout << "New best model by acc: \"" << onnx_fname << "\"\n\n";
                        } else if (curr_acc <= best_acc) { // Only improves loss
                            best_loss = curr_loss;
                            sprintf(weights_fname, "%s_by-loss.bin", onnx_name);
                            sprintf(onnx_fname, "%s_by-loss.onnx", onnx_name);
                            std::cout << "New best model by loss: \"" << onnx_fname << "\"\n\n";
                        } else { // Improves loss and acc
                            best_acc = curr_acc;
                            best_loss = curr_loss;
                            sprintf(weights_fname, "%s_by-loss-and-acc.bin", onnx_name);
                            sprintf(onnx_fname, "%s_by-loss-and-acc.onnx", onnx_name);
                            std::cout << "New best model by loss and acc: \"" << onnx_fname << "\"\n\n";
                        }
                        save(net, weights_fname);
                        save_net_to_onnx_file(net, onnx_fname);
                    }
                }
                barrier_distributed();

                if (id == 0) {
                    // Predict

                    string class_names_file = classes;
                    std::cout << "Reading imagenet class names..." << std::endl;
                    vector<string> class_names(num_classes);
                    class_names = eddl::read_txt_file(class_names_file);

                    printf("\nPREDICTIONS:\n");

                    for (int item = 0; item < 10; item++) {

                        printf("Item nr: %d\n", item);
                        std::string _range_ = std::to_string(item);
                        Tensor* xout = x_test->select({_range_});
                        Tensor* yout = y_test->select({_range_});
                        save_image_from_tensor(xout, yout, 0, 1, item, "pred");
                        vtensor preds = predict(net,{xout});
                        //preds[0]->print();
                        //preds[0]->save("pred_out.txt");
                        // Print top K predictions
                        int top_k = min(num_classes - 1, 5);
                        std::cout << "Top " << top_k << " true:" << std::endl;
                        std::cout << eddl::get_topk_predictions(yout, class_names, top_k) << std::endl;


                        std::cout << "Top " << top_k << " predictions:" << std::endl;
                        std::cout << eddl::get_topk_predictions(preds[0], class_names, top_k) << std::endl;

                        delete xout;
                        delete yout;
                        for (int i = 0; i < preds.size(); i++)
                            delete preds[i];
                    }

                }

                barrier_distributed();

                if (use_dg)
                    stop_data_generator();

            }
          
            if (use_dg == 0) {
                delete x_train;
                delete y_train;
            }
        } else { // Chunks>1
            printf("Chunks not supported\n");
            exit(1);
        }

    } else {

        //std::cout << "Evaluate test:" << std::endl;
        // Evaluate
        barrier_distributed();
        TIMED_EXEC("EVALUATE", evaluate(net,{x_test},
        {
            y_test
        }), secs);
        barrier_distributed();
        TIMED_EXEC("DISTR EVALUATE", evaluate_distr(net,{x_test},
        {
            y_test
        }), secs);
        barrier_distributed();
               TIMED_EXEC("CUSTOM EVALUATE", custom_evaluate(net,{x_test},{y_test}, {xbatch},{ybatch},batch_size, DIV_BATCH, false), secs);
    }
    //if (id==0)
    //    save_net_to_onnx_file (net,onnx_name);   

    delete xbatch;
    delete ybatch;
    //delete x_train;
    //delete y_train;
    delete x_test;
    delete y_test;
    delete net;

    end_data_generator;
    end_distributed();

    return EXIT_SUCCESS;
}


///////////
