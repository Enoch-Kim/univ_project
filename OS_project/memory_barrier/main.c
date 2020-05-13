#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define FALSE 0
#define	TRUE 1
#define N 2


DWORD WINAPI thread_func_1(LPVOID lpParam);
void enter_region(int processNum);
void leave_region(int processNum);
int turn;
int interested[2];
volatile double	shared_var = 0.0;
int	job_complete[2] = {0, 0};

int main(void)
{
	DWORD dwThreadId_1, dwThrdParam_1 = 1;
    HANDLE hThread_1;
	int		i, j;
	// Create Thread 1
	hThread_1 = CreateThread(
        NULL,                        // default security attributes 
        0,                           // use default stack size  
        thread_func_1,                  // thread function 
        &dwThrdParam_1,                // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId_1);                // returns the thread identifier 
 
   // Check the return value for success.
 
    if (hThread_1 == NULL) 
    {
       printf("Thread 1 creation error\n");
 	  exit(0);
    }
    else 
    {
       CloseHandle( hThread_1 );
    }

	/* I am main thread */
	/* Now Main Thread and Thread 1 runs concurrently */

	int processNum = 0;
	for (i = 0; i < 1; i++) {
		for (j = 0; j < 100000000; j++) {
			enter_region(processNum);
			shared_var++;
			leave_region(processNum);
		}
	}

	printf("Main Thread completed\n");
	job_complete[0] = 1;
	while (job_complete[1] == 0) ;

	printf("%f\n", shared_var);
	_getch();
	ExitProcess(0);
}


DWORD WINAPI thread_func_1(LPVOID lpParam)
{
	int		i, j;
	int processNum = 1;
	for (i = 0; i < 1; i++) {
		for (j = 0; j < 100000000; j++) {
			enter_region(processNum);
			shared_var++;
			leave_region(processNum);
		}
	}
	printf("Thread_1 completed\n");
	job_complete[1] = 1;
	ExitThread(0);
}

void enter_region(int processNum) {
	int other = 1 - processNum;
	interested[processNum] = TRUE;
	turn = processNum;
	// set memory barrier
	__asm mfence;
	while (turn == processNum && interested[other] == TRUE);
}

void leave_region(int processNum) {
	interested[processNum] = FALSE;
}