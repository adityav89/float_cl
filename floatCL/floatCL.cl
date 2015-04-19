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

//Zeros memory
__kernel void zero_mem(__global float *vals)
{
	const uint i = get_global_id(0);

	vals[i] = 0;
}

//The O(1) max_elt finder with n^2 processors
__kernel void find_max_A(__global float *vals, __global float *out)
{
    const uint i = get_global_id(0);
    const uint j = get_global_id(1);
    
    if(i > j)
    {
    	if(vals[i] < vals[j])
       	   out[i] = 1;
    	else out[j] = 1;
    }
}

__kernel void find_max_B(__global float *vals, __global float *retind)
{
    const uint i = get_global_id(0);

    if(vals[i] == 0)
       retind[0] = i;
}

//Copies from b to a
__kernel void copy(__global float *a, __global float *b)
{
	const uint i = get_global_id(0);

	a[i] = b[i];
}

//SERIAL GPU FN. below
__kernel void reset_sort(__global float *misc)
{
	misc[1] = 0;
}

__kernel void sort_fill(__global float *vals, __global float *misc, __global float *index)
{
	const uint i = (int)misc[0];
	const uint j = (int)misc[1];

	vals[i] = -9999;
	index[j] = i;
	misc[1] = misc[1] + 1;
}
