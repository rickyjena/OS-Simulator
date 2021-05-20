#ifndef SIM02_BATCH_H
#define SIM02_BATCH_H

// header files
#include <stdio.h>
#include "MetaDataAccess.h"
#include "ConfigAccess.h"

// Memory Management Unit Data Structure
typedef struct _MMU{
    int id;
    int base;
    int offset;
    struct _MMU* next;
} MMU;

// Process Control Block Data Structure
typedef struct _PCB{
    int totalCycleTime;
    int PCB_ID;
    enum state {NEW, READY, RUNNING, EXIT} state;
    OpCodeType *currentMetaData;
    MMU *mmu;
    struct _PCB* next;
} PCB;

// All Function Prototypes For Sim 2//////////////////////////////////////////

//PCBFunctions
PCB* createPCB(ConfigDataType* configDataPtr, OpCodeType* metaDataPtr);
void initializePCB( PCB* pcbData );
int getOpCodeCycleTime(ConfigDataType* configData, OpCodeType* metaData);
int getTotalCycleTime(ConfigDataType* configData, OpCodeType* metaData);
void clearPCB(PCB* pcbList);

//LogFunctions
void logToLoc(ConfigDataType* configData, char* message, Boolean printTime);
void addLogComments( logData** logPtr, char* comment, double currentTime, 
                     Boolean printTime );
void clearLogs(ConfigDataType* configData);

//Scheduler and Threading Functions
void runSimScheduler(ConfigDataType* configData, OpCodeType* metaData);
void runFCFSScheduler(PCB* pcb, ConfigDataType* configData, 
                      OpCodeType* metaData);
void dealingWithThreads(ConfigDataType* configData, char* logStr, int runTime);

#endif
