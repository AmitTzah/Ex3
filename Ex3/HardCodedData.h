//This header file holds different macros and structures used in the project.

#ifndef HARD_CODED_DATA_H
#define HARD_CODED_DATA_H

//general constants
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define SUCCESS_CODE ((int)(0))
#define MAX_LENGTH_OF_PATH_TO_A_FILE 100




//school_thread related constants

#define NUM_OF_VALS_TO_PASS_TO_THREAD 5
#define NUMBER_OF_MAX_SIMULTANEOUSLY_WORKING_THREADS 10

#define ENG_FILE_PATH "./Eng/"
#define REAL_FILE_PATH "./Real/"
#define HUMAN_FILE_PATH "./Human/"
#define EVAL_FILE_PATH "./Eval/"
#define RESULTS_FILE_PATH "./Results/"

#define MAX_GRADE_CHARS 5 
#define NUM_OF_GRADE_COMPONENTS 4


#endif