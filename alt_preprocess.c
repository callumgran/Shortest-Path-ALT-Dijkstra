/* 
TODO:

    Run dijkstra on every node:
        To landmark:
            Save distance to each landmark from each node.
            Write out the distance to each landmark.

        From landmark:
            Save distance from each landmark to each landmark.
            Write out the distance from each landmark.

        Keep the same format for both.

    Multithread:
        Check threads available.
        Check for memory available.
        Pre compute the memory needed for x threads.
        Find max threads that can be run.

        Create a new thread for each time that is dijkstra run whilst threads are under this amount.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// 8 * dijksta params
#define DIJKSTRA_PARAM_SIZE 8 * 8

// Fix to params for dijkstra
#define allocate_thread_params()                                              \
    {                                                                         \
        param[0];                                                             \
        param[1];                                                             \
        param[2];                                                             \
    }                                                                         \

int max_threads;
int n_threads;
int single_choice = 0;
int dual_choice = 1;

pthread_mutex_t t_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t t_cond = PTHREAD_COND_INITIALIZER;

static void *dijkstra_thread(void *arg)
{
    int **partition = (int **)arg;
    // Run Dijkstra
    free(arg);
    pthread_mutex_lock(&t_mutex);
    n_threads -= 1;
    if (n_threads == 0)
        pthread_cond_signal(&t_cond);
    pthread_mutex_unlock(&t_mutex);
}

static void create_thread()
{
    pthread_t thread;
    int **param = (int **)(malloc(DIJKSTRA_PARAM_SIZE));
    // allocate_thread_params(dijkstra params);
    pthread_mutex_lock(&t_mutex);

    n_threads += 1;

    pthread_mutex_unlock(&t_mutex);
    pthread_create(&thread, NULL, dijkstra_thread, param);
}

void preprocess(int *data, int len, int choice)
{
    pthread_t   thread;
    int         n_cpus;
    int         **param;

    n_cpus = sysconf(_SC_NPROCESSORS_ONLN);

    if (n_cpus > 0)
        max_threads = n_cpus;
    else
        max_threads = 4;
    
    param = (int **)(malloc(DIJKSTRA_PARAM_SIZE));
    // allocate_thread_params(dijkstra params);

    n_threads = 1;

    pthread_create(&thread, NULL, dijkstra_thread, param);
    pthread_mutex_lock(&t_mutex);
    pthread_cond_wait(&t_cond, &t_mutex);
    pthread_mutex_unlock(&t_mutex);
}
