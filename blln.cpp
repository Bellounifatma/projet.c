#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define T_MAT 3
#define BUFFER_SIZE 100

int B[T_MAT][T_MAT], C[T_MAT][T_MAT], A[T_MAT][T_MAT];
int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

void *producteur(void *arg) {
    int row = *((int *)arg);
    for (int j = 0; j < T_MAT; ++j) {
        int result = 0;
        for (int k = 0; k < T_MAT; ++k) {
            result += B[row][k] * C[k][j];
        }

        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&buffer_not_full, &mutex);
        }

        buffer[count++] = result;
        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void *consommateur(void *arg) {
    for (int i = 0; i < T_MAT; ++i) {
        for (int j = 0; j < T_MAT; ++j) {
            pthread_mutex_lock(&mutex);
            while (count == 0) {
                pthread_cond_wait(&buffer_not_empty, &mutex);
            }

            A[i][j] = buffer[--count];
            pthread_cond_signal(&buffer_not_full);
            pthread_mutex_unlock(&mutex);
        }
    }
    pthread_exit(NULL);
}

int main() {
    // Fill matrices B and C with 1
    for (int i = 0; i < T_MAT; ++i) {
        for (int j = 0; j < T_MAT; ++j) {
            B[i][j] = 1;
            C[i][j] = 1;
        }
    }

    pthread_t producteurs[T_MAT];
    for (int i = 0; i < T_MAT; ++i) {
        pthread_create(&producteurs[i], NULL, producteur, &i);
    }

    pthread_t consommateur_thread;
    pthread_create(&consommateur_thread, NULL, consommateur, NULL);

    for (int i = 0; i < T_MAT; ++i) {
        pthread_join(producteurs[i], NULL);
    }

    pthread_join(consommateur_thread, NULL);

    // Display matrix A
    printf("Matrix A:\n");
    for (int i = 0; i < T_MAT; ++i) {
        for (int j = 0; j < T_MAT; ++j) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    return 0;
}
