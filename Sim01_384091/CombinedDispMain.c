// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"

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
    ConfigDataType *configDataPtr;
    OpCodeType *mdData;
    
    // display component title
       // function: printf
    printf( "\nConfig File Upload Program\n" );
    printf( "==========================\n\n" );
    
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
        displayConfigData( configDataPtr );
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
    printf( "\nMeta Data File Upload Program\n" );
    printf( "=============================\n\n" );
    
       
    // get data from meta data file
       // function: copyString, getOpCodes
    copyString( mdFileName, configDataPtr->metaDataFileName );
    mdAccessResult = getOpCodes( mdFileName, &mdData);
    
    // check for successful upload
    if( mdAccessResult == NO_ERR )
       {
        // display meta data file
           // function: displayMetaData
        displayMetaData( mdData );
       }
       
    // otherwise, assume unsuccessful upload
    else
      {
       // display meta data error message
          // function: displayMetaDataError
       displayMetaDataError( mdAccessResult );
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
    
