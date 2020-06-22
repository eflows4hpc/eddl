#include <math.h>
#include <stdio.h>

extern "C" {

void k_thresholded_relu(float *A, float *B, long int size, float param){

  #pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem
  #pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem
  #pragma HLS INTERFACE s_axilite port=A  bundle=control
  #pragma HLS INTERFACE s_axilite port=B  bundle=control
  #pragma HLS INTERFACE s_axilite port=size bundle=control
  #pragma HLS INTERFACE s_axilite port=param bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  
  for (int i = 0; i < size; i++) {
    if (A[i] > param) B[i] = A[i];
    else B[i] = 0.0;
  }
}

} // extern "C"
