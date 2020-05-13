#include <stdio.h>
#include <time.h>

using namespace std;

void readArrayFromFile(FILE*,int*,int);
void shellSort(int*,int);
int  getHashKey(int*);

int main () {

    FILE *myFile;
    myFile = fopen("unsorted10000000.txt", "r");
    
    int size = 10000000, hashKey = 0;
    int *numberArr = new int[size];
    clock_t start, end;
    double cpu_time_used_sec;
    
    readArrayFromFile(myFile, numberArr, size);

    start = clock();
    shellSort(numberArr, size);
    end = clock();

    cpu_time_used_sec = ((double) (end - start)) / CLOCKS_PER_SEC;
    int cpu_time_used_ms = (int)(cpu_time_used_sec * 1000);
    
    hashKey = getHashKey(numberArr);
    hashKey %= 100000;

    delete[] numberArr;

    printf("MAC CPU : 3.1 GHz Intel Core i5\n");
    printf("Memory : 8GB\n");
    printf("동작시간 : %d msec\n", cpu_time_used_ms);
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

void shellSort(int* arr, int size) {
    int gap = size/2;
    while(gap){
        for(int i = 0; i < gap; ++i){
            for(int j=i+gap; j<size; j+=gap){
                int k=j, cur = arr[j];
                while((k-=gap) >=0 && arr[k] > cur){
                     arr[k+gap] = arr[k];
                     arr[k] = cur;
                }  
            }
        }
        gap/=2;
    }
    
}

int getHashKey(int* arr) {
    int sum=0;
    for(int i = 4999999, end = 5000100; i < end;++i)
        sum += (arr[i] % 100000);
    return sum;
}