#summary This is a mini-wiki which is going to help you understand floatCL.

= Introduction =

floatCL is an API that uses openCL to make use of the GPU to execute parallel algorithms, currently only sort and finding the largest element.


= Understanding =

  * 3 main files
  ## floatCL.h - contains the declarations for floatCL
  ## floatCL.c - contains definitions for the function declarations
  ## floatCL.cl - contains parallelized code in CL

  * Usage in main/other programs
{{{
#include "floatCL/floatCL.h"

init_custom_cl(buffer size);        //initializes openCL to run on GPU

cl_mem gpu_mem_blk = put_to_GPURAM(array of floats, number to transfer);
call_(any fn. defined in floatCL.h) //calls the kernel slave(synchronous)

read_from_GPURAM(gpu_mem_blk, your float array, number to transfer);

clean_custom_cl();                  //cleans/releases openCL stuff
clReleaseMemObject(gpu_mem_blk);    //clean your variables
}}}

  * compile how-to:
{{{
g++ main.c floatCL.c -lOpenCL
}}}

= Examples =
  * Both example have been provided with a makefile.
  * type `make` in the project folder where the makefile is located
  * navigate to build/name
  * execute in the terminal