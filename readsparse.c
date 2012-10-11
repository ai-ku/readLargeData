/*=================================================================
 * fulltosparse.c
 * This example demonstrates how to populate a sparse
 * matrix.  For the purpose of this example, you must pass in a
 * non-sparse 2-dimensional argument of type double.

 * Comment: You might want to modify this MEX-file so that you can use
 * it to read large sparse data sets into MATLAB.
 *
 * This is a MEX-file for MATLAB.  
 * Copyright 1984-2006 The MathWorks, Inc.
 * All rights reserved.
 *
 * Use with:
 * ~/matlabtools/../.matlab/R2010b/mexopts.sh
 * to compile: mex readsparse.c -largeArrayDims
 *=================================================================*/

/* $Revision: 1.5.6.2 $ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* Needed for the ceil() prototype */
#include "foreach.h"
#include "mex.h"

/* If you are using a compiler that equates NaN to be zero, you must
 * compile this example using the flag  -DNAN_EQUALS_ZERO. For example:
 *
 *     mex -DNAN_EQUALS_ZERO fulltosparse.c
 *
 * This will correctly define the IsNonZero macro for your C compiler.
 */

#if defined(NAN_EQUALS_ZERO)
#define IsNonZero(d) ((d)!=0.0 || mxIsNaN(d))
#else
#define IsNonZero(d) ((d)!=0.0)
#endif
int nrow = 1200000 ,initnnz = 13000, ncol = 0,allnnz = 0, maxnnz = 0;

typedef struct Instance{
     int idx;
     double val;
} Ins;

typedef struct _Row{
     int nnz;
     Ins * neigs;
} * Row;

int comp(const void * a, const void * b){
     Ins *A = (Ins*) a;
     Ins *B = (Ins*) b;
     if(A->idx > B->idx) return 1;
     else if(A->idx < B->idx) return -1;
     return 0;
}

Row * sparse_data_to_array(char * fname){ 
     Row * data; 
     data  = (Row*) calloc(nrow, sizeof(Row));
     int ri = 0;
     for(int i = 0 ; i < nrow; i++){
          data[i] = (Row) malloc(sizeof(struct _Row));
          data[i]->neigs = (Ins*) malloc(initnnz * sizeof(Ins));    
     }
     foreach_line(str, fname){
          int ti = 0, rowid = 0, nnz = 0;
          if(ri % 100 == 0)
               mexPrintf(".");
          //          data[ri] = (Row) malloc(sizeof(struct _Row));
          //data[ri]->neigs = (Ins*) malloc(initnnz * sizeof(Ins));
          int nn;
          foreach_token(tok, str){ /*each token*/
               double dd;
               if(ti == 0) rowid = atoi(tok);
               else if(ti % 2 == 1) nn = atoi(tok);
               else if(ti % 2 == 0){
                    if(maxnnz != 0 && nnz >= maxnnz)
                         break;
                    dd = atof(tok);
                    (data[ri]->neigs[nnz]).idx = nn;
                    (data[ri]->neigs[nnz]).val = atof(tok);            
                    if(nn > ncol) ncol = nn;               
                    nnz++;
                    allnnz++;
               }
               ti++;
          }
          data[ri]->nnz = nnz;
          /*Matlab expects sorted non-zero indexes*/
          qsort(data[ri]->neigs, data[ri]->nnz, sizeof(Ins), comp);
          ri++;
     } 
     for(int i = ri; i < nrow ; i++){
          free(data[i]->neigs);
     }
     nrow = ri;     
     ncol = ncol + 1;

     return data;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
     mwSize m,n, nzmax;
     mwIndex *irs,*jcs,j,k;
     double *sr, percent_sparse;
     char *buf;
     int buflen;
     ncol = 0,allnnz = 0;
     /* Check for proper number of input and output arguments */    
     if (nrhs < 1) {
          mexErrMsgTxt("One input argument required.");
     }
     else if(nrhs == 1){
          maxnnz = 0; /*Default value no limit*/
     }
     else if(nrhs == 2){
          maxnnz = mxGetScalar(prhs[1]);
     }
     if(nlhs > 1){
          mexErrMsgTxt("Too many output arguments.");
     }     
     buflen = mxGetN(prhs[0])*sizeof(mxChar)+1;
     buf = mxMalloc(buflen);
     mxGetString(prhs[0],buf,buflen);
     mexPrintf("fname:%s\n",buf);
     //     Row * data = read_sparse_binary_data_to_array(buf, &nrow, &ncol, &nnz);
     Row * data = sparse_data_to_array(buf);
     //     print_data(data);     
     percent_sparse = (double)allnnz/nrow;
     mexPrintf("row:%d col:%d nnz:%d(%g)\n",nrow,ncol,allnnz, percent_sparse);
     nzmax=(mwSize)allnnz+1;
     m = (mwSize)nrow;
     n = (mwSize)ncol;
     plhs[0] = mxCreateSparse(n,m,nzmax,0);
     sr  = mxGetPr(plhs[0]); 
     irs = mxGetIr(plhs[0]); 
     jcs = mxGetJc(plhs[0]); 
     /* Copy nonzeros */
     k = 0; 
     for (j=0; j<m; j++) {
          jcs[j] = k; 
          if (k>=nzmax){
               mexPrintf("More elements than memory:max:%d k:%d\n",(int)nzmax,(int)k);
          }
          for (int i=0; (i< data[j]->nnz); i++) {
               sr[k] = (data[j]->neigs[i]).val; 
               irs[k] = (data[j]->neigs[i]).idx; 
               k++;                
          }
          mxFree(data[j]->neigs);
          mxFree(data[j]);
     }
     mxFree(data);
     jcs[m] = k;
     mexPrintf("allnnz:%d\n",(int)k);
     return;
}
 
