#include <bits/stdc++.h>

typedef unsigned long long ull;
using namespace std;

bool isPrimeNum(ull num){
    if(num % 2 ==0 || num % 3 ==0) return false;
    ull sqrtNum = sqrt(num);
    for(ull i=5; i<= sqrtNum; i+=6){
        if(num % i == 0) return false;
    }
    for(ull i=7; i<= sqrtNum; i+=6){
        if(num % i == 0) return false;
    }
    return true;
}

int main () {
    ull maxNum = (ull)(powl(2,61) - 1);
    ull m = maxNum + 1;
    clock_t start, end;
    clock_t start1, end1;
    double cpu_time_used_ms = 0;
    bool result = false;

    start = clock();
    result = isPrimeNum(maxNum);
    end = clock();
    
    cpu_time_used_ms = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
    
    printf("MAC CPU : i5-7267U\n");
    printf("Memory : 8GB\n");
    printf("동작시간 : %f ms\n", cpu_time_used_ms);
    if(result)
        printf("결과 : Yes\n");
    else
        printf("결과 : No\n");

    return 0;
}