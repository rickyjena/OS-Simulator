// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "simulator_384091.h"

/*
Function Name: main
Algorithm: driver function to test config and metadata file 
           upload operation together
Precondition: none
Postcondition: returns zero (0) on success
Exceptions: none
Notes: demonstrates development and use of metadata file upload function
*/
int main( int argc, char **argv )
   {
    // initialize function/variables
    int configAccessResult, mdAccessResult;
    char configFileName[ MAX_STR_LEN ];
    char mdFileName[ MAX_STR_LEN ];
    char displayString[ STD_STR_LEN ];
    ConfigDataType *configDataPtr;
    OpCodeType *mdData;
    
    // display component title
       // function: printf
    /*printf( "\nConfig File Upload Program\n" );
    printf( "==========================\n\n" );*/

    // Print beginning of Simulator
    printf( "\nSimulator Program\n" );
    printf( "=================\n\n" );
    
    // check for not correct number of command line arguements (two)
    if( argc < 2 )
       {
        // print missing comand line arguement error
           // function: printf
        printf( "ERROR: Program requires file name for config file" );
        printf( "as command line argument\n" );
        printf( "Program Terminated\n" );
        
        // return non-normal program result
        return 1;
       }
       
    // get data from configuration file
       // function: copyString, getConfigData
    copyString( configFileName, argv[ 1 ] );
    configAccessResult = getConfigData( configFileName, &configDataPtr );
    
    // check for succesful upload
    if( configAccessResult == NO_ERR )
       {
        // display config file
           // function: displayConfigData
        // displayConfigData( configDataPtr );
        printf( "Uploading Configuration Files\n" );
       }
       
    // otherwise, assume failed upload 
    else
       {
        // display configuration upload error
           // function: displayConfigError
        displayConfigError( configAccessResult );
       }

    // display title
       // function: printf
    /*printf( "\nMeta Data File Upload Program\n" );
    printf( "=============================\n\n" );*/
    
    //This to make sure if the meta data files empty it gives the
        //the right error. Also this fixed the segfault error
    if( configDataPtr == NULL )
       {
        displayMetaDataError( MD_FILE_ACCESS_ERR );
       }
       
    else
       {
        // get data from meta data file
           // function: copyString, getOpCodes
        copyString( mdFileName, configDataPtr->metaDataFileName );
        mdAccessResult = getOpCodes( mdFileName, &mdData);
        
        // check for successful upload
        if( mdAccessResult == NO_ERR )
           {
            printf( "\nUploading Meta Data Files\n" );
           }
               
        // otherwise, assume unsuccessful upload
        else
        {
         // display meta data error message
            // function: displayMetaDataError
         displayMetaDataError( mdAccessResult );
        }
        
        // We Will print Begin Simulation for Sim02
        if(configDataPtr->logToCode == LOGTO_BOTH_CODE 
           || configDataPtr->logToCode == LOGTO_MONITOR_CODE)
        {
         logToLoc(configDataPtr,"Begin Simulation\n", False,True);
         logToLoc(configDataPtr,"================\n\n", False, False);
        }
        
        // runs the schedule for PCB, and logging to files and monitor
        runSimulator(configDataPtr, mdData);
        
        // Will Print Once Simulaton is complete for Sim02
        if(configDataPtr->logToCode == LOGTO_BOTH_CODE 
           || configDataPtr->logToCode == LOGTO_MONITOR_CODE)
        {
         logToLoc(configDataPtr,"End Simulation - Complete\n", False,True);
	      logToLoc(configDataPtr,"=========================\n", False,False);
        }
    
        // checks if it wants to log to file or cmd and file and then writes file
        if( configDataPtr->logToCode == LOGTO_FILE_CODE || 
              configDataPtr->logToCode == LOGTO_BOTH_CODE )
        {
         // pointer for processes going to be written into log
         logData* logHold = configDataPtr->log;
         
         // opens log file to write into
         FILE* logFile = fopen(configDataPtr->logToFileName, "w+");
         
         //Printing Config Data Info At Top of Log File
         fprintf(logFile, "%s"
                 ,"\n==================================================");
         fprintf(logFile, "%s","\nSimulator Log File Header\n\n");
         
         fprintf(logFile, "File Name                       : %s\n",
                 configDataPtr->metaDataFileName );
         
         configCodeToString( configDataPtr->cpuSchedCode, displayString );
         
         fprintf(logFile, "CPU Scheduling                  : %s\n",
                 displayString);
         
         fprintf(logFile, "Quantum Cycles                  : %d\n",
                 configDataPtr->quantumCycles);
         
         fprintf(logFile, "Memory Available (KB)           : %ld\n",
                 configDataPtr->memAvailable );
         
         fprintf(logFile, "Processor Cycle Rate (ms/cycle) : %d\n",
                 configDataPtr->procCycleRate);
         
         fprintf(logFile,"I/O Cycle Rate (ms/cycle)       : %d\n",
                 configDataPtr->ioCycleRate );
         
         // loops while logHold pointer exists and has processes
         while(logHold != NULL)
            {
             // writes processes into log file
             fprintf(logFile, "%s",logHold->comment);
         
             logHold = logHold->next;
            }
         
         // sets log hold to the configDataPtr
         logHold = configDataPtr->log;
         
         // makes new pointer to keep track of whats in loghold currently
         logData* curlogHold = logHold;
         
         // loops while logHold pointer still has memory and frees all of it
         while(logHold != NULL)
            {
             logHold = logHold->next;
             
             if(curlogHold != NULL)
               {
 		          free(curlogHold);
                
 		          curlogHold = logHold;
               }
               
            }
         
         // closes the log file being written into
         fclose(logFile);
        }
         // shut down, clean up program
         
         // clear meta data
            // function: clearMetaDataList
         mdData = clearMetaDataList( mdData );
       
         // add endline for vertical spacing
            // function: printf
         printf( "\n" );
         
         // cleared last config pointer to pass valgrind test
         clearConfigData( &configDataPtr );
         
         // return success
         return 0;
       }
   
   }