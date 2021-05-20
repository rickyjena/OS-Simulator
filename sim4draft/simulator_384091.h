#ifndef SIMULATOR_H
#define SIMULATOR_H

// header files
#include <stdio.h>
#include "MetaDataAccess.h"
#include "ConfigAccess.h"

// Memory Management Unit Data Structure
typedef struct _MMU{
    int base;
    int offset;
} MMU;
typedef enum  {NEW, READY, WAITING, RUNNING, EXIT} state_t;
// Process Control Block Data Structure
typedef struct _PCB{
    int totalCycleTime;
    int PCB_ID;
    MMU allocateMem;
    MMU accessMem;
    state_t state;
    OpCodeType *currentMetaData;
    struct _PCB* next;
} PCB;

// All Function Prototypes For Sim 2//////////////////////////////////////////

//PCBFunctions
PCB* setProcesses(ConfigDataType* configDataPtr, OpCodeType* metaDataPtr);
void initializePCB( PCB* pcbData );
int getOpCodeCycleTime(ConfigDataType* configData, OpCodeType* metaData);
int getTotalCycleTime(ConfigDataType* configData, OpCodeType* metaData);
void clearPCB(PCB* pcbList);

PCB * GetPCBFromBlockQueue();
Boolean AddToBlockQueue(PCB * blockPCB);

PCB * GetFromInterupQueue();
Boolean AddToInteruptQueue(PCB * schPCB);
PCB * GetNextProcess(ConfigDataType *configData, PCB * headPCB);
PCB * GetNextProcess_READY(ConfigDataType *configData, PCB * headPCB);
void printPCBStatus(ConfigDataType *configData,PCB * pcb );
//LogFunctions
void logToLoc(ConfigDataType* configData, char* message, Boolean printTime, Boolean addNewLineBefore);
void addLogComments( logData** logPtr, char* comment, double currentTime, 
                     Boolean printTime, Boolean addNewLineBefore );
void clearLogs(ConfigDataType* configData);

//Scheduler and Threading Functions
void runSimulator(ConfigDataType* configData, OpCodeType* metaData);
void runCPUScheduler(PCB* pcb, ConfigDataType* configData, 
                      OpCodeType* metaData);
void dealingWithThreads(ConfigDataType* configData, char* logStr, int runTime);
void getFormatedStrWithSlash(char *deststr, int chosenInteger);

#endif
