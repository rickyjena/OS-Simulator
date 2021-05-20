// header files

  //Library Files Used
#include <pthread.h>

  //Header Files Linked
#include "BatchProgramSim.h"
#include "simtimer.h"

//PCB FUNCTIONS////////////////////////////////////////////////////////////////

/*
Function Name: createPCB
Algorithm: will make a PCB structure like a linked list which will
           include all PCB programs
Precondition: We have the right structs to create PCB
Postcondition: returns head of linked list to create all of the PCB
Exceptions: none
Notes: none
*/
PCB* createPCB(ConfigDataType* configDataPtr, OpCodeType* metaDataPtr)
   {
    // intialize variables
    OpCodeType* OpCodeData;
    PCB* headOfPCB;
    PCB* prevPCBNode;
    int pcbID;
    
    OpCodeData = metaDataPtr->next;
    headOfPCB = NULL;
    prevPCBNode = NULL;
    pcbID = 0;
    
    while( OpCodeData != NULL && !( OpCodeData -> opLtr == 'S' && 
           compareString( OpCodeData -> opName, "end" ) == STR_EQ ) )
       {
        // Intializing PCB LIST
        PCB* pcbList;
        
        // Setting Pointers for PCB Linked List
        pcbList = malloc( sizeof( PCB ) );
        pcbList -> PCB_ID = pcbID;
        pcbList -> currentMetaData = OpCodeData;
        pcbList -> state = NEW;
        pcbList -> mmu = NULL;
        pcbList -> next = NULL;
        
        // Checks if previous node exists and sets pointer
        if( prevPCBNode != NULL )
           {
            prevPCBNode -> next = pcbList;
           }
        
        // If it doesn't have a previous node it will be the the head
        else
           {
            headOfPCB = pcbList;
           }
        
        // Increment the Pcb ID
        pcbID++;
        
        // Loops till OpCode letter is A and  OpCode Data  ends
        while( !( OpCodeData -> opLtr == 'A' && 
              compareString(OpCodeData -> opName, "end" ) == STR_EQ ) )
           {
            pcbList->totalCycleTime += getOpCodeCycleTime(configDataPtr,
                                                         OpCodeData);
            OpCodeData = OpCodeData -> next;
           }
        
        //Sets Pointers once exits loop
        prevPCBNode= pcbList;
        OpCodeData = OpCodeData -> next;
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
void initializePCB( PCB* pcbData )
   {
    // Create pointer to hold the PCB Data
    PCB* pcbHold = pcbData;
    
    // loops till there is no more PCB Data
    while( pcbHold != NULL )
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
int getOpCodeCycleTime( ConfigDataType* configData, OpCodeType* metaData )
   {
    // Check for metaData  op code letter
    if( metaData->opLtr == 'P' )
       {
        // returns the metaData's Op Value for the linked op code letter
        return metaData->opValue * configData -> procCycleRate;
       }
    
    // Check for metaData  op code letter
    else if( metaData->opLtr == 'I' || metaData->opLtr == 'O' )
       {
        // returns the linked Op code value for the the op code letter
        return metaData->opValue * configData -> ioCycleRate;
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
int getTotalCycleTime(ConfigDataType* configData, OpCodeType* metaData)
   {
    // initiallize the variables and pointers that will be used
    int totalTime = 0;
    OpCodeType* metaDataCycle = metaData;
    
    // Loops while metaData Cycles pointer is not pointing at Op Code Letter A
    // and while the op Name  is not end for the linked op code letter
    while( !( metaDataCycle -> opLtr == 'A' 
          && compareString( metaDataCycle -> opName, "end" ) == STR_EQ ) )
       {
        // Increment the total time with the opcode cycle time
        totalTime += getOpCodeCycleTime( configData, metaDataCycle );
        
        // sets ponter to points to the next metadata cycle
        metaDataCycle = metaDataCycle -> next;
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
void clearPCB( PCB* pcbList )
   {
    // Initialize pointers that are storing the 
    PCB* pcbMem = pcbList;
    PCB* currentPCBMem = pcbMem;
    
    // Loops while the the pcbMem exists in the nodes of the PCB list
    while( pcbMem != NULL )
    {
        // sets pointer to point to the memory for the next nodes
        pcbMem = pcbMem->next;
        
        // checks if current PCB's memory exists
        if(currentPCBMem != NULL) 
        {
           // Frees the memory allocation of the the PCB Memory
           free( currentPCBMem );
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
void logToLoc( ConfigDataType* configData, char* logText, Boolean printTime )
    {
     //Initializes varibles that will be used
     double currentTime = 0.0;
     int index;
     
     //
     char * timeStr = (char *)malloc(10);
     for( index = 0; index < 10; index++)
     {
     	timeStr[ index ] = ' ';
     }
     
     //Start the timer for when the system starts so will be zero at beggening
     if(compareString("OS: System Start",logText) == 0)
     {
       currentTime = accessTimer( ZERO_TIMER, timeStr );
     }
     
     // This will give the time once the Operation stops
     else if(compareString("OS: System stop",logText) == 0)
     {
       currentTime = accessTimer( STOP_TIMER, timeStr );
     }
     
     // This is when the lap timer works 
     else 
     {
       currentTime = accessTimer( LAP_TIMER, timeStr );
     }
     
     /* Here are the switch statements for choosing whether to log it to the
     log file, monitor, or both.*/
     
     switch(configData->logToCode)
       {
        case LOGTO_FILE_CODE:
            // Only adds comments to log file
            addLogComments(&configData->log, logText, currentTime,printTime);
            break;

        case LOGTO_MONITOR_CODE:
            if( printTime==False)
               {
               // This will log it to the monitor without timer for the process
                printf(logText);
               }
            
            else
               {
             //This will print it to the monitor with the timer for the process
                printf( "%f: %s\n",currentTime,  logText );
               }
            
            break;

        case LOGTO_BOTH_CODE:
            // the addLogComments will print it to the log file
            addLogComments(&configData->log, logText, currentTime,printTime);
            
            // These will log it to the monitor with or without the timer
            if( printTime==False )
               {
                printf( logText );
               }
            
            else
               {
                printf("%f: %s\n",currentTime,  logText);
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
void addLogComments(logData** logPtr, char* comment, double currentTime, 
                Boolean printTime)
   {
    // checks if the log file pointer 
    if( *logPtr == NULL )
       {
        // allocates the memory to the logPtr
        *logPtr = ( logData * ) malloc( sizeof( logData ) );
        
        // For the case you don't want the timer to be printed
        if( printTime==False )
         {
          sprintf((*logPtr)->comment, comment);
         }
        
        // For the case you want the timer on the side
        else
         {
          sprintf( ( *logPtr )->comment, "%f: %s\n", currentTime, comment );
         }
         
         (*logPtr)->next = NULL;
       }
    
    // Otherwise recursively calls
    else
       {
        addLogComments( &( *logPtr )->next, comment, currentTime, printTime );
       }
    
   }
// This method is used to clear memory allocation made in log pointers
   /*
     We made changes to ou ConfigAccess.h to help with this function.
     We created a logData struct and made a pointer called log in the 
     config data structure.
   */
void clearLogs(ConfigDataType* configData)
   {
    logData* logHold = configData->log;

    while(logHold != NULL)
    {
        logData* next = logHold->next;
        free(logHold);
        logHold = next;
    }

   }

// Runs all the methods needed for simulation scheduler
void runSimScheduler(ConfigDataType* configData, OpCodeType* metaData)
   {
    logToLoc(configData, "OS: System Start", True);

    logToLoc(configData, "OS: Create Process Control Blocks", True);
    
    PCB* pcbLink = createPCB(configData, metaData);
    
    logToLoc(configData, "OS: All processes initialized in New state", True);
    
    initializePCB(pcbLink);
    
    logToLoc(configData, "OS: All processes now set in Ready state", True);
    
    runFCFSScheduler(pcbLink, configData, metaData);
    
    logToLoc(configData, "OS: System stop", True);

    clearPCB(pcbLink);
   }

// Runs method for First Come First Serve Scheduling
void runFCFSScheduler(PCB* pcb, ConfigDataType* configData, OpCodeType* metaData)
   {
    /*Initiallizes a stringHold array to help store the strings 
     for displaying to log  and monitor*/
    char* stringHold = malloc(sizeof(char)*256);;
    int index;
    
    // initializes the array with blanks
    for( index = 0; index < 256; index++ )
     {
      stringHold[ index ] = ' ';
     }
    
    PCB* pcbHold= pcb;
    
    while(pcbHold != NULL)
       {
        OpCodeType* metaDataHold = pcbHold->currentMetaData;

        int runCyle = getTotalCycleTime(configData, metaDataHold);
        
        sprintf(stringHold, "OS: Process %d selected with %d ms remaining", 
                pcbHold->PCB_ID, runCyle);
        
        logToLoc(configData, stringHold, True);
        
        pcb->state = READY;
        
        sprintf(stringHold, "OS: Process %d set in RUNNING state\n", 
                pcbHold->PCB_ID);
        
        logToLoc(configData, stringHold, True);

        // main while loop to run through each thread and generate logs
        while( !( metaDataHold->opLtr == 'A' 
              && compareString(metaDataHold->opName, "end" ) == STR_EQ ) )
           {
            int opCodeRunningTime = 
                 getOpCodeCycleTime( configData, metaDataHold );
            
            if( metaDataHold->opLtr == 'I' )
               {
                sprintf(stringHold, "Process %d, %s input start",
                         pcbHold->PCB_ID, metaDataHold->opName);
                
                logToLoc(configData, stringHold, True);
               }
            
            else if(metaDataHold->opLtr == 'O')
               {
                sprintf(stringHold, "Process %d, %s output start",
                         pcbHold->PCB_ID, metaDataHold->opName);
                
                logToLoc(configData, stringHold, True);
               }
            
            else if(metaDataHold->opLtr == 'P')
               {
                sprintf(stringHold, "Process %d, %s operation start",
                         pcbHold->PCB_ID, metaDataHold->opName);
                
                logToLoc(configData, stringHold, True);
               }
            
            dealingWithThreads(configData, stringHold, opCodeRunningTime);
            
            // print statements for output end and input ends
            if( metaDataHold->opLtr == 'I' )
               {
                sprintf(stringHold, "Process %d, %s input end",
                         pcbHold->PCB_ID, metaDataHold->opName);
                
                logToLoc(configData, stringHold, True);
               }
            
            else if( metaDataHold->opLtr == 'O' )
               {
                sprintf( stringHold, "Process %d, %s output end",
                         pcbHold->PCB_ID, metaDataHold->opName );
                
                logToLoc(configData, stringHold, True);
               }
            
            else if(metaDataHold->opLtr == 'P')
               {
                sprintf(stringHold, "Process %d, %s operation end", 
                         pcbHold->PCB_ID, metaDataHold->opName);
                
                logToLoc(configData, stringHold, True);
               }
            
            metaDataHold = metaDataHold->next;
           }
        
        pcbHold->state = EXIT;
        
        logToLoc(configData, "\n", False);
        
        sprintf( stringHold, "Process %d ended and set in EXIT state", 
                 pcbHold->PCB_ID );
        
        logToLoc(configData, stringHold, True);
        
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
void dealingWithThreads(ConfigDataType* configData, char* logStr, int runTime)
   {
    pthread_t pthread_monitor;

    pthread_create(&pthread_monitor, NULL, runTimerThreadFunction, &runTime);

    pthread_join(pthread_monitor, NULL);
   }
