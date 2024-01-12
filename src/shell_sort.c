#ifndef SHELL_SORT
#include "shell_sort.h"
#endif

double shell_sort(unsigned int* arr, int count, int threads)
{
	int k;
	double start, end;
	start = omp_get_wtime();

	for (int d = count / 2; d > 0; d /= 2) {
		#pragma omp parallel for num_threads(threads), private(k)
		for (k = 0; k < d; ++k)						// каждый поток меняет только свои данные
			for (int i = d + k; i < count; i += d)	// сортировка вставками
				for (int j = i - d; j >= 0; j -= d) {
					if (arr[j] <= arr[j + d]) break;
					unsigned int tmp = arr[j];
					arr[j] = arr[j + d];
					arr[j + d] = tmp;
				}
	}

	end = omp_get_wtime();
	return end - start;
}
