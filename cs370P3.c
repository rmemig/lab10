/*	Ryan Emig
	10/25/2013
	using teachers examples*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void usage();
int factorial(int n);
void *T1(void *t);
void *T2(void *t);

bool finished = false;
int count = 0 ;
pthread_mutex_t count_mutex;
#define MAX 10

typedef struct _thread_data{
int data_array[MAX];
} thread_data;

thread_data global_data;

int main(int argc, char *argv[])
{
	// Check for the correct number of arguments
	if(argc!=3)		
	{usage(); return(1);}

	int numOfProducerThreads = atoi(argv[1]);
	int numOfConsumerThreads = atoi(argv[2]);

	// Make sure integer arguments are between 1 and 5 inclusive
	if((numOfProducerThreads<1) || (numOfProducerThreads>5) || (numOfConsumerThreads<1) || (numOfConsumerThreads>5))
	{usage(); return(1);}

	pthread_t thread[numOfProducerThreads + numOfConsumerThreads];
	pthread_attr_t attr;
	int t;
	void *status;
	
	/* Initialize mutex and condition variable objects */
	pthread_mutex_init(&count_mutex, NULL);
	
	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(t=0; t<numOfProducerThreads; t++) 
	{pthread_create(&thread[t], &attr, T1, (void *)&t);}
	   
	for(t=0; t<numOfConsumerThreads; t++) 
	{pthread_create(&thread[t+numOfProducerThreads], &attr, T2, (void *)&t);}
	
	sleep(30);
	finished = true;
	
	/* Free attribute and wait for the other threads */
	for(t=0; t<numOfProducerThreads+numOfConsumerThreads; t++) 
	{pthread_join(thread[t], &status);}
	
	printf("Main program exiting after joining threads\n");
	/* Clean up and exit */
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&count_mutex);
	
	pthread_exit(NULL);
	return 0;
} // end main

int factorial(int n){
	int fact = 1;
	for (int i = 1; i <= n; i++) 
	{fact *= i;}
	return fact;
}

void usage()
{
	printf("Usage: This program takes 2 numbers from the command line, each number between 1 and 5 inclusive. The first of these numbers indicate the number of producer threads to create, the second of these numbers indicates the number of consumer threads to create.\n");
}

void *T1(void *t)
{
   printf("Producer started.\n");
	int i;
        while(!finished) {
		int randomNumber = rand() % 10;
		sleep(randomNumber);
		i = rand() % 10;

		if(count<10)
		{
	                pthread_mutex_lock(&count_mutex);
			global_data.data_array[count] = i;
	                count++ ;
	                printf("Producer added %d to queue, size = %d\n", i, count);
	                pthread_mutex_unlock(&count_mutex);
		}
		else
		{
			printf("Unable to add\n");
		}
        }
   printf("Producer finished\n");
   pthread_exit(NULL);
}

void *T2(void *t)
{
   printf("Consumer started.\n");
	int i;
        while(!finished) {
		int randomNumber = rand() % 10;
		sleep(randomNumber);		

		if(count>0)
		{
	        	pthread_mutex_lock(&count_mutex);
			i = global_data.data_array[count];
	                count-- ;
			//fact = factorial(i);
	                printf("Consumer removed %d, computed %d! = %d, queue size = %d\n", i, i, factorial(i), count);
	                pthread_mutex_unlock(&count_mutex);
		}
		else
		{
			printf("Unable to remove\n");
		}
	}
printf("Consumer finished\n");
pthread_exit(NULL);
}
