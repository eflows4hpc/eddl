#if defined(cPROTO)
#include "eddl/serialization/onnx/utils_onnx.h"

void sync_snets_with_orig(Net *net, bool acc_gradients)
{
  if (net->snets[0]->dev != DEV_CPU)
  {
    sync_params(net);
    if (acc_gradients)
      sync_acc_gradients(net);
  }
}

void sync_params(Net *net)
{
  for (int j = 0; j < net->layers.size(); j++)
  {
    for (int k = 0; k < net->layers[j]->params.size(); k++)
    {
      net->layers[j]->params[k]->fill_(0.0);
      for (int i = 0; i < net->snets.size(); i++)
      {
        Tensor::inc(net->snets[i]->layers[j]->params[k], net->layers[j]->params[k]);
      }
      net->layers[j]->params[k]->div_(net->snets.size());
    }
  }
}

void sync_acc_gradients(Net *net)
{
  for (int j = 0; j < net->layers.size(); j++)
  {
    for (int k = 0; k < net->layers[j]->acc_gradients.size(); k++)
    {
      net->layers[j]->acc_gradients[k]->fill_(0.0);
      for (int i = 0; i < net->snets.size(); i++)
      {
        Tensor::inc(net->snets[i]->layers[j]->acc_gradients[k], net->layers[j]->acc_gradients[k]);
      }
      net->layers[j]->acc_gradients[k]->div_(net->snets.size());
    }
  }
}

#endif // defined(cPROTO)
