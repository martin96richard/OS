#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 5
#define LEFT ( i + N - 1 ) % N
#define RIGHT ( i + 1 ) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define NB_EAT 10 //define the number of times of philosopher must eat

//mutex m initialized to 1
pthread_mutex_t m;
//mutex s[N] initialized to 0's
pthread_mutex_t s[N];
//int state[N] initiated to THINKING's
int state[N];
int philo_id[N]; //philosopher id
int nb_eat[N] = {0, 0, 0, 0, 0}; //counter to record the number of times a philosopher eats

//prototypes
void test(int i);

//
void down(pthread_mutex_t *mut) {
    pthread_mutex_lock(mut);
}

void up(pthread_mutex_t *mut) {
    pthread_mutex_unlock(mut);
}

void grab_forks(int i) {
    down(&m);
    state[i] = HUNGRY;
    printf("Philo(%d) is hungry...\n", i);
    test(i);
    up(&m);
    down(&s[i]);
}

void put_away_forks(int i) {
    down(&m);
    state[i] = THINKING;
    printf("Philo(%d) is thinking...\n", i);
    test(LEFT); //check if left philo is hungry : if yes : philo eat
    test(RIGHT);
    up(&m);
}

void test(int i) {
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        nb_eat[i]++;
        printf("Philo(%d) is eating...\n", i);
        up(&s[i]);
    }
}

void *philo_job(void *id) {
    int *i = id;

    for (int j = 0; j < NB_EAT; j++) {
        grab_forks(*i);
        sleep(1);
        put_away_forks(*i);
        printf("Philo(%d) have eaten %d times\n", *i, nb_eat[*i]);
    }
}

int main() {
    //thread id for each philo
    pthread_t philo[N];
    int error = 0;

    //initialize state array to THINKING & philo ids
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        philo_id[i] = i;
        printf("philo%d, right:%d, left:%d is thinking...\n", i, RIGHT, LEFT);
    }

    //initialize mutex m
    if (pthread_mutex_init(&m, NULL) != 0)
        return EXIT_FAILURE;
    //initialize all mutex s[]
    for (int i = 0; i < N; i++) {
        if (pthread_mutex_init(&s[i], NULL) != 0)
            return EXIT_FAILURE;
    }

    //initialize all philo threads
    for (int i = 0; i < N; i++) {
        if ((error = pthread_create(&philo[i], NULL, &philo_job, &philo_id[i])) != 0) {
            printf("Cannot create philo thread : [%s]", strerror(error));
        }
    }

    //wait until all threads are finished
    for (int i = 0; i < N; i++) {
        pthread_join(philo[i], NULL);
    }

    //destroy all mutex
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&s[i]);
    }
    pthread_mutex_destroy(&m);

    return 0;
}