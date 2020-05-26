/*
* EDDL Library - European Distributed Deep Learning Library.
* Version: 0.6
* copyright (c) 2020, Universidad Politécnica de Valencia (UPV), PRHLT Research Centre
* Date: April 2020
* Author: PRHLT Research Centre, UPV, (rparedes@prhlt.upv.es), (jon@prhlt.upv.es)
* All rights reserved
*/


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include "eddl/net/net.h"
#include <pthread.h>
#include "eddl/utils.h"
#include "eddl/random.h"

#include "eddl/layers/core/layer_core.h"

#ifdef cGPU
#include "eddl/hardware/gpu/gpu_tensor.h"
#endif

#define VERBOSE 0

using namespace std;
using namespace std::chrono;


/////////////////////////////////////////
void Net::fts() {
    int i, j, k, n;
    vector<int> visit;
    vector<int> gin;

    for (i = 0; i < layers.size(); i++) {
        visit.push_back(0);

        n=0;
        for (j = 0; j < layers[i]->parent.size(); j++)
          if (isIn(layers[i]->parent[j],layers,k)) n++;

        gin.push_back(n);
    }

    for (i = 0; i < layers.size(); i++) {

        for (j = 0; j < layers.size(); j++)
            if ((gin[j] == 0) && (!visit[j])) break;

        if (j == layers.size())
            msg("error recurrent net", "Net.fts");


        visit[j] = 1;
        vfts.push_back(layers[j]);

        for (k = 0; k < layers[j]->lout; k++)
            for (n = 0; n < layers.size(); n++)
                if (layers[n] == layers[j]->child[k]) gin[n]--;

    }

   if (VERBOSE) {
    for (i = 0; i < vfts.size(); i++) {
      cout<<vfts[i]->name<<"-->";
    }
    cout<<"\n";
    getchar();
  }


}


/////////////////////////////////////////
void Net::bts() {
    int i, j, k, n;
    vector<int> visit;
    vector<int> gout;


    //fprintf(stdout,"BTS:");
    for (i = 0; i < layers.size(); i++) {
        visit.push_back(0);
        n=0;
        for (j = 0; j < layers[i]->child.size(); j++)
          if (isIn(layers[i]->child[j],layers,k)) n++;
        gout.push_back(n);
    }

    for (i = 0; i < layers.size(); i++) {

        for (j = 0; j < layers.size(); j++)
            if ((gout[j] == 0) && (!visit[j])) break;

        if (j == layers.size())
          msg("error recurrent net in", "Net.bts");

        visit[j] = 1;
        vbts.push_back(layers[j]);

        for (k = 0; k < layers[j]->lin; k++)
            for (n = 0; n < layers.size(); n++)
                if (layers[n] == layers[j]->parent[k]) gout[n]--;

    }

if (VERBOSE) {
   for (i = 0; i < vbts.size(); i++) {
     cout<<vbts[i]->name<<"-->";
   }
   cout<<"\n";
   getchar();
}


}


/////////////////////////////////////////
//// BUILD FUNCS
/////////////////////////////////////////
void Net::toCPU(int t){
    CompServ *cs=new CompServ(t, {}, {},0);

    for (int i = 0; i < snets.size(); i++) {
      Xs[i].clear();
      Ys[i].clear();
    }

    snets.clear();

    set_compserv(cs);

    if (cs->type == "local") {
      if (VERBOSE)  {
        if (snets[0]->dev == DEV_CPU)
          cout << "Net running on CPU\n";
        else if (snets[0]->dev < DEV_FPGA)
          cout << "Net running on GPU " << snets[0]->dev - DEV_GPU << "\n";
        else
          cout << "Net running on FPGA " << snets[0]->dev - DEV_FPGA << "\n";
      }
    }
}
void Net::toGPU(vector<int> g,int lsb,int mem){
    CompServ *cs=new CompServ(0, g, {},lsb,mem);

    for (int i = 0; i < snets.size(); i++) {
      Xs[i].clear();
      Ys[i].clear();
    }

    snets.clear();

    set_compserv(cs);

    if (VERBOSE) {
    if (cs->type == "local") {
      if (snets[0]->dev == DEV_CPU)
        cout << "Net running on CPU\n";
      else if (snets[0]->dev < DEV_FPGA)
        cout << "Net running on GPU " << snets[0]->dev - DEV_GPU << "\n";
      else
        cout << "Net running on FPGA " << snets[0]->dev - DEV_FPGA << "\n";
    }
  }
}

void Net::build(Optimizer *opt, vloss lo, vmetrics me, CompServ *cs, bool initialize){
	onnx_pretrained = !initialize; // For controlling when to copy the weights to the snet

  if (isbuild) return;


  for(int i=0;i<layers.size();i++) {
    if ((layers[i]->orig!=nullptr)&&(layers[i]->orig->net!=this)) {
      layers[i]->orig->net->build(opt->clone(),{},{},cs,true);
    }
    else if (layers[i]->net!=this) {
      layers[i]->net->build(opt->clone(),{},{},cs,true);
    }
  }

  cout<<"Building "<<name<<endl;

  build(opt, lo, me, initialize);

  set_compserv(cs);

  if (VERBOSE) {
    if (cs->type == "local") {
      if (snets[0]->dev == DEV_CPU)
        cout << "Net running on CPU\n";
      else if (snets[0]->dev < DEV_FPGA)
        cout << "Net running on GPU " << snets[0]->dev - DEV_GPU << "\n";
      else
        cout << "Net running on FPGA " << snets[0]->dev - DEV_FPGA << "\n";
    }
  }
  isbuild=true;

}


void Net::build(Optimizer *opt, vloss lo, vmetrics me, bool initialize) {
    if (VERBOSE) cout<<"Build net "<<name<<"\n";

    // check devices
    dev = -1;
    int ind;

    for(int i=0; i<layers.size(); i++){
        if (layers[i]->isrecurrent) isrecurrent=true;

        // Set device // TODO: Rewrite this
        if (dev == -1) {
            dev = layers[i]->dev;
        } else {
            if (layers[i]->dev != dev) {
                msg("Net with layers in different devices", "Net.build");
            }
        }

        // Set params
        layers[i]->verbosity_level = this->verbosity_level;
    }
    // set optimizer
    optimizer = opt;
    optimizer->setlayers(layers);

    // set loss functions and create targets tensors
    this->losses = vloss(lo);
    for (int i = 0; i < lo.size(); i++) {
        if (lo[i]->name == "soft_cross_entropy") lout[i]->delta_bp = 1;
        lout[i]->target = new Tensor(lout[i]->output->getShape(), dev);
    }
    // set metrics
    this->metrics = vmetrics(me);


    // forward sort
    fts();
    // backward sort
    bts();
    // random params
    if(initialize) do_initialize();
}

void Net::set_compserv(CompServ *cs){
    int todev;
    this->cs=cs;

    mem_level=cs->mem_level;
    for(int i=0;i<layers.size();i++)
        layers[i]->set_mem_level(mem_level);

    if (cs->type == "local") {

        if (cs->local_gpus.size() > 0) todev = DEV_GPU;
        else if (cs->local_fpgas.size() > 0) todev = DEV_FPGA;
        else todev = DEV_CPU;

        if (todev == DEV_CPU) {
            if (dev == DEV_CPU) {

                int nthreads = cs->local_threads;
                if (nthreads <= 0)
                    msg("Threads must be > 0", "Net.set_compserv");

                Eigen::initParallel();
                Eigen::setNbThreads(nthreads);

                snets.push_back(this);

            } else {
                msg("Net and Layers device missmatch", "Net.set_compserv");
            }
        } else if (todev < DEV_FPGA) {
#ifndef cGPU
            msg("EDDLL not compiled for GPU", "Net.set_compserv");
#else
        // split on multiple GPUs
        int ngpus=gpu_devices();
        if (ngpus==0) {
          msg("GPU devices not found","Net.set_compserv(");
        }
        if (cs->local_gpus.size()>ngpus)
        {
          msg("GPU list on ComputingService is larger than available devices","Net.set_compserv");
        }

        if (VERBOSE) cout<<"Selecting GPUs from CS_GPU\n";

        for(int i=0;i<cs->local_gpus.size();i++)
          if (cs->local_gpus[i]) {
            devsel.push_back(i);
            if (VERBOSE) cout<<"GPU ("<<i<<")\n";
          }

        if (!devsel.size())
          msg("No gpu selected","Net.set_compserv");

        if (VERBOSE) cout<<"split into "<<devsel.size()<<" GPUs devices\n";

        if (!cs->isshared) {
            split(devsel.size(),DEV_GPU);
        }


#endif
        } else {
            // split on multiple FPGAs
        }
    } else {
        msg("Distributed version not yet implemented", "Net.set_compserv");
    }

    // create input and output tensors (X,Y)
    for (int i = 0; i < snets.size(); i++) {
      for (int j = 0; j < snets[i]->lin.size(); j++)
          Xs[i].push_back(new Tensor(snets[i]->lin[j]->input->shape));
      for (int j = 0; j < snets[i]->lout.size(); j++)
          Ys[i].push_back(new Tensor(snets[i]->lout[j]->output->shape));
    }
  }

// Split nets among CS
void Net::split(int c, int todev) {
    int i, j, k, l;

    vlayer nlayers;
    vlayer nin;
    vlayer nout;
    int ind;
    vlayer par;

    int bs=1;
    int m=0;

    //clone net into CompServices
    for (i = 0; i < c; i++) {
        if (VERBOSE) cout << "Split " << i << "\n";

        nlayers.clear();
        nin.clear();
        nout.clear();
        if (i == c - 1) bs += m;

        //clone layers into CompServices
        for(int j=0;j<vfts.size();j++) {
          if (!isInorig(vfts[j], nlayers, ind)) {

              vlayer par;
              for (l = 0; l < vfts[j]->parent.size(); l++)
                  if (isInorig(vfts[j]->parent[l], nlayers, ind))
                    par.push_back(nlayers[ind]);

              Layer *n;
              if (vfts[j]->isshared) {
                Layer *on;
                if (vfts[j]->orig->clones.size()>i) {
                  on=vfts[j]->orig->clones[i];
                }
                else {
                  // should have been cloned previously in build
                  msg("error","build");
                }
                n=on->share(0,1,par);
                n->name="share_"+on->name;
                n->orig=vfts[j];
              }
              else {
                n=vfts[j]->clone(i, bs, par, todev + devsel[i]);
                n->name="clone_"+i+vfts[j]->name;
                vfts[j]->clones.push_back(n);
              }

              nlayers.push_back(n);
              if (isIn(vfts[j],lin,ind)) nin.push_back(n);
              if (isIn(vfts[j],lout,ind)) nout.push_back(n);
          }
        }

        // create twin net on CS device
        snets.push_back(new Net(nin, nout));
        // build new net
        char cname[100];
        sprintf(cname,"snet_%d",i);
        snets[i]->name=cname;
        snets[i]->build(optimizer->clone(), losses, metrics);
        if(onnx_pretrained){ //We need to copy the imported weights to each snet
            //printf("Copying from CPU to GPU\n");
            for(int i = 0; i < snets.size(); i++)
                for(int j = 0; j < layers.size(); j++)
                    layers[j]->copy(snets[i]->layers[j]);
        }
        snets[i]->plot("smodel.pdf","LR");
    }
}


void Net::resize(int b)
{
  int i,j;

  if (batch_size==b) return;

  batch_size=b;
  if (VERBOSE) cout<<"Resizing Net to batch_size="<<batch_size<<"\n";

  int c=snets.size();
  int bs,m;

  if (batch_size<c) {
    printf("=====> Warning: batch_size (%d) lower than compserv resources (%d)\n",batch_size,c);
    bs=1;
    m=0;
    c=batch_size;
  }
  else {
    bs = batch_size / c;
    m = batch_size % c;
  }

  for (j = 0; j < layers.size(); j++) {
      layers[j]->resize(batch_size);
  }

  for(i=0; i<c; i++) {
    Xs[i].clear();
    Ys[i].clear();

    if (i==c-1) bs+=m;
    snets[i]->batch_size=bs;
    for (j = 0; j < snets[i]->layers.size(); j++) {
        snets[i]->layers[j]->resize(bs);
      }

    for (j = 0; j < snets[i]->lin.size(); j++)
        Xs[i].push_back(new Tensor(snets[i]->lin[j]->input->shape));

    for (j = 0; j < snets[i]->lout.size(); j++)
        Ys[i].push_back(new Tensor(snets[i]->lout[j]->output->shape));
  }

  reset();

}

Layer * Net::getLayer(vlayer in)
{
  int i,j,k,l,ind;
  if (lin.size()!=in.size())
    msg("Error size of input layers set","Net:Net");


  vlayer nlayers;
  //input layers
  for (i = 0; i < lin.size(); i++)  {
    vlayer par;
    Layer *n=lin[i]->share(0, 1, par);

    n->name=in[0]->name+n->name;
    nlayers.push_back(n);

    in[i]->addchild(n);
    n->addparent(in[i]);
  }

  // rest of layers
  for (k = 0; k < layers.size(); k++) {
      for (j = 0; j < layers.size(); j++) {
          if (!isInorig(layers[j], nlayers, ind)) {
              vlayer par;
              for (l = 0; l < layers[j]->parent.size(); l++) {
                  if (!isInorig(layers[j]->parent[l], nlayers, ind)) break;
                  else {par.push_back(nlayers[ind]);}
              }
              if (l == layers[j]->parent.size()) {
                  Layer *n;
                  n=layers[j]->share(0, 1, par);
                  nlayers.push_back(n);
                  n->name=in[0]->name+n->name;
                  n->isshared=true;
              }
          }
      }
    }

  vlayer nout;
  // set outputs
  for (j = 0; j < lout.size(); j++)
    if (isInorig(lout[j], nlayers, ind))
        nout.push_back(nlayers[ind]);

  return nout[0];

}




bool check_rnn_forward(Layer *l) {

  bool frnn=false;

  for(int i=0;i<l->child.size();i++) {
    if (l->child[i]->isrecurrent) {frnn=true;break;}
    else frnn=check_rnn_forward(l->child[i]);
    if (frnn) break;
  }

  return frnn;
}

// Unroll Recurrent net
Net* Net::unroll(int inl, int outl, bool seq, bool areg) {
  int i, j, k, l;

  vlayer *nlayers;
  vlayer *nin;
  vlayer *nout;
  int ind;
  vlayer par;
  vector<bool> frnn;


  // set vfts sort
  layers.clear();
  for(int i=0;i<vfts.size();i++)
    layers.push_back(vfts[i]);

  // check if rnn is in forward path
  for(int i=0;i<layers.size();i++)
    if (layers[i]->isrecurrent) frnn.push_back(true);
    else frnn.push_back(check_rnn_forward(layers[i]));

  // set sort first frnn
  vlayer lfrnn;
  for(int i=0;i<layers.size();i++)
    if (frnn[i]) lfrnn.push_back(layers[i]);

  for(int i=0;i<layers.size();i++)
    if (!frnn[i]) lfrnn.push_back(layers[i]);

  layers.clear();
  for(int i=0;i<lfrnn.size();i++)
    layers.push_back(lfrnn[i]);


  // re-check frnn with the new sort
  frnn.clear();
  for(int i=0;i<layers.size();i++)
    if (layers[i]->isrecurrent) frnn.push_back(true);
    else frnn.push_back(check_rnn_forward(layers[i]));



  // unroll inputs
  nin=new vlayer[inl];
  nlayers=new vlayer[inl];
  nout=new vlayer[inl];

  for (i = 0; i < inl; i++) {
    //input layers
    for (j = 0; j < lin.size(); j++)  {
      nin[i].push_back(lin[j]->share(i, batch_size, par));
      nlayers[i].push_back(nin[i][j]);
    }

    // rest of layers
    for (j = 0; j < layers.size(); j++) {
      if ((i>=(inl-outl))||(frnn[j])) {
        if (!isInorig(layers[j], nlayers[i], ind)) {
          vlayer par;
          for (l = 0; l < layers[j]->parent.size(); l++) {
            if (!isInorig(layers[j]->parent[l], nlayers[i], ind)) break;
            else par.push_back(nlayers[i][ind]);
          }
          if (l == layers[j]->parent.size()) {

            if ((layers[j]->isrecurrent)&&(i>0)) {
              par.push_back(nlayers[i-1][j]);
              nlayers[i].push_back(layers[j]->share(i, batch_size, par));
            }
            else {
              nlayers[i].push_back(layers[j]->share(i, batch_size, par));
            }
          }
          else msg("Unexpected error","unroll");
        }
      }
    }

  // set output layers
  if (i>=(inl-outl)) {
    for (j = 0; j < lout.size(); j++)
      if (isInorig(lout[j], nlayers[i], ind))
        nout[i].push_back(nlayers[i][ind]);
  }

}

/////
vlayer ninl;
vlayer noutl;
for (i = 0; i < inl; i++)
  for (j = 0; j < nin[i].size(); j++)
    ninl.push_back(nin[i][j]);
for (i = 0; i < inl; i++)
  for (j = 0; j < nout[i].size(); j++)
    noutl.push_back(nout[i][j]);

Net *rnet=new Net(ninl, noutl);

return rnet;
}


void Net::build_rnet(int inl,int outl) {
  int i, j, k, n;
  int todev;

  if (cs->local_gpus.size() > 0) todev = DEV_GPU;
  else if (cs->local_fpgas.size() > 0) todev = DEV_FPGA;
  else todev = DEV_CPU;

  if ((rnet==nullptr)||(inl!=rnet->lin.size())) {

   if (rnet!=nullptr) delete rnet;

   printf("Recurrent net %d time steps, %d outputs\n",inl,outl);

   // Create an unrolled version on CPU
   rnet=unroll(inl,outl,false,false);

   rnet->plot("rmodel.pdf","LR");

   for(i=0;i<rnet->layers.size();i++) {
     rnet->layers[i]->isrecurrent=false;
     rnet->layers[i]->net=rnet;
     rnet->layers[i]->orig=rnet->layers[i];
   }
   rnet->isrecurrent=false;

   vloss lr;
   for(i=0;i<losses.size();i++) lr.push_back(losses[i]->clone());

   vmetrics mr;
   for(i=0;i<metrics.size();i++) mr.push_back(metrics[i]->clone());

   rnet->build(optimizer->share(),lr,mr,cs->share(),false);
   //cout<<rnet->summary();
   fflush(stdout);
   rnet->plot("rmodel.pdf","LR");
   rnet->name="rnet";


   //getchar();
   //cout<<rnet->summary();

   if (todev!=DEV_CPU) {
     // unroll CS devices and link
     for(i=0;i<rnet->snets.size();i++)
       delete rnet->snets[i];
     rnet->snets.clear();
     for(i=0;i<snets.size();i++) {
     //cout<<snets[i]->summary();
       rnet->snets.push_back(snets[i]->unroll(inl,outl,false,false));
       for(j=0;j<rnet->snets[i]->layers.size();j++) {
             rnet->snets[i]->layers[j]->isrecurrent=false;
       }
       rnet->snets[i]->isrecurrent=false;

       rnet->snets[i]->build(snets[i]->optimizer->share(),lr,mr,false);
       rnet->snets[i]->plot("rsnet.pdf","LR");
       for(j=0;j<rnet->snets[i]->layers.size();j++) {
             rnet->snets[i]->layers[j]->orig=rnet->layers[j];
             rnet->snets[i]->layers[j]->net=rnet;
       }
      }
    }

   rnet->flog_tr=flog_tr;
   rnet->flog_ts=flog_ts;

   rnet->reset_loss();
   rnet->reset();
   rnet->reset_grads();

   fflush(stdout);

  }
}





void Net::enable_distributed(){
	for(Layer* l : layers){
		l->enable_distributed();
	}
}
