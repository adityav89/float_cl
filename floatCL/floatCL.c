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

#include "floatCL.h"

cl_int error;
cl_platform_id platform;
cl_device_id device;
cl_uint platforms, devices;
cl_program prog;
cl_context context;
cl_command_queue cq;

cl_kernel k_copy, k_zero_mem, k_find_maxA, k_find_maxB, 
		k_sort_fill, k_reset_sort;

cl_mem ref_buf, ref_tmp, misc;


void init_custom_cl(int buf_size)
{
	FILE *fp;
	char cl_src[CL_SRC_SIZE];
	const char *const_src[] = {cl_src};
	size_t cl_src_size;

	error=clGetPlatformIDs(1, &platform, &platforms);

	error=clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &devices);

	cl_context_properties properties[]={CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0};
	context=clCreateContext(properties, 1, &device, NULL, NULL, &error);
	cq = clCreateCommandQueue(context, device, 0, &error);

	fp = fopen(CL_FILE_NAME, "r");
	cl_src_size = fread(cl_src, 1, CL_SRC_SIZE, fp);

	prog=clCreateProgramWithSource(context, 1, const_src, &cl_src_size, &error);
	error=clBuildProgram(prog, 0, NULL, "", NULL, NULL);
	if(error!=CL_SUCCESS)
	{
           puts("CL: RT compile error(parallel.cl)");
	   return;
	}

	k_copy = clCreateKernel(prog, KERNEL_COPY, &error);
	k_zero_mem = clCreateKernel(prog, KERNEL_ZERO_MEM, &error);
	k_find_maxA = clCreateKernel(prog, KERNEL2D_FIND_MAXA, &error);
	k_find_maxB = clCreateKernel(prog, KERNEL_FIND_MAXB, &error);
	k_sort_fill = clCreateKernel(prog, SKERNEL_SORT_FILL, &error);
	k_reset_sort = clCreateKernel(prog, SKERNEL_RESET_SORT, &error);

	ref_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) *  buf_size, NULL, &error);
	ref_tmp = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) *  buf_size, NULL, &error);
	misc = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) *  N_MISC, NULL, &error);
}

void clean_custom_cl()
{
	clReleaseKernel(k_copy);
	clReleaseKernel(k_zero_mem);
	clReleaseKernel(k_find_maxA);
	clReleaseKernel(k_find_maxB);
	clReleaseKernel(k_sort_fill);
	clReleaseKernel(k_reset_sort);

	clReleaseCommandQueue(cq);
	clReleaseContext(context);

	clReleaseMemObject(ref_buf);
	clReleaseMemObject(ref_tmp);
	clReleaseMemObject(misc);
}

cl_int call_sort(cl_mem vals, cl_mem index, int no)
{
	int i;
	int j;
	float todel[100];

	call_copy(ref_tmp, vals, no);		//used as a copier
	call_reset_sort();


	for(i=0; i<no; i++)
	{
		call_max(ref_tmp, misc, no);
		call_sort_fill(ref_tmp, index);
	}


}

cl_int call_reset_sort()
{
	const size_t worksize = 1;

	clSetKernelArg(k_reset_sort, 0, sizeof(misc), &misc);
	
	error = clEnqueueNDRangeKernel(cq, k_reset_sort, 1, NULL, &worksize, NULL, 0, NULL, NULL);
	error=clFinish(cq);
}

cl_int call_sort_fill(cl_mem vals, cl_mem index)
{
	const size_t worksize = 1;

	clSetKernelArg(k_sort_fill, 0, sizeof(vals), &vals);
	clSetKernelArg(k_sort_fill, 1, sizeof(misc), &misc);
	clSetKernelArg(k_sort_fill, 2, sizeof(index), &index);

	error = clEnqueueNDRangeKernel(cq, k_sort_fill, 1, NULL, &worksize, NULL, 0, NULL, NULL);
	error=clFinish(cq);
}

cl_int call_max(cl_mem vals, cl_mem ret, int no)
{
        const size_t worksize2d[] = {no, no};
	const size_t worksize = no;

	call_zero_mem(ref_buf, no);

	clSetKernelArg(k_find_maxA, 0, sizeof(vals), &vals);
	clSetKernelArg(k_find_maxA, 1, sizeof(ref_buf), &ref_buf);

	error = clEnqueueNDRangeKernel(cq, k_find_maxA, 2, NULL, worksize2d, NULL, 0, NULL, NULL);
	error=clFinish(cq);

	clSetKernelArg(k_find_maxB, 0, sizeof(ref_buf), &ref_buf);
	clSetKernelArg(k_find_maxB, 1, sizeof(ret), &ret);

	error = clEnqueueNDRangeKernel(cq, k_find_maxB, 1, NULL, &worksize, NULL, 0, NULL, NULL);
	error=clFinish(cq);
}

cl_int call_zero_mem(cl_mem vals, int no)
{
	const size_t worksize = no;

	clSetKernelArg(k_zero_mem, 0, sizeof(vals), &vals);

	error = clEnqueueNDRangeKernel(cq, k_zero_mem, 1, NULL, &worksize, NULL, 0, NULL, NULL);
	error=clFinish(cq);
}

cl_int call_copy(cl_mem a, cl_mem b, int no)
{
	const size_t worksize = no;

	clSetKernelArg(k_copy, 0, sizeof(a), &a);
	clSetKernelArg(k_copy, 1, sizeof(b), &b);

	error = clEnqueueNDRangeKernel(cq, k_copy, 1, NULL, &worksize, NULL, 0, NULL, NULL);
	error=clFinish(cq);
}

cl_int read_from_GPURAM(cl_mem mem, float *a, int n)
{
	error=clEnqueueReadBuffer(cq, mem, CL_TRUE, 0, sizeof(float) * n, a, 0, NULL, NULL);
	error=clFinish(cq);
	
	return error;
}

cl_mem put_to_GPURAM(float *a, int n)
{
	cl_mem mem;

	mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) *  n, NULL, &error);
	error = clEnqueueWriteBuffer(cq, mem, CL_TRUE, 0, sizeof(float) * n, a, 0, NULL, NULL);

	return mem;
}

cl_mem only_alloc_mem(int n)
{
	cl_mem mem;

	mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) *  n, NULL, &error);

	return mem;
}
