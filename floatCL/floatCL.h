/*
This file is part of floatCL.

floatCL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

floatCL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with floatCL.  If not, see <http://www.gnu.org/licenses/>.

Author: Aditya Vijayakumar
Date: 	Apr 10, 2011
*/

#include <stdio.h>
#include <string.h>
#include <CL/cl.h>

#define CL_SRC_SIZE 8192
#define GPU_SPLIT 512				//Not used yet
#define N_MISC 2				//Stores Misc values

#define CL_FILE_NAME "floatCL.cl"		//.cl filename

//Statement used for debugging
#define CL_PE if(error!=CL_SUCCESS) printf("\nERROR %d ", error);

//Parallel Kernels
#define KERNEL2D_FIND_MAXA "find_max_A"
#define KERNEL_FIND_MAXB "find_max_B"
#define KERNEL_ZERO_MEM "zero_mem"
#define KERNEL_COPY "copy"

//Serial Kernels
#define SKERNEL_RESET_SORT "reset_sort"
#define SKERNEL_SORT_FILL "sort_fill"

//Init & dealloc OpenCL
void init_custom_cl(int);
void clean_custom_cl();

//GPU Memory access
cl_mem put_to_GPURAM(float *a, int n);
cl_mem only_alloc_mem(int n);
cl_int read_from_GPURAM(cl_mem mem, float *a, int n);

//Kernel calls
cl_int call_copy(cl_mem a, cl_mem b, int no);
cl_int call_max(cl_mem vals, cl_mem ret, int no);
cl_int call_zero_mem(cl_mem vals, int no);
cl_int call_sort(cl_mem vals, cl_mem index, int no);
cl_int call_reset_sort();
cl_int call_sort_fill(cl_mem vals, cl_mem index);
