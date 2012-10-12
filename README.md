readLargeData
=============


This function reads the sparse input data files to MATLAB
     -Format

     	[info_token] [index_1] [value_1] [index_2] [value_2] ... [index_n] [value_n]

	*info_token might be anything the function does not use this token.

v0.0.3:
	readsparse.c:
		Reads the sparse matrix 	
		
	readsparseidx:
		Constructs a Matlab matrix that keeps the non-zero elements of the sparse matrix
		ex:  
		     input: [info_token] [index_1] [value_1] [index_2] [value_2] ... [index_n] [value_n]
		     Matlab matrix:
		     	    [index_1] [index_2] ... [index_n]
			
v0.0.2:
	Compatible with Matlab standard Ansi C compiler (mex -setup and select option 2)

v0.0.1:
	This code uses the std99 and a lots of macros but it is working