//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "worker_row_thread.h"


DWORD WINAPI worker_row_thread(LPVOID lpParam) {

	ROW_THREAD_params_t* p_params;
	
	/* Check if lpParam is NULL */
	if (NULL == lpParam)
	{
		return ROW_THREAD__CODE_NULL_PTR;
	}

	//Convert (void *) to parameters type.
	p_params = (ROW_THREAD_params_t*)lpParam;

	int time = p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 3];
	int page_index = floor((p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 2]) / SIZE_OF_PAGE);
	int time_of_use = p_params->parsed_row_array[NUM_OF_ROW_VARIABLES-1];
	int* current_time = p_params->current_time;
	HANDLE semaphore = p_params->semaphore;
	Page* page_table=p_params->page_table;

	Page page_read = read_page_table_protected(page_table, (p_params->page_table_readers_writers_parmas), page_index);

	
	if (page_read.valid == false) {
		int index_of_free_frame=0;
		int index_of_page_where_end_time_has_passed=0;

		//int frame=iterate_over_page_table_and_search_for_avaliable_frame(p_params, &index_of_free_frame, &index_of_page_where_end_time_has_passed);

		//check if free frame was not found
		if (index_of_free_frame == -1 && index_of_page_where_end_time_has_passed == -1) {

			//signal to_main that goes to waiting mode, so that main could continue.
			ReleaseSemaphore(semaphore, 1, NULL);

			//waiting_mode()

		}

		else {

			//found a frame that can be evicted then used
			if (index_of_page_where_end_time_has_passed != -1) {

				//print_eviction_line_to_output(index_of_page_where_end_time_has_passed, page table)
				// print placement line to output(frame, p_params)
				//update page table(p_params)
			
			}

			else {

			// print placement line to output(frame, p_params)
			//update page table(p_params)

			}
		}


	}

	//this page already has a frame
	else {
		
		//update end time of table if needed.
	}


	printf("Finished working on a row number %d ", p_params->row_index);

	//signaled to_main that finished work, so that main could continue.
	ReleaseSemaphore(semaphore, 1, NULL);
	return ROW_THREAD__CODE_SUCCESS;
}


Page* create_and_init_page_table(size_t num_of_pages) {

	Page* page_table= calloc(num_of_pages, sizeof(Page));


	if (page_table == NULL) {
		printf("Memory allocation to page_table array failed!");
		exit(1);
	}
	else {
		for (size_t i = 0; i < num_of_pages; i++)
		{
			page_table[i].valid = false;

		}
	}
	return page_table;
}





/*
waiting_mode(&current_time, thread, page_table, output_file) {

	//contastnly reading page_table to find a frame where end_time<=current_time
	//when finds such frame, it prints evict as needed and update the page table, then print placement.
	//should be sempahore protected as the read/write problem code.



}
*/







int read_current_time_protected(ReadersWritersParam* clock_readers_writers_parmas, int* current_time){
	int current_t;
	

	//need to add function to check failure and exit correctly

	WaitForSingleObject(clock_readers_writers_parmas->turn_slide_mutex, INFINITE);
	ReleaseMutex(clock_readers_writers_parmas->turn_slide_mutex);

	WaitForSingleObject(clock_readers_writers_parmas->mutex, INFINITE);


	clock_readers_writers_parmas->readers += 1;
	if (clock_readers_writers_parmas->readers == 1) {
		WaitForSingleObject(clock_readers_writers_parmas->room_empty_semaphore, INFINITE);

	}
	ReleaseMutex(clock_readers_writers_parmas->mutex);

	//critical section for readers
	
	current_t= *current_time;

	//end ofcritical section for readers

	WaitForSingleObject(clock_readers_writers_parmas->mutex, INFINITE);
	clock_readers_writers_parmas->readers -= 1;

	if (clock_readers_writers_parmas->readers == 0) {

		ReleaseSemaphore(clock_readers_writers_parmas->room_empty_semaphore, 1, NULL);

	}


	ReleaseMutex(clock_readers_writers_parmas->mutex);

	return current_t;
}

void write_to_current_time_protected(int updated_time, ReadersWritersParam* clock_readers_writers_parmas, int* current_time) {

	WaitForSingleObject(clock_readers_writers_parmas->turn_slide_mutex, INFINITE);
	WaitForSingleObject(clock_readers_writers_parmas->room_empty_semaphore, INFINITE);

	//critical section for writers

	*current_time = updated_time;

	//end ofcritical section for writers
	
	ReleaseMutex(clock_readers_writers_parmas->turn_slide_mutex);
	ReleaseSemaphore(clock_readers_writers_parmas->room_empty_semaphore, 1, NULL);

	


}



Page read_page_table_protected(Page* page_table, ReadersWritersParam* page_table_readers_writers_parmas, int index_of_page_to_access){
	Page page_read;

	//need to add function to check failure and exit correctly

	WaitForSingleObject(page_table_readers_writers_parmas->turn_slide_mutex, INFINITE);
	ReleaseMutex(page_table_readers_writers_parmas->turn_slide_mutex);

	WaitForSingleObject(page_table_readers_writers_parmas->mutex, INFINITE);


	page_table_readers_writers_parmas->readers += 1;
	if (page_table_readers_writers_parmas->readers == 1) {
		WaitForSingleObject(page_table_readers_writers_parmas->room_empty_semaphore, INFINITE);

	}
	ReleaseMutex(page_table_readers_writers_parmas->mutex);

	//critical section for readers

	page_read = page_table[index_of_page_to_access];

	//end ofcritical section for readers

	WaitForSingleObject(page_table_readers_writers_parmas->mutex, INFINITE);
	page_table_readers_writers_parmas->readers -= 1;

	if (page_table_readers_writers_parmas->readers == 0) {

		ReleaseSemaphore(page_table_readers_writers_parmas->room_empty_semaphore, 1, NULL);

	}


	ReleaseMutex(page_table_readers_writers_parmas->mutex);

	return page_read;
}

void write_to_page_table_protected(Page* page_table, ReadersWritersParam* page_table_readers_writers_parmas, int index_of_page_to_access, Page new_page_to_write) {

	WaitForSingleObject(page_table_readers_writers_parmas->turn_slide_mutex, INFINITE);
	WaitForSingleObject(page_table_readers_writers_parmas->room_empty_semaphore, INFINITE);

	//critical section for writers

	page_table[index_of_page_to_access] = new_page_to_write;

	//end ofcritical section for writers

	ReleaseMutex(page_table_readers_writers_parmas->turn_slide_mutex);
	ReleaseSemaphore(page_table_readers_writers_parmas->room_empty_semaphore, 1, NULL);




}


void iterate_over_page_table_and_search_for_avaliable_frame(ROW_THREAD_params_t* p_params, int * index_of_free_frame, int * index_of_page_where_end_time_has_passed) {

	Page page_read;
	int current_time = read_current_time_protected(p_params->clock_readers_writers_parmas, p_params->current_time);
	for (int i = 0; i < current_time; i++) {

		page_read = read_page_table_protected(p_params->page_table, p_params->page_table_readers_writers_parmas, i);
		if (page_read.end_time <= *(p_params->current_time)) {

			*index_of_page_where_end_time_has_passed = i;
		}

		else { 

			//index_of_free_frame=check_if_frame_is_not_in_any_pages_and_return_its_index(page_table)
		}

	}


	
}



