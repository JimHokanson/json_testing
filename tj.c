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

//Other approach
//remove the rightmost 1 in x : x & (x-1)
//extract the rightmost 1 in x : x&-x
//smear right at 1 an up x : x xor (x-1)
    
// int x = 8+4;
//   //Yields rightmost bit
//   printf("%d\n",x & -x); //4
//   //Smears righmost bit
//   printf("%d\n",x | (x-1)); //15 => 8 4 2 1 all set - 2 and 1 from smear
//   //Turn off rightmost bit
//   printf("%d\n",x & (x-1)); //8
//   return 0;    

void getStructQuotes(uint64_t *bm_quote_mov, uint64_t *bm_backslash_mov,uint64_t bsq){

    uint64_t temp_quote_word = *bm_quote_mov; //This might get modified ...
    
    //Make backslashes 0, 1 for everything else
    //We'll use this to count zeros to get # of backslashes
    uint64_t inverted_bs = ~(*bm_backslash_mov); //0 => \     1 => else 
            
    int bsq_loc;
    int n_quote_escapes = __builtin_popcount(bsq);
    for (int j = 0; j < n_quote_escapes; j++){
        //Find current quote
        
        _BitScanReverse64 (&bsq_loc, bsq);
                
        //Zero out found location so next loop doesn't find it again ...
        bsq &= ~((uint64_t)1 << bsq_loc); 
                
                //Set everything high to zero to allow finding first
                //non-backslash character
             	inverted_bs2 = _bzhi_u32(inverted_bs,bsq_loc);
                
                // "this is a \\\\\"test"
                //                 x <= bsq_loc
                //  111111111100000000000 <= inverted_bs2        
                
                //TODO: consider 0 check then _bit_scan_reverse
                //  since _lzcnt_u32 has high latency
                //Counting zeros instead of finding non-backslash
                //because we might have all backslashes which would 
                //lead to undefined behavior for _bit_scan_reverse
                //Lat: 3, CPI: 1
                n_zeros = _lzcnt_u32(inverted_bs2);
                if (n_zeros == 32){
                    //Then need to go back even further :/
                    //TODO
                    n_backslashes = 1;
                }else{
                    //If bsq_loc == 31 then 1 zero means 0 backslashes
                    //0 = 1 - (X - 31)
                    //If bsq_loc == 30 then 2 zeros means 0 backslashes
                    //0 = 2 - (X - 30)
                    //If bsq_loc == 30 then 3 zeros means 1 backslashes
                    //1 = 3 - (X - 30)
                    n_backslashes = n_zeros - (32 - bsq_loc);
                }
                
//                 if (!printed){
//                     printed = 1;
//                     mexPrintf("a: %d\n",n_backslashes); //2
//                     mexPrintf("b: %d\n",n_zeros); //23
//                     mexPrintf("c: %d\n",bsq_loc); //10
//                     mexPrintf("d: %d\n",inverted_bs2); //511
//                     mexPrintf("e: %d\n",temp_quote_word);
//                 }
                
                //Avoid this, might not have any ones ...        
                //non_bs_loc = _bit_scan_reverse(inverted_bs2);
                //n_backslashes = bsq_loc-non_bs_loc-1;
                
                temp_quote_word &= ~((n_backslashes & 1) << bsq_loc);
                
            }
            
            *bm_quote_mov = temp_quote_word;
}
    
void altApproach(uint64_t *bm_quote_mov,uint64_t *bm_backslash_mov,uint64_t n){

    
    //  0010000000000100000010000 <= "
    //  0000100000111000000100000 <= \
    //  0000000000000100000010000 <= bsq
    //  1111011111000111111011111 <= inverted bs
    //               o  => x & -x
    //  11110111110000  =>              
    
    //  1) popcnt
    //  2 _bit_scan_forward to bsq
    //  3) grab all \ left - count zeros
    
    
    //  0010000000000100000010000 <= "
    //  0000100000111000000100000 <= \
    //  0000000000000100000010000 <= bsq
    //  
    //  1) popcnt - to know # to run
    //  2) x & -x to get target location
    //  3) x & (x-1) get all low values
    //  4) zero count?
    
    // 
    
    //TODO: Support first row ...
    
    for (uint64_t i = 1; i<n; i++){
        bsq = ((*bm_backslash_mov_prev >> 63) | (*bm_backslash_mov << 1)) & *bm_quote_mov;
        
        if (bsq){
            //Call method
            
            
        }
        //TODO:
    }
    
    
}
    
void getStructQuotes64(uint64_t *bm_quote_mov, 
        uint64_t *bm_backslash_mov,uint64_t *m2){

    int printed = 0;
    uint64_t bsq;
    uint64_t inverted_bs;
    uint64_t inverted_bs2;
    int bsq_loc;
    int n_quote_escapes;
    int n_backslashes;
    uint32_t n_zeros;
    int current_set_bit;
    
    int temp_quote_word;
    
    uint32_t *bm_backslash_mov_prev;
    uint32_t *bm_backslash_mov_prev2;
    
    //Advance past 1st word ...
    ++bm_quote_mov;
    ++bm_backslash_mov;
    
    bm_backslash_mov_prev = bm_backslash_mov-1;
    
    for (; bm_quote_mov < m2; bm_quote_mov++){
        
        bsq = ((*bm_backslash_mov_prev >> 31) | (*bm_backslash_mov << 1)) & *bm_quote_mov;

        //Only update if we have any \" 
        if (bsq){
            temp_quote_word = *bm_quote_mov; //This might get modified ...
            inverted_bs = ~(*bm_backslash_mov); //0 => \     1 => else 
            
            n_quote_escapes = __builtin_popcount(bsq);
            for (int j = 0; j < n_quote_escapes; j++){
                //Find current quote
                bsq_loc = _bit_scan_reverse(bsq);
                
                //Zero out found location so next loop doesn't find it again ...
                bsq &= ~(1 << bsq_loc); 
                
                //Set everything high to zero to allow finding first
                //non-backslash character
             	inverted_bs2 = _bzhi_u32(inverted_bs,bsq_loc);
                
                // "this is a \\\\\"test"
                //                 x <= bsq_loc
                //  111111111100000000000 <= inverted_bs2        
                
                //TODO: consider 0 check then _bit_scan_reverse
                //  since _lzcnt_u32 has high latency
                //Counting zeros instead of finding non-backslash
                //because we might have all backslashes which would 
                //lead to undefined behavior for _bit_scan_reverse
                //Lat: 3, CPI: 1
                n_zeros = _lzcnt_u32(inverted_bs2);
                if (n_zeros == 32){
                    //Then need to go back even further :/
                    //TODO
                    n_backslashes = 1;
                }else{
                    //If bsq_loc == 31 then 1 zero means 0 backslashes
                    //0 = 1 - (X - 31)
                    //If bsq_loc == 30 then 2 zeros means 0 backslashes
                    //0 = 2 - (X - 30)
                    //If bsq_loc == 30 then 3 zeros means 1 backslashes
                    //1 = 3 - (X - 30)
                    n_backslashes = n_zeros - (32 - bsq_loc);
                }
                
//                 if (!printed){
//                     printed = 1;
//                     mexPrintf("a: %d\n",n_backslashes); //2
//                     mexPrintf("b: %d\n",n_zeros); //23
//                     mexPrintf("c: %d\n",bsq_loc); //10
//                     mexPrintf("d: %d\n",inverted_bs2); //511
//                     mexPrintf("e: %d\n",temp_quote_word);
//                 }
                
                //Avoid this, might not have any ones ...        
                //non_bs_loc = _bit_scan_reverse(inverted_bs2);
                //n_backslashes = bsq_loc-non_bs_loc-1;
                
                temp_quote_word &= ~((n_backslashes & 1) << bsq_loc);
                
            }
            
            *bm_quote_mov = temp_quote_word;
        }
        ++bm_backslash_mov;
        ++bm_backslash_mov_prev;        
    }    
    
}
    
void getStructQuotes32(uint32_t *bm_quote_mov, 
        uint32_t *bm_backslash_mov,uint32_t *m2){

    int printed = 0;
    uint32_t bsq;
    uint32_t inverted_bs;
    uint32_t inverted_bs2;
    int bsq_loc;
    int n_quote_escapes;
    int n_backslashes;
    uint32_t n_zeros;
    int current_set_bit;
    
    int temp_quote_word;
    
    uint32_t *bm_backslash_mov_prev;
    uint32_t *bm_backslash_mov_prev2;
    
    //Advance past 1st word ...
    ++bm_quote_mov;
    ++bm_backslash_mov;
    
    bm_backslash_mov_prev = bm_backslash_mov-1;
    
    for (; bm_quote_mov < m2; bm_quote_mov++){
        
        bsq = ((*bm_backslash_mov_prev >> 31) | (*bm_backslash_mov << 1)) & *bm_quote_mov;

        //Only update if we have any \" 
        if (bsq){
            temp_quote_word = *bm_quote_mov; //This might get modified ...
            inverted_bs = ~(*bm_backslash_mov); //0 => \     1 => else 
            
            n_quote_escapes = __builtin_popcount(bsq);
            for (int j = 0; j < n_quote_escapes; j++){
                //Find current quote
                bsq_loc = _bit_scan_reverse(bsq);
                
                //Zero out found location so next loop doesn't find it again ...
                bsq &= ~(1 << bsq_loc); 
                
                //Set everything high to zero to allow finding first
                //non-backslash character
             	inverted_bs2 = _bzhi_u32(inverted_bs,bsq_loc);
                
                // "this is a \\\\\"test"
                //                 x <= bsq_loc
                //  111111111100000000000 <= inverted_bs2        
                
                //TODO: consider 0 check then _bit_scan_reverse
                //  since _lzcnt_u32 has high latency
                //Counting zeros instead of finding non-backslash
                //because we might have all backslashes which would 
                //lead to undefined behavior for _bit_scan_reverse
                //Lat: 3, CPI: 1
                n_zeros = _lzcnt_u32(inverted_bs2);
                if (n_zeros == 32){
                    //Then need to go back even further :/
                    //TODO
                    n_backslashes = 1;
                }else{
                    //If bsq_loc == 31 then 1 zero means 0 backslashes
                    //0 = 1 - (X - 31)
                    //If bsq_loc == 30 then 2 zeros means 0 backslashes
                    //0 = 2 - (X - 30)
                    //If bsq_loc == 30 then 3 zeros means 1 backslashes
                    //1 = 3 - (X - 30)
                    n_backslashes = n_zeros - (32 - bsq_loc);
                }
                
//                 if (!printed){
//                     printed = 1;
//                     mexPrintf("a: %d\n",n_backslashes); //2
//                     mexPrintf("b: %d\n",n_zeros); //23
//                     mexPrintf("c: %d\n",bsq_loc); //10
//                     mexPrintf("d: %d\n",inverted_bs2); //511
//                     mexPrintf("e: %d\n",temp_quote_word);
//                 }
                
                //Avoid this, might not have any ones ...        
                //non_bs_loc = _bit_scan_reverse(inverted_bs2);
                //n_backslashes = bsq_loc-non_bs_loc-1;
                
                temp_quote_word &= ~((n_backslashes & 1) << bsq_loc);
                
            }
            
            *bm_quote_mov = temp_quote_word;
        }
        ++bm_backslash_mov;
        ++bm_backslash_mov_prev;        
    }    
    
}
    
    
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
        
    
        
    
    
    //Add 1 to take care of any floaters (if any)
    int n_qwords = n/64 + 1;
    
    TIC(step0);
    
    #define BM_PTR uint32_t
    
    BM_PTR *bm_quote_mov = mxMalloc(n_qwords*sizeof(int64_t));
    BM_PTR *bm_quote = bm_quote_mov;
    
    BM_PTR *bm_backslash_mov = mxMalloc(n_qwords*sizeof(int64_t));
    BM_PTR *bm_backslash = bm_backslash_mov;
    
//     BM_PTR *bm_lb_mov = mxMalloc(n_qwords*sizeof(int64_t));
//     BM_PTR *bm_lb = bm_lb_mov;
//     
//     BM_PTR *bm_rb_mov = mxMalloc(n_qwords*sizeof(int64_t));
//     BM_PTR *bm_rb = bm_rb_mov;
//     
//     BM_PTR *bm_comma_mov = mxMalloc(n_qwords*sizeof(int64_t));
//     BM_PTR *bm_comma = bm_comma_mov;
    
    //30ms - ",\
    //41ms - + {
    //63ms - " \ { } ,

    TOC(step0,time_step0);
    
    TIC(step1);
    //TODO: Error check on allocation
    
    //Population of quote and backslash bitmaps
    //-----------------------------------------------------
    __m256i c_quote = _mm256_set1_epi8 ('"');
    __m256i c_backslash = _mm256_set1_epi8 ('\\');
//     __m256i c_lb = _mm256_set1_epi8 ('{');
//     __m256i c_rb = _mm256_set1_epi8 ('}');
//     __m256i c_comma = _mm256_set1_epi8 (',');
    __m256i mm_data;
    
    char *m = data + (n/64)*64;
    char *i = data;
    for (; i<m; i+=32){
        mm_data = _mm256_loadu_si256 ((const __m256i *)i);
        *bm_quote_mov = _mm256_movemask_epi8(_mm256_cmpeq_epi8 (mm_data,c_quote));
        *bm_backslash_mov = _mm256_movemask_epi8(_mm256_cmpeq_epi8 (mm_data,c_backslash));
        ++bm_backslash_mov;
        ++bm_quote_mov;
//         *bm_lb_mov = _mm256_movemask_epi8(_mm256_cmpeq_epi8 (mm_data,c_lb));
//         ++bm_lb_mov;
//         *bm_rb_mov = _mm256_movemask_epi8(_mm256_cmpeq_epi8 (mm_data,c_rb));
//         ++bm_rb_mov;
//         *bm_comma_mov = _mm256_movemask_epi8(_mm256_cmpeq_epi8 (mm_data,c_comma));
//         ++bm_comma_mov;
    }

    //Zero out last word
    *bm_quote_mov = 0;
    *bm_backslash_mov = 0;
    
    m = data + n;
    int bit_position = 0;
    for (; i < m; i++){
        *bm_quote_mov |= ((*i == '"') << bit_position);
        *bm_backslash_mov |= ((*i == '\\') << bit_position);
        ++bit_position;
    }
    
    
    TOC(step1,time_step1);
    
    //TODO: Make sure first char is not \
    TIC(step2)
    getStructQuotes32(bm_quote,bm_backslash,bm_quote_mov);
    TOC(step2,time_step2);
    
    
    mx_temp = mxCreateNumericMatrix(1, 0, mxUINT64_CLASS, 0);
    mxSetData(mx_temp,bm_quote);
    mxSetN(mx_temp,n_qwords);
    mxAddField(plhs[0],"real_string_mask"); \
    mxSetField(plhs[0],0,"real_string_mask",mx_temp); 
    
    mx_temp = mxCreateNumericMatrix(1, 0, mxUINT64_CLASS, 0);
    mxSetData(mx_temp,bm_backslash);
    mxSetN(mx_temp,n_qwords);
    mxAddField(plhs[0],"bs_mask"); \
    mxSetField(plhs[0],0,"bs_mask",mx_temp); 
    
}
    
    
    
    
    