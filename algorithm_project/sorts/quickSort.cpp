#include <stdio.h>
#include <time.h>
#include <thread>

using namespace std;

void readArrayFromFile(int* arr , int size);
int  partition(int* arr,int start, int end);
void subQuickSort(int* arr, int start, int end);
void getSortedRange(int* arr, int sortedRange[10][2], int size);
void merge(int* arr, int left, int mid, int right);
void quickSort(int* arr, int size);
int  getHashKey(int* arr);

int main () {
    int size = 10000000, hashKey = 0;
    int *numberArr = new int[size];
    clock_t start, end;
    double cpu_time_used_ms = 0;

    readArrayFromFile(numberArr, size);
    
    start = clock();
    quickSort(numberArr, size-1);
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

int partition(int* arr,int start, int end){
    int pick= arr[end];
    int i=start, j=end, temp=0;
    while(i<=j){
        while(i < end && arr[i] < pick)
            ++i;
        while(j >= start && arr[j] >= pick)
            --j;
        if(i>j){
            arr[end] = arr[i];
            arr[i] = pick;
        }else{
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    return i;
}

void subQuickSort(int* arr, int start, int end){
    if(end <= start) return;
    int dividingLine = partition(arr,start, end);
    subQuickSort(arr, start, dividingLine-1);
    subQuickSort(arr, dividingLine+1, end);
}

void getSortedRange(int* arr, int sortedRange[10][2], int size){
    int tenth = size/10;
    bool slop = false;                  // true == sorted,  false unsorted
    int begin=0, idx=0;
    for(int i=0; i<size-1; ++i){
        if(arr[i] <= arr[i + 1]){
            if(slop == false){
                begin = i;
                slop = true;
            }
            continue;
        }else{
            if(slop == true){
                if(i+1 - begin >= tenth){
                    sortedRange[idx][0] = begin;
                    sortedRange[idx][1] = i;
                    ++idx;
                }
                slop = false;
            }
            continue;
        }
    }
    if(slop == true && size-begin>=tenth){
        sortedRange[idx][0] = begin;
        sortedRange[idx][1] = size-1;
    }
}

void merge(int* arr, int left, int mid, int right){
    int leftSize = mid - left + 1, rightSize = right - mid;
    if(mid - left + 1 == 0 || rightSize == 0) return;
    if(arr[mid] <= arr[mid+1]) return;
    int size = right - left + 1;
    int* arr2 = new int[size];
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
    delete[] arr2;
}

void quickSort(int* arr, int size) {
    if(size < 1000000){
        subQuickSort(arr, 0, size-1);
        return;
    }
    int sortedRange[10][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    getSortedRange(arr, sortedRange, size);
    int count = 0;
    for(int i=0; i<10; ++i){
        if(sortedRange[i][1] != 0){
            count++;
        }
    }

    if(count == 0){
        thread th1(subQuickSort, arr, 0, size/3);
        thread th2(subQuickSort, arr, size/3+1, size/3*2);
        subQuickSort(arr, size/3*2+1, size-1);
        th1.join();
        th2.join();
        merge(arr, 0, size/3, size/3*2);
        merge(arr, 0, size/3*2, size-1);
        return;
    }
    thread* ths = new thread[count];
    ths[0] = thread(subQuickSort, arr, 0, sortedRange[0][0]);
    for(int i=1; i<count; ++i){
        ths[i] = thread(subQuickSort, arr, sortedRange[i-1][1]+1, sortedRange[i][0]-1);
    }
    subQuickSort(arr, sortedRange[count-1][1]+1, size-1);
    
    for(int i=0; i<count; ++i)
        ths[i].join();
    for(int i=count - 1; i>=0; --i)
        merge(arr, sortedRange[i][0], sortedRange[i][1], size-1);
    merge(arr, 0, sortedRange[0][0]-1, size-1);
    delete[] ths;
}

int getHashKey(int* arr) {
    int sum = 0, i = 4999999, end = 5000100;
    for(;i < end;++i)
        sum += (arr[i] % 100000);
    return sum;
}
