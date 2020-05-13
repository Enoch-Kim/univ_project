#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

#define left(x) x*2
#define right(x) x*2+1


void readArrayFromFile(int* arr , int size); 
void fixHeap(int *arr, int root, int K, int bound);
void heapSort(int* arr, int size);
int  getHashKey(int* arr);



int main () {
    int size = 10000000, hashKey = 0;
    int *numberArr = new int[size+1];
    clock_t start, end;
    double cpu_time_used_ms = 0;

    readArrayFromFile(numberArr, size);

    start = clock();
    heapSort(numberArr, size);
    end = clock();

    cpu_time_used_ms = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
    
    hashKey = getHashKey(numberArr);
    hashKey %= 100000;
    
    delete[] numberArr;

    printf("CPU : Dual Core i5-7267U\n");
    printf("Memory : 8GB\n");
    printf("동작시간 : %f ms\n", cpu_time_used_ms);
    printf("Hash Value : %d\n\n", hashKey);
    return 0;
}


void readArrayFromFile(int* arr , int size) {
    FILE *myFile;
    myFile = fopen("unsorted10000000.txt", "r");
    if (myFile == NULL) 
        throw "파일 읽기 오류!\n";
    
    for (int i = 1; i <= size; i++){
        fscanf(myFile, "%d", &arr[i]);
    }
    
    fclose(myFile);
    printf("파일을 읽어오는데 성공했습니다.\n\n");
}

void fixHeap(int *arr, int node, int K, int bound){ 
    int vacant = node;
    while(2*vacant<=bound){
        int largerChild = left(vacant);
        if(left(vacant) < bound && arr[right(vacant)] > arr[left(vacant)])
            largerChild = right(vacant);
        if(K<arr[largerChild]){
            arr[vacant] = arr[largerChild];
            vacant = largerChild;
        }else{
            break;
        }
    }
    arr[vacant] = K;
}

void heapSort(int* arr, int size){
    for(int i=(size)/2; i>=1; i--){
        fixHeap(arr, i, arr[i], size);
    }
    for(int i=size; i>=2; --i){
        int maxVal = arr[1];
        fixHeap(arr, 1, arr[i], i-1);
        arr[i] = maxVal;
    }
}

int getHashKey(int* arr) {
    int sum = 0, i = 5000000, end = 5000101;
    for(;i < end;++i)
        sum += (arr[i] % 100000);
    return sum;
}