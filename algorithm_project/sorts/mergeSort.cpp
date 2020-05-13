#include <stdio.h>
#include <time.h>

using namespace std;
void readArrayFromFile(int* arr , int size); 
void merge(int* arr, int left, int right, int mid);
void mergeSort(int* arr, int left, int right);
int  getHashKey(int* arr);



int main () {
    int size = 10000000, hashKey = 0;
    int *numberArr = new int[size];
    clock_t start, end;
    double cpu_time_used_ms = 0;

    readArrayFromFile(numberArr, size);

    start = clock();
    mergeSort(numberArr, 0, size-1);
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
    
    for (int i = 0; i < size; i++){
        fscanf(myFile, "%d", &arr[i]);
    }
    
    fclose(myFile);
    printf("파일을 읽어오는데 성공했습니다.\n\n");
}


void merge(int* arr, int left, int right, int mid){
    int size = right - left + 1;
    int arr2[size];
    int i = left,
        j = mid + 1,
        count = 0,
        spare = 0;
    
    while(i <= mid && j <= right){
        arr2[count++] = arr[i] < arr[j] ? arr[i++] : arr[j++];
    }
    
    spare = (i == mid + 1) ? j : i;
    while(count <= size)
        arr2[count++] = arr[spare++];
    i = left;
    j = 0;
    while(i <= right)
        arr[i++] = arr2[j++];
}

void mergeSort(int* arr, int left, int right){
    int mid = (left + right) / 2;
    if(left == right)
        return;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid+1, right);
    merge(arr, left, right, mid);
}


int getHashKey(int* arr) {
    int sum = 0, i = 4999999, end = 5000100;
    for(;i < end;++i)
        sum += (arr[i] % 100000);
    return sum;
}

