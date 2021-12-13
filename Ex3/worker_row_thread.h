//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#ifndef WORKER_ROW_THREAD_H
#define WORKER_ROW_THREAD_H

#include <stddef.h>
#include <stdbool.h>
#include <Windows.h>

#include "worker_row_thread.h"
#include "HardCodedData.h"


typedef struct Pages {

	size_t frame_num;
	bool valid;
	size_t end_time;

} Page;


//A type for thread return codes
typedef enum
{
	SCHOOL_THREAD__CODE_SUCCESS,
	SCHOOL_THREAD__CODE_NULL_PTR,
} SCHOOL_THREAD__return_code_t;


// A type for thread parameters
typedef struct
{
	int* current_time;
	int parsed_row_array[NUM_OF_ROW_VARIABLES];
	Page* page_table;
	
} ROW_THREAD_params_t;





Page* create_and_init_page_table(size_t num_of_pages);

DWORD WINAPI worker_row_thread(LPVOID lpParam);


#endif
