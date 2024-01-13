#ifndef SHELL_SORT
#include "shell_sort.h"
#endif

//PS E:\Inst\3 course\5 sem\par_prog\labs\6\shell_sort> mpiexec -n 2 .\x64\Debug\shell_sort.exe

double shell_sort(unsigned int* arr, int count, int rank, int numtasks);

int main(int argc, char** argv)
{
	FILE* file = NULL;
	errno_t status;
	unsigned int* arr;
	unsigned int* tmp;
	int numtasks, rank, i, count = 1000, times = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if (rank == 0) {
		status = fopen_s(&file, "../par_prog-6/report/data.txt", "a");
		if (status != 0) {
			printf("File was not opened!\nError code: %d\n", status);
			return 0;
		}

		printf("OpenMP: %d; Processes: %d\n", _OPENMP, numtasks);

		arr = (unsigned int*)malloc(sizeof(unsigned int) * count * times);
		if (arr == NULL) {
			fclose(file);
			printf("Memory error!\n");
			return 0;
		}

		#pragma omp parallel for private(i)
		for (i = 0; i < count * times; ++i) rand_s(arr + i);

		status = fprintf(file, "NUMTASKS\tNUMBER\tDURATION\t\tTIMES:\t%d\n", times);
		if (status < 0) printf("Data was not written!\nError code: %d\n", status);
	}
	else arr = (unsigned int*)malloc(sizeof(unsigned int) * count);

	for (int i = 0; i < times; ++i) {
		if (rank == 0) tmp = arr + count * i;
		else tmp = arr;
		double duration = shell_sort(tmp, count, rank, numtasks);

		if (rank == 0) {
			fprintf(file, "%d\t%d\t%f\n", numtasks, i, duration);
			if (status < 0) printf("Data was not written!\nError code: %d\n", status);
		}

		/*
		if (rank == 0) {
			FILE* test;
			fopen_s(&test, "../par_prog-6/report/test.txt", "a");
			for (int id = 0; id < count; ++id) {
				fprintf(test, "%u\n", (arr + count * i)[id]);
				if (id > 0) if ((arr + count * i)[id] < (arr + count * i)[id - 1]) fprintf(test, "CRINGE\n");
			}
			fprintf(test, "\n\n");
			fclose(test);
		}
		*/
	}

	free(arr);
	if (rank == 0) fclose(file);

	MPI_Finalize();
	return 0;
	
}

double shell_sort(unsigned int* arr, int count, int rank, int numtasks)
{
	int k, id;
	double start, end;
	unsigned int* big_buf = NULL;

	if (rank == 0) big_buf = (unsigned int*)malloc(sizeof(unsigned int) * count * numtasks);
	
	MPI_Barrier(MPI_COMM_WORLD);
	start = omp_get_wtime();
	
	for (int d = count / 2; d > 0; d /= 2) {
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(arr, count, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
		
		for (k = 0; k < d; ++k)	
			if (k % numtasks == rank)
				for (int i = d + k; i < count; i += d)	// сортировка вставками
					for (int j = i - d; j >= 0; j -= d) {
						if (arr[j] <= arr[j + d]) break;
						unsigned int tmp = arr[j];
						arr[j] = arr[j + d];
						arr[j + d] = tmp;
					}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Gather(arr, count, MPI_UNSIGNED, big_buf, count, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

		if (rank == 0)
			for (int arr_id = 1; arr_id < numtasks; ++arr_id)
				for (int offset = arr_id; offset < d; offset += numtasks)
					for (int i = offset; i < count; i += d)
						arr[i] = big_buf[arr_id * count + i];
	}
	
	end = omp_get_wtime();
	if (rank == 0) free(big_buf);

	return end - start;
}