#include <cstdio>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <pthread.h>
#include <algorithm>
using namespace std;
 
const long MAX = 10000000L;  //数组中最大数
const long long MAX_NUM = 100000000L;  //排序数
const int thread = 4;       //线程数
const int thread_num = MAX_NUM / thread;  //每个线程排序的个数
 
int num[MAX_NUM];
int tmp_num[MAX_NUM];
 
pthread_barrier_t barrier;
 
/**
 * Initialized Data
 */
void init()
{
	srandom((int)time(NULL));
	for(int i = 0; i < MAX_NUM; ++i)
		num[i] = random() % MAX;
}
 
/**
 *quick sort function
 */
void qsorts(int *start, int *end)
{	
	int nums = end - start;
	if(nums > 0)
	{
		int index = 0;
		int flag = start[0];
		int i = 0, j = nums;
		while(i != j)
		{
			while(j > i && start[j] >= flag)
				--j;
			start[index] = start[j];
			index = j;
			while(i < j && start[i] <= flag)
				++i;
			start[index] = start[i];
			index = i;
		}
		start[index] = flag;
		qsorts(start, start + (i - 1));
		qsorts(start + j + 1, end);
	}
}
 
void* work(void *arg)  //线程排序函数
{
	long index = (long)arg;
	qsorts(num + index, num + index + thread_num - 1);
	pthread_barrier_wait(&barrier);
	pthread_exit(NULL);
}
 
void meger()        //最终合并函数
{
	long index[thread];
	for (int i = 0; i < thread; ++i)
	{
		index[i] = i * thread_num;
	}
 
	for(long i = 0; i < MAX_NUM; ++i)
	{
		long min_index;
		long min_num = MAX;
		for(int j = 0; j < thread; ++j)
		{
			if((index[j] < (j + 1) * thread_num) 
				&& (num[index[j]] < min_num))
			{
				min_index = j;
				min_num = num[index[j]];
			}
		}
		tmp_num[i] = num[index[min_index]];
		index[min_index]++;
	}
}
 
int main(int argc, char const *argv[])
{
	init();
	
	struct timeval start, end;
	pthread_t ptid;
	//printf("%ld %ld\n", num[1], num[2]);
	gettimeofday(&start, NULL);
 
	//init pthread and Thread barrier
	//add 1, total have (thread + 1) threads.
	pthread_barrier_init(&barrier, NULL, thread + 1);
	for(int i = 0; i < thread; ++i)
		pthread_create(&ptid, NULL, work, (void *)(i * thread_num));
 
	pthread_barrier_wait(&barrier);
 
	meger();
	// use one thread to sort
	// qsorts(num, num + MAX_NUM - 1);
 
	gettimeofday(&end, NULL);
	long long s_usec = start.tv_sec * 1000000 + start.tv_usec;
	long long e_usec = end.tv_sec * 1000000 + end.tv_usec;
 
	double useTime = (double)(e_usec - s_usec) / 1000000.0;
	printf("sort use %.4f seconds\n", useTime);
 
 
	FILE *fp = fopen("result2.txt", "w+");
	for(long long i = 0; i < MAX_NUM; ++i)
		fprintf(fp, "%ld ", tmp_num[i]);
 
	return 0;
}

