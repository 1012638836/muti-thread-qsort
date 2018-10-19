#define NTHR   8                /* 线程数 */
#define NUMNUM 8000000L         /* 待排序数 */
#define TNUM   (NUMNUM/NTHR)    /* 每个线程分配到的需要排序的数 */
long nums[NUMNUM];
long snums[NUMNUM];
pthread_barrier_t b;    //屏障
 
bool compare(long a, long b)
{
    return a < b;
}
//排序线程
//对nums数组的从idx~idx+TNUM部分进行快速排序
void *workThread(void *arg)
{
    long    idx = (long)arg;
 
    sort(&nums[idx],&nums[idx+TNUM],compare);
    pthread_barrier_wait(&b);
 
    pthread_exit(NULL);
}
 
//对已经排好序数组nums的NTHR部分进行合并
void merge()
{
    long idx[NTHR];  //idx保存数组nums的NTHR部分的起始位置
    for (long i = 0; i < NTHR; i++)
        idx[i] = i * TNUM;
 
    for (long sidx = 0; sidx < NUMNUM; sidx++)
    {
        long minidx;
        long num = LONG_MAX;
 
        //从NTHR部分的数组中查找出最小的一个, 将其index保存到idx[minidx]中
        for (long i = 0; i < NTHR; i++)
        {
            //idx[i] < (i+1)*TNUM 确保是在一个部分之中,
            //不会产生两个部分相互比较的情况
            if ((idx[i] < (i+1)*TNUM) && (nums[idx[i]] < num))
            {
                num = nums[idx[i]];
                minidx = i;
            }
        }
 
        snums[sidx] = nums[idx[minidx]];
        idx[minidx]++;
    }
}
 
int main()
{
    srandom(time(NULL));
    for (unsigned long i = 0; i < NUMNUM; i++)
        nums[i] = random();
 
    //创建NTHR个线程分别对数组相邻的NTHR部分进行排序
    struct timeval  start, end;
    pthread_t       tid;
    gettimeofday(&start, NULL);
    pthread_barrier_init(&b, NULL, NTHR+1);
    for (unsigned long i = 0; i < NTHR; i++)
        pthread_create(&tid, NULL,workThread, (void *)(i * TNUM));
    pthread_barrier_wait(&b);
    merge();
    gettimeofday(&end, NULL);
 
    //计算用时
    long long startusec = start.tv_sec * 1000000 + start.tv_usec;
    long long endusec = end.tv_sec * 1000000 + end.tv_usec;
    double elapsed = (double)(endusec - startusec) / 1000000.0;
    printf("sort took %.4f seconds\n", elapsed);
 
    //将排序后的结果写入文件, 以便查看是否已经排好序
    FILE *fp = fopen("save.txt", "w+");
    for (unsigned long i = 0; i < NUMNUM; i++)
        fprintf(fp, "%ld ", snums[i]);
}

