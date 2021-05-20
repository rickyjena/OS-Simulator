// header files

//Library Files Used
#include <pthread.h>

//Header Files Linked
#include "simulator_384091.h"
#include "simtimer.h"

PCB * pcbOrderedQueue [10];
int pcbOrderQueueRear = 0;

PCB * pcbBlockQueue [10];
int maxQueueSize = 10;
int pcbBlockQueueRear = 0;
Boolean isCPUIdle = False;
PCB * pcbInteruptQueue [10];
int inteQueueRear = 0;

//run simulator function
// Runs all the methods needed for simulator machine
void runSimulator(ConfigDataType *configData, OpCodeType *metaData)
{
   // Initializes Variables used in Simulator Machine
   PCB *PCBPtrPrev;
   int swapFlag;
   PCB *PCBPtr1;
   PCB *NullPtr;
   PCB *PCBPtrNext;

   logToLoc(configData, "OS: System Start", True, False);

   logToLoc(configData, "OS: Create Process Control Blocks", True, False);

   PCB *pcbLinkHead = setProcesses(configData, metaData);

   logToLoc(configData, "OS: All processes initialized in New state", 
                                                            True, False);

   initializePCB(pcbLinkHead);

   logToLoc(configData, "OS: All processes now set in Ready state", 
                                                            True, False);

   //Cases for both cpu scheduling codes:
   
   //For FCFS
   if (configData->cpuSchedCode == CPU_SCHED_FCFS_N_CODE)
   {
      runCPUScheduler(pcbLinkHead, configData, metaData);
   }

   //For SJF
   else if (configData->cpuSchedCode == CPU_SCHED_SJF_N_CODE)
   {
      //Sort the PCB List for SJF
      NullPtr = NULL;
      swapFlag = 1;

      do
      {
         swapFlag = 0;
         PCBPtr1 = pcbLinkHead;
         PCBPtrPrev = pcbLinkHead;
         while ((PCBPtr1 != NullPtr) && PCBPtr1->next != NullPtr)
         {
            if (PCBPtr1->totalCycleTime > PCBPtr1->next->totalCycleTime)
            {
               PCBPtrNext = PCBPtr1->next->next;
               if (PCBPtr1 == pcbLinkHead)
               {
                  pcbLinkHead = PCBPtr1->next;
               }
               PCBPtrPrev->next = PCBPtr1->next;
               PCBPtrPrev->next->next = PCBPtr1;
               PCBPtr1->next = PCBPtrNext;
               swapFlag = 1;
            }
            PCBPtrPrev = PCBPtr1;
            PCBPtr1 = PCBPtr1->next;
         }

      } while (swapFlag);
      //Run for SJF after Sorting of PCB is done
      runCPUScheduler(pcbLinkHead, configData, metaData);
   }

   else if (configData->cpuSchedCode == CPU_SCHED_FCFS_P_CODE)
   {
      runFCFS_P_CPUScheduler(pcbLinkHead, configData, metaData);
   }

   else if (configData->cpuSchedCode == CPU_SCHED_RR_P_CODE)
   {
      printf('Need help dawg');
   }

   else if (configData->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
   {
      NullPtr = NULL;
      swapFlag = 1;

      do
      {
         swapFlag = 0;
         PCBPtr1 = pcbLinkHead;
         PCBPtrPrev = pcbLinkHead;
         while ((PCBPtr1 != NullPtr) && PCBPtr1->next != NullPtr)
         {
            if (PCBPtr1->totalCycleTime > PCBPtr1->next->totalCycleTime)
            {
               PCBPtrNext = PCBPtr1->next->next;

               if (PCBPtr1 == pcbLinkHead)
               {
                  pcbLinkHead = PCBPtr1->next;
               }

               PCBPtrPrev->next = PCBPtr1->next;
               PCBPtrPrev->next->next = PCBPtr1;
               PCBPtr1->next = PCBPtrNext;
               swapFlag = 1;
            }
            PCBPtrPrev = PCBPtr1;
            PCBPtr1 = PCBPtr1->next;
         }

      } while (swapFlag);

      runFCFS_P_CPUScheduler(pcbLinkHead, configData, metaData);
   }

   logToLoc(configData, "OS: System stop", True, False);

   clearPCB(pcbLinkHead);
}

int waitingQueue( PCB *pcb, ConfigDataType *configData, OpCodeType *metaData )
{
   double waitingTime = 0.0;

   double processTime = 0.0;

   int interruptFlag = 0;

   int index;
   PCB *pcbWaitingQueue;
   
   char *timeStr = (char *)malloc(10);
   for (index = 0; index < 10; index++)
   {
      timeStr[index] = ' ';
   }
   
   while( pcb->state == WAITING )
   {
      pcbWaitingQueue+=pcb->PCB_ID;

      waitingTime = accessTimer(LAP_TIMER, timeStr );

      processTime += (metaData->opValue * configData->ioCycleRate);

      if( waitingTime == processTime )
      {
         pcb->state = RUNNING;
         interruptFlag=1;
      }
   }

   return interruptFlag;
}

PCB * ReOrderPCBLinkedList(PCB *pcb, PCB * LastPCB)
{
   PCB * TopPCB = pcb;
   PCB * PrevPcb = NULL;

   while (pcb != NULL)
   {
      if(LastPCB == pcb)
      {
         if(PrevPcb != NULL) 
         {
            PrevPcb->next = pcb->next;
         }

         else 
         {
            TopPCB = LastPCB->next;
         }
      }
      PrevPcb = pcb;
      pcb = pcb->next;
   }
   if(PrevPcb != NULL)
   {
      PrevPcb->next=LastPCB;
   }

   LastPCB->next=NULL;
   return TopPCB;
}

void setProcessState(ConfigDataType *configData,PCB * pcb,  state_t st )
{
    pcb->state = st;
}

void printPCBStatus(ConfigDataType *configData,PCB * pcb )
{
   char stringHold [100] = "";
    //NEW, READY, WAITING, RUNNING, EXIT
    if(pcb->state == NEW )
    {
      sprintf(stringHold, "OS: Process %d set in NEW state",pcb->PCB_ID);
    }

    else if(pcb->state == READY )
    {  
      sprintf(stringHold, "OS: Process %d set in READY state",pcb->PCB_ID);
    }

    else if(pcb->state == WAITING )
    {
      sprintf(stringHold, "OS: Process %d set in WAITING state",pcb->PCB_ID);
    }

    else if(pcb->state == RUNNING )
    {
      sprintf(stringHold, "OS: Process %d set in RUNNING state",pcb->PCB_ID);
    }

    else if(pcb->state == EXIT )
    {
      sprintf(stringHold, "OS: Process %d set in EXIT state",pcb->PCB_ID);
    }

    logToLoc(configData, stringHold, True, False);
}

Boolean AddToBlockQueue(PCB * blockPCB)
{
   if(pcbBlockQueueRear < maxQueueSize)
   {
      int index = 0;
      for(index = 0; index<pcbBlockQueueRear; index++)
      {
         if(pcbBlockQueue[index] != NULL)
         {
            if(pcbBlockQueue[index]->PCB_ID > blockPCB->PCB_ID )
            {
               for(int maxindex=pcbBlockQueueRear-1; maxindex>index; 
                                                          maxindex--)
               {
                  pcbBlockQueue[maxindex+1] = pcbBlockQueue[maxindex];
               }

               pcbBlockQueue[index] = blockPCB;
               break;
            }

            else 
            {
               pcbBlockQueue[pcbBlockQueueRear] = blockPCB;
            }
            break;
         }
      }

      if(pcbBlockQueue[0] == NULL)
      {
        pcbBlockQueue[pcbBlockQueueRear] = blockPCB;
      }

      pcbBlockQueueRear++;
      return True;
   }
   return False;

}
PCB * GetPCBFromBlockQueue()
{
   PCB * blockPCB = pcbBlockQueue[0];
   int index;
   /*Initialize Scheduling Queue*/
   for(int index = 1; index < maxQueueSize; index++)
   {
      if(index <= pcbBlockQueueRear)
      {
        pcbBlockQueue[index-1] = pcbBlockQueue[index];
      }

      else
      { 
        pcbBlockQueue[index] = NULL;
      }

   }
   pcbBlockQueueRear--;
   //The queue rear can not be less than 0
   if(pcbBlockQueueRear < 0)
   {
      pcbBlockQueueRear = 0;
   }

   return blockPCB;
}

Boolean AddToInteruptQueue(PCB * schPCB)
{
   if(inteQueueRear < maxQueueSize)
   {
      pcbInteruptQueue[inteQueueRear] = schPCB;
      inteQueueRear++;
      return True;
   }
   return False;

}

Boolean IsSystemCPUIdle()
{
   Boolean isSysIdle = True;
   for(int index=0; index < pcbBlockQueueRear; index++ )
   {
      if(pcbOrderedQueue[index] != NULL) 
      {
         if(pcbOrderedQueue[index]->state != WAITING)
         {
            isSysIdle = False;
         }

      }

   }

   return isSysIdle;
}

PCB * GetFromInterupQueue( )
{
   PCB * schPCB = pcbInteruptQueue[0];
   int index;
   /*Initialize Scheduling Queue*/
   for(int index = 1; index < maxQueueSize; index++)
   {
      if(index <= inteQueueRear)
      {
        pcbInteruptQueue[index-1] = pcbInteruptQueue[index];
      }

      else 
      {
        pcbInteruptQueue[index] = NULL;
      }
   }
   inteQueueRear--;
   //The queue rear can not be less than 0
   if(inteQueueRear < 0)
   {
      inteQueueRear = 0;
   }

   return schPCB;
}

Boolean IsAllProcessDone(PCB * pcb)
{
   Boolean isAllDone = True;
   PCB * currPCB = pcb;
   while (currPCB != NULL)
   {
      if(currPCB->state != EXIT)
         {
            isAllDone = False;
            break;
         }

      currPCB = currPCB->next;
   }

   return isAllDone;

}

Boolean IsAllBlocked (PCB * headPCB)
{
      int index = 0;
      Boolean isAllBlocked = True;
      for(index=0; index < pcbOrderQueueRear; index++)
      {
         if(pcbOrderedQueue[index]->state != WAITING)
         {
            if(pcbOrderedQueue[index]->state != EXIT)
            {
               isAllBlocked = False;
            }

         }

      }

      return isAllBlocked;
}

PCB * GetShortestProcessingPCB(ConfigDataType *configData)
{
      //Get Shortest Possible Process
         int lowestTime = 0;
         PCB * nextPCB = NULL;
         for(int index=0; index < pcbOrderQueueRear; index++)
         {
            if(pcbOrderedQueue[index]->state != EXIT)
            {
               nextPCB = pcbOrderedQueue[index];
               break;
            }
         }

         for(int index=0; index < pcbOrderQueueRear; index++)
         {
            if(pcbOrderedQueue[index]->state != EXIT)
            {
               int currentTaskProcessTime = 
                      GetCurrentTaskProcessTime(configData, 
                                            pcbOrderedQueue[index]);

               if(currentTaskProcessTime != 0)
               {
                  if(lowestTime == 0 )
                  {
                     if( pcbOrderedQueue[index+1] != NULL )
                     {
                        if(pcbOrderedQueue[index+1]->state != EXIT)
                        {
                           int nextTaskProcessTime = 
                                    GetCurrentTaskProcessTime(configData, 
                                                   pcbOrderedQueue[index+1]);

                           if(currentTaskProcessTime > nextTaskProcessTime)
                           {
                            nextPCB = pcbOrderedQueue[index+1];
                            lowestTime = nextTaskProcessTime;
                           }

                           else 
                           { 
                              nextPCB = pcbOrderedQueue[index+1];
                              lowestTime = currentTaskProcessTime;
                           }
                        }
                     }
                  }
                  else 
                  {
                     if(currentTaskProcessTime < lowestTime)
                     {
                        nextPCB = pcbOrderedQueue[index];
                        lowestTime = currentTaskProcessTime;
                     }
                  }
               }
            }
         }
         return nextPCB;
   
}
PCB * GetNextProcess(ConfigDataType *configData, PCB * headPCB)
{
      PCB * nextPCB = NULL;
      if(!IsAllBlocked(headPCB)) {
         for(int index=0; index < pcbOrderQueueRear; index++)
         {
            if(pcbOrderedQueue[index]->state != EXIT)
            {
               if(pcbOrderedQueue[index]->state != WAITING)
               {
                  nextPCB = pcbOrderedQueue[index];
                  break;
               }
            }
         }
      }
   
      return nextPCB;
}
int GetCurrentTaskProcessTime( ConfigDataType * configData, PCB * currPCB){
   int timeRequired = 0;
   state_t ret_state = READY;
   char stringHold [100] = "";
   OpCodeType *metaDataHold = currPCB->currentMetaData;
    while (!(metaDataHold->opLtr == 'A' && compareString(metaDataHold->opName, "end") == STR_EQ))
      {
         if(metaDataHold->opLtr == 'A'){
            metaDataHold = metaDataHold->next;
            continue;
         }
         if(metaDataHold->opValue != 0)
         {
            timeRequired = getOpCodeCycleTime(configData, metaDataHold);
            break;
         }
         metaDataHold = metaDataHold->next;
      }
      return timeRequired;
}


Boolean NeedToBlock(ConfigDataType *configData, PCB * nextPCB, OpCodeType *metaDataHold)
{
   Boolean needToBlock = False;
   if(metaDataHold->opLtr == 'I' || metaDataHold->opLtr == 'O' )
   {
      if(metaDataHold->opValue!=0)
      {
         if(configData->quantumCycles < metaDataHold->opValue)
         {
            metaDataHold->opValue = metaDataHold->opValue - configData->quantumCycles;
         }

         needToBlock = True;
      }
   }
   else if (metaDataHold->opLtr == 'O') 
   {
         if(configData->quantumCycles < metaDataHold->opValue)
         {
            metaDataHold->opValue = metaDataHold->opValue - configData->quantumCycles;
            needToBlock = True;
         }

   }

   return needToBlock;
}

state_t ProcessInputOutPut( ConfigDataType *configData, PCB * nextPCB, OpCodeType *metaDataHold)
{
   state_t ret_state = READY;
   char stringHold [100] = "";
   if (metaDataHold->opLtr == 'I')
   {
      sprintf(stringHold, "Process %d, %s input start\n",
            nextPCB->PCB_ID, metaDataHold->opName);
      logToLoc(configData, stringHold, True,True);
   }
   
   else if (metaDataHold->opLtr == 'O')
   {
      sprintf(stringHold, "Process %d, %s output start\n",
            nextPCB->PCB_ID, metaDataHold->opName);
      logToLoc(configData, stringHold, True,True);
   }
   if(NeedToBlock(configData, nextPCB, metaDataHold))
   {
      setProcessState(configData, nextPCB, WAITING);
      AddToBlockQueue(nextPCB);
      return WAITING;
   }
 
   return ret_state;
}

state_t ProcessOperation(ConfigDataType *configData, PCB * nextPCB, OpCodeType *metaDataHold)
{
   state_t ret_state = READY;
   char stringHold [100] = "";
   int index = 0;
   for(index=0;index<100;index++)
   {
      stringHold[index] = 0;
   }

   sprintf(stringHold, "Process %d, %s operation start",
         nextPCB->PCB_ID, metaDataHold->opName);

   logToLoc(configData, stringHold, True,True);

   //setProcessState(configData, nextPCB, WAITING);
   stringHold [100] = "";

   if(NeedToBlock(configData, nextPCB, metaDataHold))
   {
      setProcessState(configData, nextPCB, WAITING);
      AddToBlockQueue(nextPCB);
      return WAITING;
   }

   int opCodeRunningTime = getOpCodeCycleTime(configData, metaDataHold);
   dealingWithThreadsWithoutJoin(configData, stringHold, opCodeRunningTime);
   
   if (metaDataHold->opValue != 0 ) 
   {
      metaDataHold->opValue = 0;
      nextPCB->state = READY;
   }

   stringHold [100] = "";
   sprintf(stringHold, "Process %d, %s operation end\n",
         nextPCB->PCB_ID, metaDataHold->opName);

   logToLoc(configData, stringHold, True,False);
    return ret_state;
}

void CompleteIOBlockedProcess(ConfigDataType *configData,
                             PCB * nextPCB, OpCodeType *metaData)
{
   char stringHold [100] = "";
   OpCodeType *metaDataHold = nextPCB->currentMetaData;
   state_t ret_state = WAITING;
    while (!(metaDataHold->opLtr == 'A' 
            && compareString(metaDataHold->opName, "end") == STR_EQ))
      {
         if(metaDataHold->opLtr == 'A')
         {
            metaDataHold = metaDataHold->next;
            continue;
         }

         //Process when opValue is not zero skip the nodes that has 
         //process value is 0 because CPU already processsed
         if(metaDataHold->opValue != 0)
         {
            int opCodeRunningTime = getOpCodeCycleTime(configData, metaDataHold);
            dealingWithThreadsWithoutJoin(configData, stringHold, opCodeRunningTime);
            metaDataHold->opValue = 0;
            if (metaDataHold->opLtr == 'I')
            {
               sprintf(stringHold, "Process %d, %s input end\n",
                     nextPCB->PCB_ID, metaDataHold->opName);
               logToLoc(configData, stringHold, True,True);
            }
            
            else if (metaDataHold->opLtr == 'O')
            {
               sprintf(stringHold, "Process %d, %s output end\n",
                     nextPCB->PCB_ID, metaDataHold->opName);
               logToLoc(configData, stringHold, True,True);
            }
            setProcessState(configData, nextPCB, READY);
            printPCBStatus(configData,nextPCB );
            break;
         }
         metaDataHold = metaDataHold->next;
      }
}
void runFCFS_P_CPUScheduler(PCB *pcb, ConfigDataType *configData, OpCodeType *metaData)
{
   /*Initiallizes a stringHold array to help store the strings 
     for displaying to log  and monitor*/
   char *stringHold = malloc(sizeof(char) * 256);
   int index;
   /*Initialize Scheduling Queue*/
   for(int index = 0; index < maxQueueSize; index++)
   {
      
      pcbOrderedQueue[index] = NULL;
      pcbBlockQueue[index] = NULL;
      pcbInteruptQueue[index] = NULL;
   }
   // Add the nodes to the scheduling queue
   // Set all PDB to READY state
   PCB *pcbHold = pcb;
   pcbOrderQueueRear = 0;
   while (pcbHold != NULL)
   {
      pcbOrderedQueue[pcbOrderQueueRear] = pcbHold;
      pcbOrderQueueRear++;
      pcbHold = pcbHold->next;
   }
   // initializes the array with blanks
   for (index = 0; index < 256; index++)
   {
      stringHold[index] = ' ';
   }

   // Stamaster loop untill end of all process
   while (IsAllProcessDone(pcb) == False)
   {
      PCB * nextPCB = GetNextProcess(configData, pcb);
      if(nextPCB == NULL){
         if(IsAllBlocked(pcb))
         {
            if(IsSystemCPUIdle()) {
               logToLoc(configData, "OS: System/CPU idle", True, False );
            }
            nextPCB = GetShortestProcessingPCB(configData);
            if(nextPCB != NULL)
            {
               sprintf(stringHold, "OS: Interrupt called by process %d",   nextPCB->PCB_ID);
               logToLoc(configData, stringHold, True, False );
               
               CompleteIOBlockedProcess(configData, nextPCB, metaData);
               //continue;
            }
               
         }
         else 
            nextPCB = GetShortestProcessingPCB(configData);

      }

      if(isCPUIdle == True){
         isCPUIdle = False;
      }
      if(nextPCB == NULL)
        continue;
      OpCodeType *metaDataHold = nextPCB->currentMetaData;
      int runCycle = getTotalCycleTime(configData, metaDataHold);
      if(runCycle == 0){
         nextPCB->state = EXIT;
         sprintf(stringHold, "OS: Process %d selected with %d ms remaining",
            nextPCB->PCB_ID, runCycle);
         logToLoc(configData, stringHold, True, False);
         printPCBStatus(configData,nextPCB );
         continue; 
      }
      state_t ret_state = READY;
      sprintf(stringHold, "OS: Process %d selected with %d ms remaining",
            nextPCB->PCB_ID, runCycle);
      logToLoc(configData, stringHold, True, False);
      setProcessState(configData,nextPCB, RUNNING);
      printPCBStatus(configData,nextPCB );
      while (!(metaDataHold->opLtr == 'A' && compareString(metaDataHold->opName, "end") == STR_EQ))
      {
         if(metaDataHold->opLtr == 'A'){
            metaDataHold = metaDataHold->next;
            continue;
         }
         if(compareString(metaDataHold->opName, "end") == STR_EQ){
            nextPCB->state = EXIT;
            metaDataHold = metaDataHold->next;
            break;   
         }
         //Process when opValue is not zero skip the nodes that has process value is 0 because CPU already processsed
         if(metaDataHold->opValue != 0)
         {
            if (metaDataHold->opLtr == 'I')
            {
               ret_state = ProcessInputOutPut(configData, nextPCB, metaDataHold);
            }
            
            else if (metaDataHold->opLtr == 'O')
            {
               ret_state = ProcessInputOutPut(configData, nextPCB, metaDataHold );
            }

            else if (metaDataHold->opLtr == 'P')
            {
               ret_state = ProcessOperation(configData, nextPCB, metaDataHold);
            }
         }
         if(nextPCB->state == WAITING)
             break;
         // else {
         //    printPCBStatus(configData,nextPCB );      
         //    runCycle = getTotalCycleTime(configData, metaDataHold);
         //    sprintf(stringHold, "OS: Process %d selected with %d ms remaining",
         //          nextPCB->PCB_ID, runCycle);
         //    logToLoc(configData, stringHold, True, False);
         //    setProcessState(configData,nextPCB, RUNNING);
         //    printPCBStatus(configData,nextPCB );
         // }
         metaDataHold = metaDataHold->next;
      }
      printPCBStatus(configData,nextPCB );

   }   
   free(stringHold);
}

void runRR_PCPUScheduler(PCB *pcb, ConfigDataType *configData, OpCodeType *metaData)
{
   /*Initiallizes a stringHold array to help store the strings 
     for displaying to log  and monitor*/
   char *stringHold = malloc(sizeof(char) * 256);
   int index;

   // initializes the array with blanks
   /*for (index = 0; index < 256; index++)
   {
      stringHold[index] = ' ';
   }
   int segmentationFault = 0;

   PCB *pcbHold = pcb;

   //while (pcbHold != NULL)*/
}



//PCB FUNCTIONS////////////////////////////////////////////////////////////////

/*
Function Name: setProcesses
Algorithm: will make a PCB structure like a linked list which will
           include all PCB programs
Precondition: We have the right structs to create PCB
Postcondition: returns head of linked list to create all of the PCB
Exceptions: none
Notes: none
*/
PCB *setProcesses(ConfigDataType *configDataPtr, OpCodeType *metaDataPtr)
{
   // intialize variables
   OpCodeType *OpCodeData;
   PCB *headOfPCB;
   PCB *prevPCBNode;
   int pcbID;

   OpCodeData = metaDataPtr->next;
   headOfPCB = NULL;
   prevPCBNode = NULL;
   pcbID = 0;

   while (OpCodeData != NULL && !(OpCodeData->opLtr == 'S' &&
                                  compareString(OpCodeData->opName, "end") == STR_EQ))
   {
      // Intializing PCB LIST
      PCB *pcbList;

      // Setting Pointers for PCB Linked List
      pcbList = malloc(sizeof(PCB));
      pcbList->PCB_ID = pcbID;
      pcbList->currentMetaData = OpCodeData;
      pcbList->state = NEW;
      pcbList->next = NULL;
      pcbList->totalCycleTime = 0;

      // Checks if previous node exists and sets pointer
      if (prevPCBNode != NULL)
      {
         prevPCBNode->next = pcbList;
      }

      // If it doesn't have a previous node it will be the the head
      else
      {
         headOfPCB = pcbList;
      }

      // Increment the Pcb ID
      pcbID++;

      // Loops till OpCode letter is A and  OpCode Data  ends
      while (!(OpCodeData->opLtr == 'A' &&
               compareString(OpCodeData->opName, "end") == STR_EQ))
      {
         pcbList->totalCycleTime += getOpCodeCycleTime(configDataPtr,
                                                       OpCodeData);
         if ((OpCodeData->opLtr == 'M') && (compareString(OpCodeData->opName, "allocate") == STR_EQ))
         {
            pcbList->allocateMem.base = OpCodeData->opValue / 10000;
            pcbList->allocateMem.offset = OpCodeData->opValue % 10000;
         }

         if ((OpCodeData->opLtr == 'M') && (compareString(OpCodeData->opName, "access") == STR_EQ))
         {
            pcbList->accessMem.base = OpCodeData->opValue / 10000;
            pcbList->accessMem.offset = OpCodeData->opValue % 10000;
         }

         OpCodeData = OpCodeData->next;
      }

      //Sets Pointers once exits loop
      prevPCBNode = pcbList;
      OpCodeData = OpCodeData->next;
   }

   //returns the head of the PCB List
   return headOfPCB;
}

/*
Function Name: initializePCB
Algorithm: Intializes the pcb to the ready state
Precondition: Needs the Data stored in each Process
Postcondition: None
Exceptions: none
Notes: none
*/
void initializePCB(PCB *pcbData)
{
   // Create pointer to hold the PCB Data
   PCB *pcbHold = pcbData;

   // loops till there is no more PCB Data
   while (pcbHold != NULL)
   {
      // Sets state of PCB to Ready State
      pcbHold->state = READY;

      // Sets pointer to hold the next Process
      pcbHold = pcbHold->next;
   }
}

/*
Function Name: getOpCodeCycleTime
Algorithm: get the total time it takes for the opcode to cycle
Precondition: needs the config data and meta data used in the PCB
Postcondition: returns the total cycle time for the config data to cycle
Exceptions: none
Notes: none
*/
int getOpCodeCycleTime(ConfigDataType *configData, OpCodeType *metaData)
{
   // Check for metaData  op code letter
   if (metaData->opLtr == 'P')
   {
      // returns the metaData's Op Value for the linked op code letter
      return metaData->opValue * configData->procCycleRate;
   }

   // Check for metaData  op code letter
   else if (metaData->opLtr == 'I' || metaData->opLtr == 'O')
   {
      // returns the linked Op code value for the the op code letter
      return metaData->opValue * configData->ioCycleRate;
   }

   // Otherwise return stub or 0 because no time or Opcode exists for it
   else
   {
      return 0;
   }
}

/*
Function Name: getTotalCycleTime
Algorithm: get the total time it takes for the configData 
           and metaData to cycle
Precondition: needs the config data and meta data used in the PCB
Postcondition: returns the cycle time
Exceptions: none
Notes: none
*/
int getTotalCycleTime(ConfigDataType *configData, OpCodeType *metaData)
{
   // initiallize the variables and pointers that will be used
   int totalTime = 0;
   OpCodeType *metaDataCycle = metaData;

   // Loops while metaData Cycles pointer is not pointing at Op Code Letter A
   // and while the op Name  is not end for the linked op code letter
   while (!(metaDataCycle->opLtr == 'A' && compareString(metaDataCycle->opName, "end") == STR_EQ))
   {
      // Increment the total time with the opcode cycle time
      totalTime += getOpCodeCycleTime(configData, metaDataCycle);

      // sets ponter to points to the next metadata cycle
      metaDataCycle = metaDataCycle->next;
   }

   return totalTime;
}

/*
Function Name: clearPCB
Algorithm: Clears the pcb to the in order to free any memory allocations
Precondition: Needs to be able to have the the pointers and the list 
              the PBC data is stored in
Postcondition: All memory allocations that deal with PCB's are free
Exceptions: none
Notes: none
*/
void clearPCB(PCB *pcbList)
{
   // Initialize pointers that are storing the
   PCB *pcbMem = pcbList;
   PCB *currentPCBMem = pcbMem;

   // Loops while the the pcbMem exists in the nodes of the PCB list
   while (pcbMem != NULL)
   {
      // sets pointer to point to the memory for the next nodes
      pcbMem = pcbMem->next;

      // checks if current PCB's memory exists
      if (currentPCBMem != NULL)
      {
         // Frees the memory allocation of the the PCB Memory
         free(currentPCBMem);
         currentPCBMem = pcbMem;
      }
   }
}

//LOGTO FUNCTIONS//////////////////////////////////////////////////////////////

/*
Function Name: logToLoc
Algorithm: This will log the PCB List data to a desired location 
           and set the timer with it to where in the list it is 
           called from start to end
Precondition: Needs to be able to have the the pointers and the list of 
              the PCB data is given the option to log to the log file,
              log to the monitor, or log to both
Postcondition: This will return nothing but will take in 
Exceptions: none
Notes: none
*/
void logToLoc(ConfigDataType *configData, char *logText, Boolean printTime, Boolean addNewLineBefore)
{
   //Initializes varibles that will be used
   double currentTime = 0.0;
   int index;

   //
   char *timeStr = (char *)malloc(10);
   for (index = 0; index < 10; index++)
   {
      timeStr[index] = 0;
   }

   //Start the timer for when the system starts so will be zero at beggening
   if (compareString("OS: System Start", logText) == 0)
   {
      currentTime = accessTimer(ZERO_TIMER, timeStr);
   }

   // This will give the time once the Operation stops
   else if (compareString("OS: System stop", logText) == 0)
   {
      currentTime = accessTimer(STOP_TIMER, timeStr);
   }

   // This is when the lap timer works
   else
   {
      currentTime = accessTimer(LAP_TIMER, timeStr);
   }

   /* Here are the switch statements for choosing whether to log it to the
     log file, monitor, or both.*/

   switch (configData->logToCode)
   {
   case LOGTO_FILE_CODE:
      // Only adds comments to log file
      addLogComments(&configData->log, logText, currentTime, printTime, addNewLineBefore);
      break;

   case LOGTO_MONITOR_CODE:
      if (printTime == False)
      {
         // This will log it to the monitor without timer for the process
         printf(logText);
      }

      else
      {
         //This will print it to the monitor with the timer for the process
         if(addNewLineBefore == True)
            printf("\n%f: %s\n", currentTime, logText);
         else 
            printf("%f: %s\n", currentTime, logText);
      }

      break;

   case LOGTO_BOTH_CODE:
      // the addLogComments will print it to the log file
      addLogComments(&configData->log, logText, currentTime, printTime, addNewLineBefore);

      // These will log it to the monitor with or without the timer
      if (printTime == False)
      {
         printf(logText);
      }

      else
      {
         if(addNewLineBefore == True)
            printf("\n%f: %s\n", currentTime, logText);
         else 
            printf("%f: %s\n", currentTime, logText);
//         printf("%f: %s\n", currentTime, logText);
      }

      break;
   }

   // This is where we free or memory allocation
   free(timeStr);
}

/*
Function Name: addLogComments
Algorithm: This method will add the strings to the logfiles
Precondition: Must be able to point to the log file to add the strings
Postcondition:Will return nothing but will help to add the 
              strings to the log file
Exceptions: none
Notes: none
*/
void addLogComments(logData **logPtr, char *comment, double currentTime,
                    Boolean printTime, Boolean addNewLineBefore)
{
   // checks if the log file pointer
   if (*logPtr == NULL)
   {
      // allocates the memory to the logPtr
      *logPtr = (logData *)malloc(sizeof(logData));

      // For the case you don't want the timer to be printed
      if (printTime == False)
      {
         sprintf((*logPtr)->comment, comment);
      }

      // For the case you want the timer on the side
      else
      {
         if(addNewLineBefore == True)
            sprintf((*logPtr)->comment, "\n%f: %s\n", currentTime, comment);
         else
            sprintf((*logPtr)->comment, "%f: %s\n", currentTime, comment);
            
      }

      (*logPtr)->next = NULL;
   }

   // Otherwise recursively calls
   else
   {
      addLogComments(&(*logPtr)->next, comment, currentTime, printTime, addNewLineBefore);
   }
}

// This method is used to clear memory allocation made in log pointers
/*
     We made changes to ou ConfigAccess.h to help with this function.
     We created a logData struct and made a pointer called log in the 
     config data structure.
   */
void clearLogs(ConfigDataType *configData)
{
   logData *logHold = configData->log;

   while (logHold != NULL)
   {
      logData *next = logHold->next;
      free(logHold);
      logHold = next;
   }
}

// This function is a helper method which helps create string to pass in for 
// our MMU opVal
void getFormatedStrWithSlash(char *deststr, int chosenInteger)
{
   char revertedstr[32];
   int length = 0; 
   //Checks if opVal for MMU is greater than 0
   while ( chosenInteger > 0 )
   {
      int begginingInt = chosenInteger % 10;
      revertedstr[length++] = begginingInt | '0';
      chosenInteger /= 10;
   }
   // as long as opVal is less than 8
   if ( length < 8 )
   {
      revertedstr[length++] = '0';
   }

   length--;
   int rev = 0;
   while (length >= 0)
   {
      if (length == 3)
      {
         deststr[rev++] = '/';
      }

      deststr[rev++] = revertedstr[length--];
   }

   deststr[rev] = 0;
}

// Runs method for CPU Scheduling This is basically the code for First come first serve
/* Although for SJF it runned after all the PCB's are bubble sorted
so that it will just call them in order after it has been sorted.*/
void runCPUScheduler(PCB *pcb, ConfigDataType *configData, OpCodeType *metaData)
{
   /*Initiallizes a stringHold array to help store the strings 
     for displaying to log  and monitor*/
   char *stringHold = malloc(sizeof(char) * 256);
   int index;

   // initializes the array with blanks
   for (index = 0; index < 256; index++)
   {
      stringHold[index] = ' ';
   }
   int segmentationFault = 0;

   PCB *pcbHold = pcb;

   while (pcbHold != NULL)
   {
      OpCodeType *metaDataHold = pcbHold->currentMetaData;

      int runCycle = getTotalCycleTime(configData, metaDataHold);

      sprintf(stringHold, "OS: Process %d selected with %d ms remaining",
              pcbHold->PCB_ID, runCycle);

      logToLoc(configData, stringHold, True, False);

      pcb->state = READY;

      sprintf(stringHold, "OS: Process %d set in RUNNING state",
              pcbHold->PCB_ID);

      logToLoc(configData, stringHold, True, True);

      segmentationFault = 0;

      // main while loop to run through each thread and generate logs
      while (!(metaDataHold->opLtr == 'A' && compareString(metaDataHold->opName, "end") == STR_EQ))
      {
         int opCodeRunningTime =
             getOpCodeCycleTime(configData, metaDataHold);

         if (metaDataHold->opLtr == 'I')
         {
            sprintf(stringHold, "Process %d, %s input start",
                    pcbHold->PCB_ID, metaDataHold->opName);

            logToLoc(configData, stringHold, True, True);
         }
         
         else if (metaDataHold->opLtr == 'O')
         {
            sprintf(stringHold, "Process %d, %s output start",
                    pcbHold->PCB_ID, metaDataHold->opName);

            logToLoc(configData, stringHold, True, False);
         }

         else if (metaDataHold->opLtr == 'P')
         {
            sprintf(stringHold, "Process %d, %s operation start",
                    pcbHold->PCB_ID, metaDataHold->opName);

            logToLoc(configData, stringHold, True, False);
         }
         else if (metaDataHold->opLtr == 'M')
         {
            char stropValue[10];
            getFormatedStrWithSlash(stropValue, metaDataHold->opValue);
            if (compareString(metaDataHold->opName, "access") == STR_EQ)
            {
               sprintf(stringHold, "Process %d, MMU attempt to access %s",
                       pcbHold->PCB_ID, stropValue);
               logToLoc(configData, stringHold, True, False);
            }

            else if (compareString(metaDataHold->opName, "allocate") == STR_EQ)
            {
               sprintf(stringHold, "Process %d, MMU attempt to allocate %s",
                       pcbHold->PCB_ID, stropValue);
               logToLoc(configData, stringHold, True, False);
               //PCB fails if Total Memory required is greater than total available memory
               if ((pcbHold->allocateMem.base > configData->memAvailable) ||
                   (pcbHold->allocateMem.base + pcbHold->allocateMem.offset) > configData->memAvailable)
               {
                  sprintf(stringHold, "Process %d, MMU failed to allocate", pcbHold->PCB_ID);
                  segmentationFault = 1;
                  logToLoc(configData, stringHold, True, False);
                  break;
               }

               else
               {
                  sprintf(stringHold, "Process %d, MMU successful allocate",
                          pcbHold->PCB_ID);
               }
               logToLoc(configData, stringHold, True, False);
            }
         }

         if (segmentationFault == 0)
         {
            dealingWithThreads(configData, stringHold, opCodeRunningTime);

            // print statements for output end and input ends
            if (metaDataHold->opLtr == 'I')
            {
               sprintf(stringHold, "Process %d, %s input end",
                       pcbHold->PCB_ID, metaDataHold->opName);

               logToLoc(configData, stringHold, True, False);
            }

            else if (metaDataHold->opLtr == 'O')
            {
               sprintf(stringHold, "Process %d, %s output end",
                       pcbHold->PCB_ID, metaDataHold->opName);

               logToLoc(configData, stringHold, True, False);
            }

            else if (metaDataHold->opLtr == 'P')
            {
               sprintf(stringHold, "Process %d, %s operation end",
                       pcbHold->PCB_ID, metaDataHold->opName);

               logToLoc(configData, stringHold, True, False);
            }
            else if (metaDataHold->opLtr == 'M')
            {
               if (compareString(metaDataHold->opName, "access") == STR_EQ)
               {
                  //PCB fails if Access Memory is greater than allocated memory
                  if ((pcbHold->accessMem.base > configData->memAvailable) ||
                      (pcbHold->accessMem.base < pcbHold->allocateMem.base) ||
                      ((pcbHold->accessMem.base + pcbHold->accessMem.offset) > (pcbHold->allocateMem.base + pcbHold->allocateMem.offset)))
                  {
                     sprintf(stringHold, "Process %d, MMU failed to access", pcbHold->PCB_ID);
                     segmentationFault = 1;
                     logToLoc(configData, stringHold, True, False);
                     break;
                  }
                  else
                  {
                     sprintf(stringHold, "Process %d, MMU successful access", pcbHold->PCB_ID);
                  }
                  logToLoc(configData, stringHold, True, False);
               }
            }
         }
         metaDataHold = metaDataHold->next;
      }

      logToLoc(configData, "\n", False, False);

      if (segmentationFault == 1)
      {
         sprintf(stringHold, "Process %d, experiences segmentation fault", pcbHold->PCB_ID);
         logToLoc(configData, stringHold, True, False);
      }
      pcbHold->state = EXIT;

      sprintf(stringHold, "Process %d ended and set in EXIT state",
              pcbHold->PCB_ID);

      logToLoc(configData, stringHold, True, False);

      pcbHold = pcbHold->next;
   }

   free(stringHold);
}

// Does all the Pthreading for us to display it to the monitor and log files
/*
  Also we made a changes in the simtimer file given by lev. We changed the 
  the the runTimerFunction to be able to run as a pointer. Now we Call the 
  function runTimerThreadFunction. Also we changed the parameter for 
  timeToString at the end of the simtimer file due to testing purposes.
*/
void dealingWithThreads(ConfigDataType *configData, char *logStr, int runTime)
{
   pthread_t pthread_monitor;
   pthread_create(&pthread_monitor, NULL, runTimer, &runTime);
   pthread_join(pthread_monitor, NULL);
}
void dealingWithThreadsWithoutJoin(ConfigDataType *configData, char *logStr, int runTime)
{
   pthread_t pthread_monitor;
   pthread_create(&pthread_monitor, NULL, runTimer, &runTime);
  
}

