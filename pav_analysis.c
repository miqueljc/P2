#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N){

    float suma=0.0F;

    for(int i=0; i<N; i++){
        suma+=(x[i])*(x[i]);
    }
    return 10*log10((1/(float)N)*suma);
}

float compute_am(const float *x, unsigned int N){
    
    float suma=0.0;

    for(int i=0; i<N; i++){
        suma+=fabs(x[i]);
    }
    return (1/(float)N)*suma;
}
float compute_zcr(const float *x, unsigned int N, float fm){
    float suma=0.0;

    for (int i=1; i<N; i++){
        if(x[i]*x[i-1]<0)
            suma+=1;
    }
    return (fm/2)*(1/((float)N-1))*suma;
}
