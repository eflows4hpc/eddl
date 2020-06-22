#include <math.h>
#include <stdio.h>
extern "C" {

void k_sign_(float *A, long int size){

  #pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem
  #pragma HLS INTERFACE s_axilite port=A  bundle=control
  #pragma HLS INTERFACE s_axilite port=size bundle=control

  for (int i = 0; i < size; ++i) {
    if(A[i] > 0.0f){
      A[i] = 1.0f;
    }else if(A[i] < 0.0f){
      A[i] = -1.0f;
    }else{
      A[i] = 0.0f;
    }
  }

}
