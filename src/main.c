#ifndef SHELL_SORT
#include "shell_sort.h"
#endif

#include <stdio.h>



int main(int argc, char** argv)
{
	FILE* file;
	errno_t status = fopen_s(&file, "../par_prog-6/report/data.txt", "w");
	if (status != 0) {
		printf("File was not opened!\nError code: %d\n", status);
		return 0;
	}

	int available_threads = omp_get_num_procs();
	//available_threads = 1;
	printf("OpenMP: %d; Threads: %d\n", _OPENMP, available_threads);

	int count = 10000000, times = 10;
	unsigned int* arr;
	arr = (unsigned int*)malloc(sizeof(unsigned int) * count * times * available_threads);
	if (arr == NULL) {
		fclose(file);
		printf("Memory error!\n");
		return 0;
	}
	for (int i = 0; i < count * times * available_threads; ++i) rand_s(arr + i);

	status = fprintf(file, "THREADS\tNUMBER\tTIME\t\tTIMES:\t%d\n", times);
	if (status < 0) printf("Data was not written!\nError code: %d\n", status);

	for (int threads = 0; threads < available_threads; ++threads)
		for (int i = 0; i < times; ++i) {
			double duration = shell_sort(arr + (threads * times + i) * count, count, threads + 1);

			fprintf(file, "%d\t%d\t%f\n", threads + 1, i, duration);
			if (status < 0) printf("Data was not written!\nError code: %d\n", status);
		}

	free(arr);
	fclose(file);
	return 0;
}