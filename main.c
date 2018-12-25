#include <stdint.h> 
#include <LPC177x_8x.h>

#define durationLed1 10000
#define durationLed2 10000
#define MAX_COUNT_OF_PROCESSES 100 // defines how much processes there can be....
#define TRUE 1

uint32_t pid=0;//to assign a pid to a process:value is only incremented.....
uint32_t currentId=0;//saves the place of the process in the queue which is running, nothing in common with pid....
uint32_t previousId=0;
uint32_t countOfProcesses=0;//holds the count of processes beeing active in the queue, used to "allocate memory"....

#define RUNNING 1
#define WAITING 2
#define FINISHED 3

typedef struct processControlBlock
{
	uint8_t m_Pid;
	uint32_t m_Duration;
	uint32_t m_LastCall;
	uintptr_t m_StackPtr;   //  <uintptr_t ? or stack as int?>
	uint32_t m_Stack[32];
	int m_Status;
	void (*m_Func)(void);
}pcb;

pcb processTable[MAX_COUNT_OF_PROCESSES];
pcb * currentProcess,previousProcess;

void led1(void);
void led2(void);
void init(void);
void delay(void);

void initProcessControlBlock(pcb* process,uint32_t duration,uint32_t lastCall,uint8_t status, void(*func)(void));
void getNextPid(void);//requests the next Id form 0 to MAX_COUNT_OF_PROCESS to 0....
void yield(void);
void deleteCurrentProcess(void);//remove the current Process from Queue

void schedule(void);//only round robin

void idle(void);

void HardFault_Handler(void);
void causeHardFault(void);


static pcb * ptr;//the pointer to the table of tasks
extern void firstProcess(uintptr_t stackPtr);//R0 - R3 
extern void contextSwitch(uintptr_t newCtx,uintptr_t *oldCtx);

int main(){
		
	init();
	
	initProcessControlBlock(&processTable[0],durationLed1,0,WAITING,led1);
	initProcessControlBlock(&processTable[1],durationLed1,0,WAITING,led2);
	
	
	ptr = processTable;
	
	//starte ersten prozess:
	firstProcess((uintptr_t)processTable[currentId].m_StackPtr);
	
}//main



void init()
{
	LPC_GPIO0->DIR |= 0xff00;
	LPC_GPIO0->CLR = 0xff00;
}

	
void delay()
{
	for(int i=0;i< 100;++i){}
}

void led1(void)
{
	static unsigned int position = 0;
	LPC_GPIO0->CLR=(1<<(position+8));	
	position = (position + 1) % 4;
	LPC_GPIO0->SET=(1<<(position+8)); // oberste LED an
	yield();
}

void led2(void)
{
	static unsigned int position = 3;
	LPC_GPIO0->CLR=(1<<(15-position));	
	position = (position + 1) % 4;
	LPC_GPIO0->SET=(1<<(15-position)); // oberste LED an
	yield();
}

void yield(void)
{
	//currentProcess->m_Status=WAITING;
	previousId = currentId;
	getNextPid();//move processCounter
	//schedule(); //brauchen wir hier nicht mehr weil durch contextSwitch ersetzt
	
	contextSwitch(processTable[currentId].m_StackPtr,&processTable[previousId].m_StackPtr);
}

void getNextPid(void)
{
	currentId = (currentId + 1) % countOfProcesses; 
}

void initProcessControlBlock(pcb* process,uint32_t duration,uint32_t lastCall,uint8_t status, void(*func)(void))
{
	process->m_Duration = duration;
	process->m_LastCall = lastCall;
	process->m_Status = status;
	process->m_Func = func;
	process->m_Pid = pid;
	pid++;
	process->m_StackPtr = (uintptr_t)&(process->m_Stack[31])-9*4;//is this castable???????
	process->m_Stack[31]=(uintptr_t)func; 
	
	countOfProcesses++;
}



void schedule(void)
{
 while(TRUE)
   {
	currentProcess = &ptr[currentId];
	if(currentProcess->m_Status == WAITING)
 	{
		currentProcess->m_Status = RUNNING;
        (*ptr[currentId].m_Func)();
   }
	else
	 {
		getNextPid();
	 }
  }
}

void deleteCurrentProcess(void)
{
	//just move one position left...
	//means last process occurs twice, and last occurence may be overwritten....
	for(int i = currentId+1;i < countOfProcesses-1;i++)
	{
		pcb* current = &processTable[i];//copy content from nextProcess into current
		pcb* next = &processTable[i+1];
		
		current->m_Pid = next->m_Pid;
		current->m_Duration = next->m_Duration;
		current->m_LastCall = next->m_LastCall;
		current->m_StackPtr = next->m_StackPtr;
		
		for(int k=0;k<32;k++)
		{
			/**copy stack*/
			current->m_Stack[k]=next->m_Stack[k];
		}
		
	    current->m_Status = next->m_Status;
	    current->m_Func = next->m_Func;
				
		processTable[i] = *current;
	}
	countOfProcesses--;
}

void HardFault_Handler(void)
{
	while(1){
	LPC_GPIO0->CLR=(1<<9);
	delay();
		LPC_GPIO0->SET=(1<<9);
	}
}

void causeHardFault(void)
{
		//save unaligned address:
    int * foo = (int*)0x000B;
	  *foo = (uint32_t)0x42424243;
}

void idle(void)
{
	while(TRUE)
	{
	}
}
