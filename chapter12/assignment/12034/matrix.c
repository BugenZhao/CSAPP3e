//
// Created by Bugen Zhao on 2020/3/7.
//

/*
 * Thread	Time		Time(CPU)
 *     1	13.313456	13.319079
 *     2	6.506039	12.932351
 *     3	4.488464	13.329639
 *     4	3.425204	13.532158
 *     5	2.746309	13.433172
 *     6	2.382196	13.903730
 *     7	2.140675	14.681017
 *     8	1.870877	14.898794
 *     9	1.910323	17.145612
 *     10	2.034612	20.319235
 *     11	2.052115	22.538085
 *     12	2.005638	24.011354
 *     13	1.955780	25.294427
 *     14	1.966276	27.432828
 *     15	1.960080	29.287775
 *     16	1.971896	31.070238
 */

#include "bzcsapp.h"
#include "csapp.h"
#include <time.h>

#define ROW 2047
#define COL 2047

int A[ROW][COL];
int B[COL][ROW];
int ans[ROW][ROW];

sem_t *sem;

void *worker(void *_row) {
    int buf[ROW] = {};
    int row = *(int *) _row;
    int i, col;

    for (i = 0; i < COL; ++i) {
        for (col = 0; col < ROW; ++col) {
            buf[col] += A[row][i] * B[i][col];
        }
    }

    for (i = 0; i < ROW; ++i) {
        ans[row][i] = buf[i];
    }

    V(sem);
}

void mul(unsigned thread) {
    int i;
    pthread_t tid[ROW];
    int param[ROW];


//    printf("START - %2d\n", thread);
    sem = Sem_open_and_unlink("/matrix_sem_", thread);
    for (i = 0; i < ROW; ++i) {
        P(sem);
        param[i] = i;
        Pthread_create(tid + i, NULL, worker, param + i);
    }
    for (i = 0; i < ROW; ++i) {
        Pthread_join(tid[i], NULL);
    }
//    printf("END   - %2d\n", thread);

    Sem_close(sem);
}

int main() {
    int thread;
    clock_t cpu_begin, cpu_end;
    struct timeval start, end;
    printf("Thread\tTime\t\tTime(CPU)\n");
    for (thread = 1; thread <= 16; thread++) {
        gettimeofday(&start, NULL);
        cpu_begin = clock();
        mul(thread);
        cpu_end = clock();
        gettimeofday(&end, NULL);
        printf("%d\t%lf\t%lf\n", thread,
               (double) (end.tv_sec - start.tv_sec) + 1e-6 * (end.tv_usec - start.tv_usec),
               (double) (cpu_end - cpu_begin) / CLOCKS_PER_SEC);
    }
}