#if defined(cPROTO)
#ifndef EDDL_EXP_ONNX_H
#define EDDL_EXP_ONNX_H
#include "eddl/serialization/onnx/onnx.pb.h"
#include "eddl/layers/operators/layer_operators.h"

/*
 * ONNX EXPORT
 */

// OPSET: 13, 6
void build_exp_node(LExp *layer, onnx::GraphProto *graph);

#endif // EDDL_EXP_ONNX_H
#endif // cPROTO
