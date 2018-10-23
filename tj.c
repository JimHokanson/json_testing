#include "stdio.h" //fopen_s?
#include <stdlib.h>
#include <ctype.h>
#include "mex.h"        //Matlab mex
#include <math.h>
#include "stdint.h"     //uint_8
#include <string.h>     //strchr()
#include <time.h>       //clock()
#include <sys/time.h>   //for gettimeofday

#include <omp.h>        //openmp


//#include "turtle_json_memory.h"
//TODO: Need to build in checks for support 
//SSE4.2
#include "x86intrin.h"


#define DEFINE_TIC(x) \
    struct timeval x ## _0; \
    struct timeval x ## _1;
  
#define START_TIC(x) gettimeofday(&x##_0,NULL);
    
//TODO: Make this call start and define   
#define TIC(x) \
    struct timeval x ## _0; \
    struct timeval x ## _1; \
    gettimeofday(&x##_0,NULL);
       
//x->name of structure
//y->output name
#define TOC(x,y) \
    gettimeofday(&x##_1,NULL); \
    double y = (double)(x##_1.tv_sec - x##_0.tv_sec) + (double)(x##_1.tv_usec - x##_0.tv_usec)/1e6; \
    mx_temp = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, 0); \
    double_temp = (double *)mxGetData(mx_temp); \
    *double_temp = y; \
    mxAddField(plhs[0],#y); \
    mxSetField(plhs[0],0,#y,mx_temp);  

    
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    //This will test the fastest way of initializing string data
    
    //Speed of malloc vs mxMalloc???
    
    //  f003(string_lengths)
    
    //0) Current Turtle JSON approach
    //1) Unitialized data
    //2) duplicate header, new data
    //3) copy entire mxArray (header and data)
    size_t n = mxGetN(prhs[0]);
    char *data = mxGetData(prhs[0]);
    mxArray *mx_temp;
    double *double_temp;
    plhs[0] = mxCreateStructMatrix(1,1,0,NULL);
        
    
        
    char *m = data + (n/32)*32;
    
    //Add 1 to take care of any floaters (if any)
    int n_out = n/64 + 1;
    
    TIC(step0);
    uint32_t *bm_quote_mov = mxMalloc(n_out*sizeof(int64_t));
    uint32_t *bm_quote = bm_quote_mov;
    
    uint32_t *bm_backslash_mov = mxMalloc(n_out*sizeof(int64_t));
    uint32_t *bm_backslash = bm_backslash_mov;
    TOC(step0,time_step0);
    
    TIC(step1);
    //TODO: Error check on allocation
    
    //Population of quote and backslash bitmaps
    //-----------------------------------------------------
    __m256i c_quote = _mm256_set1_epi8 ('"');
    __m256i c_backslash = _mm256_set1_epi8 ('\\');
    
    char *i = data;
    for (; i < m; i+=32){
        __m256i temp = _mm256_loadu_si256 ((const __m256i *)i);
        __m256i result1 = _mm256_cmpeq_epi8 (temp,c_quote);
        __m256i result2 = _mm256_cmpeq_epi8 (temp,c_backslash);
        *bm_quote_mov = _mm256_movemask_epi8(result1);
        ++bm_quote_mov;
        *bm_backslash_mov = _mm256_movemask_epi8(result2);
        ++bm_backslash_mov;
    }
    
    //Zero out last word
    *bm_quote_mov = 0;
    *bm_backslash_mov = 0;
    
    int bit_position = 0;
    for (; i < m; i++){
        *bm_quote_mov |= ((*i == '"') << bit_position);
        *bm_backslash_mov |= ((*i == '\\') << bit_position);
        ++bit_position;
    }

    
    
    TOC(step1,time_step1);
    
    
    
    mx_temp = mxCreateNumericMatrix(1, 0, mxUINT64_CLASS, 0);
    mxSetData(mx_temp,bm_quote);
    mxSetN(mx_temp,n_out);
    mxAddField(plhs[0],"real_string_mask"); \
    mxSetField(plhs[0],0,"real_string_mask",mx_temp); 
    
}
    
    
    
    
    