#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

#define THREADS 4
#define N 3000

int primes[N];
int pflag[N];
int total = 0;

sem_t sem;

int is_prime(int v)
{
    int i;
    int bound = floor(sqrt((double)v)) + 1;
    for (i = 2; i < bound; i++)
    {
        /* No need to check against known composites */
        if (!pflag[i])
            continue;
        if (v % i == 0)
        {
            pflag[v] = 0;
            return 0;
        }
    }
    return (v > 1);
}

void *work(void *arg)
{
    int start;
    int end;
    int i;
    start = (N / THREADS) * (*(int *)arg);
    end = start + N / THREADS;
    for (i = start; i < end; i++)
    {
        // sem값 1 감소 -> 0이면 대기
        sem_wait(&sem);
        if (is_prime(i))
        {
            primes[total] = i;
            total++;
        }
        // sem값 1 증가 -> 대기중인 스레드 깨움
        sem_post(&sem);
    }
    return NULL;
}

int main(int argn, char **argv)
{
    int i;
    pthread_t tids[THREADS - 1];

    // sem = 1
    sem_init(&sem, 0, 1);

    int nums[THREADS];
    for (i = 0; i < THREADS; i++)
    {
        nums[i] = i;
    }

    for (i = 0; i < N; i++)
    {
        pflag[i] = 1;
    }
    for (i = 0; i < THREADS - 1; i++)
    {
        pthread_create(&tids[i], NULL, work, (void *)&nums[i]);
    }
    i = THREADS - 1;
    work((void *)&nums[i]);

    // 쓰레드 종료 대기
    for (i = 0; i < THREADS - 1; i++)
    {
        pthread_join(tids[i], NULL);
    }

    // sem 해제
    sem_destroy(&sem);

    printf("Number of prime numbers between 2 and %d: %d\n",
           N, total);
    for (i = 0; i < total; i++)
    {
        printf("%d\n", primes[i]);
    }

    return 0;
}