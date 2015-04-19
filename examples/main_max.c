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
Description: Main file for parallel largest element

*/

#include "floatCL/floatCL.h"
#include <stdio.h>
#include <sys/time.h>

#define DTIME(a, b) (((a.tv_sec-b.tv_sec)*1000 + (double)(a.tv_usec-b.tv_usec)/1000)+0.5f)

#define VALCNT 8192
#define BUFSIZE 65536
#define MBUFSIZE 16

int process_input_file(char *fname, float *vals)
{
	FILE *fil;
	int start, end, i;
	unsigned int size, no;
	char buf[BUFSIZE], micro_buf[MBUFSIZE];

	fil = fopen(fname, "r");
	if(!fil) {
		printf("\nERROR: Input text file missing(input.txt)");
		return 0;
	}

	start = end = no = 0;
	size = fread(buf, 1, sizeof buf, fil);
	for(i = 0; i < size; i++)
	{
		if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == ',')
		{
			end = i;
			buf[i] = '\0';
			strcpy(micro_buf, &buf[start]);
			start = end + 1;
			vals[no++] = atof(micro_buf);
		}
	}

	fclose(fil);
	fil = NULL;	

	return no;
}

int main()
{
	float vals[VALCNT], result[2];
	int n;
	timeval b_transfer, a_transfer, a_process;
	double a, b, c;
	cl_mem c_values, c_max;

	n = process_input_file((char*)"input.txt", vals);

	if(n)
	{
		init_custom_cl(n);
		
		gettimeofday(&b_transfer, NULL);
		c_values = put_to_GPURAM(vals, n);
		c_max = put_to_GPURAM(vals, 2);
		gettimeofday(&a_transfer, NULL);

		call_max(c_values, c_max, n);		
		gettimeofday(&a_process, NULL);

		read_from_GPURAM(c_max, result, 2);
		printf("\nTime Taken(milli-sec): \n------------");

		a = DTIME(a_transfer, b_transfer);
		b = DTIME(a_process, a_transfer);
		c = DTIME(a_process, b_transfer);

		printf("\n1. Transfer time to GPU: %lf\n2. Process time: %lf\n3. Total time: %lf", a, b, c);
		printf("\n\n-------------------------------------------------\n");
		printf("Max value is: %f at index: %d", vals[(int)result[0]], (int)result[0]);
		printf("\n--------------------------------------------------\n\n");

		clean_custom_cl();
		clReleaseMemObject(c_values);		
		clReleaseMemObject(c_max);
	}

	return 0;	
}
