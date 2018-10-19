#define NTHR   8                /* �߳��� */
#define NUMNUM 8000000L         /* �������� */
#define TNUM   (NUMNUM/NTHR)    /* ÿ���̷߳��䵽����Ҫ������� */
long nums[NUMNUM];
long snums[NUMNUM];
pthread_barrier_t b;    //����
 
bool compare(long a, long b)
{
    return a < b;
}
//�����߳�
//��nums����Ĵ�idx~idx+TNUM���ֽ��п�������
void *workThread(void *arg)
{
    long    idx = (long)arg;
 
    sort(&nums[idx],&nums[idx+TNUM],compare);
    pthread_barrier_wait(&b);
 
    pthread_exit(NULL);
}
 
//���Ѿ��ź�������nums��NTHR���ֽ��кϲ�
void merge()
{
    long idx[NTHR];  //idx��������nums��NTHR���ֵ���ʼλ��
    for (long i = 0; i < NTHR; i++)
        idx[i] = i * TNUM;
 
    for (long sidx = 0; sidx < NUMNUM; sidx++)
    {
        long minidx;
        long num = LONG_MAX;
 
        //��NTHR���ֵ������в��ҳ���С��һ��, ����index���浽idx[minidx]��
        for (long i = 0; i < NTHR; i++)
        {
            //idx[i] < (i+1)*TNUM ȷ������һ������֮��,
            //����������������໥�Ƚϵ����
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
 
    //����NTHR���̷ֱ߳���������ڵ�NTHR���ֽ�������
    struct timeval  start, end;
    pthread_t       tid;
    gettimeofday(&start, NULL);
    pthread_barrier_init(&b, NULL, NTHR+1);
    for (unsigned long i = 0; i < NTHR; i++)
        pthread_create(&tid, NULL,workThread, (void *)(i * TNUM));
    pthread_barrier_wait(&b);
    merge();
    gettimeofday(&end, NULL);
 
    //������ʱ
    long long startusec = start.tv_sec * 1000000 + start.tv_usec;
    long long endusec = end.tv_sec * 1000000 + end.tv_usec;
    double elapsed = (double)(endusec - startusec) / 1000000.0;
    printf("sort took %.4f seconds\n", elapsed);
 
    //�������Ľ��д���ļ�, �Ա�鿴�Ƿ��Ѿ��ź���
    FILE *fp = fopen("save.txt", "w+");
    for (unsigned long i = 0; i < NUMNUM; i++)
        fprintf(fp, "%ld ", snums[i]);
}

