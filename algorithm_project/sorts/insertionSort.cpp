#include <stdio.h>
#include <time.h>

using namespace std;

void readArrayFromFile(FILE* f, int* arr , int size);
void insertionSort(int* arr, int size);
int  getHashKey(int* arr);

int main () {

    FILE *myFile;
    myFile = fopen("unsorted10000000.txt", "r");
    
    int size = 10000000, hashKey = 0;
    int *numberArr = new int[size];
    clock_t start, end;
    double cpu_time_used_sec;
    
    readArrayFromFile(myFile, numberArr, size);

    start = clock();
    insertionSort(numberArr, size);
    end = clock();

    cpu_time_used_sec = ((double) (end - start)) / CLOCKS_PER_SEC;
    int cpu_time_used_ms = (int)(cpu_time_used_sec * 1000);
    
    hashKey = getHashKey(numberArr);
    hashKey %= 100000;

    delete[] numberArr;

    printf("MAC CPU : 3.1 GHz Intel Core i5\n");
    printf("Memory : 8GB\n");
    printf("동작시간 : %d msec (%d sec)\n", cpu_time_used_ms, (int)cpu_time_used_sec);
    printf("Hash Value : %3d\n\n", hashKey);
    return 0;
}

void readArrayFromFile(FILE* f, int* arr , int size) {
    f = fopen("unsorted10000000.txt", "r");
    if (f == NULL) 
        throw "파일 읽기 오류!\n";

    for (int i = 0; i < size; i++){
        fscanf(f, "%d", &arr[i]);
    }
    fclose(f);
    printf("파일을 읽어오는데 성공했습니다.\n\n");
}

void insertionSort(int* arr, int size) {
    int cur = 0, j = 0;
    for(int i = 0; i < size; ++i){
        cur = arr[i];
        j = i;
        while(--j >= 0 && cur < arr[j])
            arr[j+1] = arr[j];
        arr[j+1] = cur;
    }
}

int getHashKey(int* arr) {
    int sum = 0, i = 4999999, end = 5000100;
    for(;i < end;++i)
        sum += (arr[i] % 100000);
    return sum;
}

