#include "mex.h"
#include "stdint.h"

//  mex dec2bin_good.c

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
 
    uint64_t *data = mxGetData(prhs[0]);
    uint64_t d = *data;
    
    //mexPrintf("%lu\n",d);
    
    char *out = mxMalloc(2*64);
    char *out_mov = out;
    
    uint64_t bit_position = 0;
    for (int i = 0; i < 64; i++){
        if (d & (((uint64_t)1) << bit_position)){
            *out_mov = '1';
        }else{
            *out_mov = '0';
        }
        ++out_mov;
        *out_mov = 0;
        ++out_mov;
        ++bit_position;
    }
        
    mxArray *mx_temp = mxCreateNumericMatrix(1, 0, mxCHAR_CLASS, 0);
    mxSetData(mx_temp,out);
    mxSetN(mx_temp,64);
    plhs[0] = mx_temp;

}