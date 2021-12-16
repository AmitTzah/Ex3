//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#ifndef WORKER_ROW_THREAD_H
#define WORKER_ROW_THREAD_H

#include <stddef.h>
#include <stdbool.h>
#include <Windows.h>


#include "HardCodedData.h"
#include "create_and_handle_processes.h"


typedef struct Pages {

	unsigned int frame_num;
	bool valid;
	unsigned int end_time;

} Page;


//A type for thread return codes
typedef enum
{
	ROW_THREAD__CODE_SUCCESS,
	ROW_THREAD__CODE_NULL_PTR,
} ROW_THREAD__return_code_t;


// A type for thread parameters
typedef struct
{
	unsigned int row_index;
	unsigned int* current_time;
	int* parsed_row_array;
	Page* page_table;
	HANDLE semaphore;
	ReadersWritersParam * page_table_readers_writers_parmas;
	ReadersWritersParam * clock_readers_writers_parmas;
	unsigned int size_of_page_table;
	unsigned int  num_of_frames;
	char* path_to_output;

} ROW_THREAD_params_t;


Page* create_and_init_page_table(unsigned int num_of_pages);

DWORD WINAPI worker_row_thread(LPVOID lpParam);

unsigned int read_current_time_protected(ReadersWritersParam* clock_readers_writers_parmas, unsigned int* current_time);
void write_to_current_time_protected(int updated_time, ReadersWritersParam *clock_readers_writers_parmas, int* current_time);

Page read_page_table_protected(Page* page_table, ReadersWritersParam *page_table_readers_writers_parmas, int index_of_page_to_access);
void write_to_page_table_protected(Page* page_table, ReadersWritersParam* page_table_readers_writers_parmas, int index_of_page_to_access, Page new_page_to_write);

void print_left_over_evictions(Page* page_table, unsigned int num_of_pages, char* path_to_output);
#endif
