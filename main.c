#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rw.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//структура для данных потока
typedef struct{
	int len; //length of line
	struct Complex **A, **B, **Res; //pointers of matrixs
	int line, column;
} pthrData;

struct Complex multiplyComplex(struct Complex a, struct Complex b){
	struct Complex c;
	c.re = a.re*b.re - a.im*b.im;
	c.im = a.im*b.re + b.im*a.re;
	return c;
}

void* threadFunc(void* thread_data){


	pthrData *data = (pthrData *) thread_data;

    data->Res[data->line][data->column].re = 0;

	for(int i = 0; i < data->len; i++){
            data->Res[data->line][data->column].re += multiplyComplex(data->A[data->line][i], data->B[i][data->column]).re;
            data->Res[data->line][data->column].im += multiplyComplex(data->A[data->line][i], data->B[i][data->column]).im;
    }


	return (void *)data;
}

int main(int argc, char* argv[]){
    int m, n, p, q; //размерности матриц
	writeString("Добро пожаловать в программу по перемножению 2-х матриц с комплексными элементами.\n");

    writeString("Введите размерность (m x n) 1-й матрицы.\n");
	writeString("m = ");
	m = readInt(); if(m <= 0) error("m > 0", -1);
	writeString("n = ");
	n = readInt(); if(n <= 0) error("n > 0", -2);

    writeString("Введите размерность (p x q) 2-й матрицы.\n");
	writeString("p = ");
	p = readInt(); if(p <= 0) error("p > 0", -3);
	if(p != n) error("Матрицы не сопряжены. Перемножение невозможно.", -4);
	writeString("q = ");
	q = readInt(); if(q <= 0) error("q > 0", -5);

	int maxCountThread = 0;
    if (argc >= 2){
        maxCountThread = atoi(argv[1]);
    }

    struct Complex **A = (struct Complex**) malloc(m * sizeof(struct Complex*));
    for (int i = 0; i < m; ++i)
        A[i] = (struct Complex*) malloc(n * sizeof(struct Complex));

    struct Complex **B = (struct Complex**) malloc(p * sizeof(struct Complex*));
    for (int i = 0; i < p; ++i)
        B[i] = (struct Complex*) malloc(q * sizeof(struct Complex));
	
    struct Complex **Res = (struct Complex**) malloc(m * sizeof(struct Complex*));
    for (int i = 0; i < m; ++i)
        Res[i] = (struct Complex*) malloc(q * sizeof(struct Complex));

    writeString("Введите элементы 1-й матрицы:\n");
    for(int i = 0; i < m; i++)
		for(int j = 0; j < n; j++)
			A[i][j] = readComplex();

	writeString("Введите элементы 2-й матрицы:\n");
    for(int i = 0; i < p; i++)
		for(int j = 0; j < q; j++){
			B[i][j] = readComplex();
        }

//* вывод матриц
    writeString("Введёная мaтрица1:\n"); 
    for (int i = 0; i < m; i++){
        for (int j = 0; j < n; j++) { writeComplex(A[i][j]); writeChar(' '); }
        writeChar('\n');
    }
    writeString("Введёная мaтрица2:\n"); 
    for (int i = 0; i < p; i++){
        for (int j = 0; j < q; j++) { writeComplex(B[i][j]); writeChar(' '); }
        writeChar('\n');
    }
//*/

	//выделяю память под массив идентификаторов потоков
	pthread_t* threads = (pthread_t*) malloc(m*q * sizeof(pthread_t));
	//сколько потоков - столько и структур с потоковых данных
	pthrData* threadData = (pthrData*) malloc(m*q * sizeof(pthrData));
	//запуск потоков
	int status, jj = -1;
	for(int i = 0; i < m; i++){
		for(int j = 0; j < q; j++){
            threadData[i*n + j].len = n;
            threadData[i*n + j].A = A;
            threadData[i*n + j].B = B;
            threadData[i*n + j].Res = Res;

			threadData[i*n + j].line = i;
			threadData[i*n + j].column = j;

            status = pthread_create(&(threads[i*n + j]), NULL, threadFunc, (void *) &threadData[i*n + j]);
			if(status != 0){
				error("ошибка создания потока\n",-5);
			}
		}
	}

	//ожидаем выполнение всех потоков
	for(int i = 0; i < m*q; i++)
		pthread_join(threads[i], NULL);

	//вывод результата
	writeString("Результирующая матрица:\n");
	for(int i = 0; i < m; i++){
		for(int j = 0; j < q; j++){
			writeComplex(threadData[i*n + j].Res[i][j]);
			writeChar(' ');
		}
		writeChar('\n');
	}

    for(int i = 0; i < m; ++i)
        free(A[i]);
    free(A);

    for(int i = 0; i < q; ++i)
        free(B[i]);
    free(B);
    
    for(int i = 0; i < m; ++i)
        free(Res[i]);
    free(Res);

	free(threads);
	free(threadData);

	pthread_mutex_destroy(&mutex);

	return 0;
}