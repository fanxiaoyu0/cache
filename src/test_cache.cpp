#define _GNU_SOURCE
#include<stdio.h>
#include<sched.h>
#include<time.h>

int main()
{
    // Bind program to a certain cpu core 
    cpu_set_t mask;
    CPU_ZERO(&mask); // clear cpu mask
    CPU_SET(3, &mask); // set cpu
    if(sched_setaffinity(0, sizeof(mask), &mask)<0) perror("sched_setaffinity\n");

    // Test cache size
    printf("Test cache size -------------------\n"); 
    for (int i=0;i<16;i++){ // arraySize from 1KB to 64MB 
        int accessTimes = 64 * 1024 * 1024;
        register int arraySize=(1<<i)*1024;
        int*a=new int[arraySize];
        clock_t startTime=clock();
        for (int j = 0; j < accessTimes; j++){
            a[(j*59) & (arraySize-1)]++; // because (x&(arraySize-1))==(x%arraySize)
        }
        clock_t endTime=clock();
        delete[] a;
        double usedTime=1000.0*(endTime-startTime)/CLOCKS_PER_SEC;
        printf("%d KB: %.2f ms\n",arraySize/1024*4,usedTime); // arraySize*4 because sizeof(int)=4
    }

    // Test cacheline size
    printf("Test cacheline size -----------------\n");
    for (int i=1;i<=6;i++){
        int accessTimes = 64 * 1024 * 1024;
        register int arraySize=1024*1024;
        int stride=(1<<i);
        int*a=new int[arraySize];
        clock_t startTime=clock();
        for (int j = 0; j < accessTimes; j++){
            a[(j*stride)&(arraySize-1)]++;
        }
        clock_t endTime=clock();
        delete[] a;
        double usedTime=1000.0*(endTime-startTime)/CLOCKS_PER_SEC;
        printf("%d: %.2f\n",4*stride,usedTime);
    }

    // Test the associativity
    printf("Test the associativity --------------\n");
    for (int i=3;i<=8;i++){
        int accessTimes = 64 * 1024 * 1024;
        register int arraySize=128*1024; // 2^17
        int blockSize=1<<(17-i); // blockNumber=2^i
        int*a=new int[arraySize];
        clock_t startTime=clock();
        for (int j = 0; j < accessTimes; j++){
            register int temp=j&(arraySize-1);
            if ((temp/blockSize)%2==1)  a[temp]++;
        }
        clock_t endTime=clock();
        delete[] a;
        double usedTime=1000.0*(endTime-startTime)/CLOCKS_PER_SEC;
        printf("%d: %.2f\n",1<<(i),usedTime);
    }
}