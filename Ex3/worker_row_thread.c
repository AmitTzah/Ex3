//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "worker_row_thread.h"
#include "file_IO.h"

extern int write_to_output_from_offset;
extern HANDLE output_file_mutex;



//this function iterates over page_table in a rising index search for frames.
//if a frame is found that is not in any page, index_of_free_frame is set to the correct index in caller.
//if a frame is found in a page where the end time has passed(smaller than current time), then the other index is set.

void iterate_over_page_table_and_search_for_avaliable_frame(ROW_THREAD_params_t* p_params, int* index_of_free_frame, int* index_of_page_where_end_time_has_passed) {
	int i;
	Page page_read;
	unsigned int current_time = read_current_time_protected(p_params->clock_readers_writers_parmas, p_params->current_time);
	int numebr_of_frames = p_params->num_of_frames;
	int was_frame_found_in_page_table = 0;

	for (i = 0; i < numebr_of_frames; i++) {
		for (unsigned int j = 0; j < p_params->size_of_page_table; j++) {
			page_read = read_page_table_protected(p_params->page_table, p_params->page_table_readers_writers_parmas, j);
			if (page_read.valid == true && page_read.frame_num == i) {
				if (page_read.end_time <= current_time) {

					*index_of_page_where_end_time_has_passed = j;
					return;

				}
				else {
					was_frame_found_in_page_table = 1;
					break;
				}

			}
		}
		if (was_frame_found_in_page_table == 0) {
			*index_of_free_frame = i;
			return;
		}

		was_frame_found_in_page_table = 0;
	}




}


//All threads that are currently waiting for a frame to evict. 
//When a frame does evict, they "race" to catch it.
void waiting_mode(unsigned int time_of_use, int page_index, ROW_THREAD_params_t* p_params) {

	//contastnly reading page_table to find a frame where end_time<=current_time
	//when finds such frame, it prints evict as needed and update the page table, then print placement.
	//should be sempahore protected as the read/write problem code.
	while (true) {
		for (unsigned int i = 0; i < p_params->size_of_page_table; i++) {
			Page page_read = read_page_table_protected(p_params->page_table, (p_params->page_table_readers_writers_parmas), i);
			unsigned int current_time = read_current_time_protected(p_params->clock_readers_writers_parmas, p_params->current_time);

			if ((page_read.end_time <= current_time) && page_read.valid == true) {

				// needs to be mutex protected
				WaitForSingleObject(output_file_mutex, INFINITE);

				//Write_to_output(page_read.end_time,i, page_read.frame_num, E)
				write_to_output_from_offset = write_to_output(p_params->path_to_output, i, page_read.frame_num, page_read.end_time, true, write_to_output_from_offset);
				//print_placement_line_to_output(page_read.end_time, page_index,page_read.frame_num, P)
				write_to_output_from_offset = write_to_output(p_params->path_to_output, page_index, page_read.frame_num, page_read.end_time, false, write_to_output_from_offset);

				ReleaseMutex(output_file_mutex);

				Page new_page_to_write;
				new_page_to_write.end_time = time_of_use + page_read.end_time;
				new_page_to_write.valid = true;
				new_page_to_write.frame_num = page_read.frame_num;

				write_to_page_table_protected(p_params->page_table, p_params->page_table_readers_writers_parmas, page_index, new_page_to_write);

				Page change_valid_to_false_in_old_page;
				change_valid_to_false_in_old_page.end_time = time_of_use + page_read.end_time;
				change_valid_to_false_in_old_page.valid = false;
				change_valid_to_false_in_old_page.frame_num = page_read.frame_num;

				write_to_page_table_protected(p_params->page_table, p_params->page_table_readers_writers_parmas, i, change_valid_to_false_in_old_page);

				return;
			}

		}
	}
}

//the worker thread
//each thread handles a row from the input file.
DWORD WINAPI worker_row_thread(LPVOID lpParam) {

	ROW_THREAD_params_t* p_params;

	/* Check if lpParam is NULL */
	if (NULL == lpParam)
	{
		return ROW_THREAD__CODE_NULL_PTR;
	}

	//Convert (void *) to parameters type.
	p_params = (ROW_THREAD_params_t*)lpParam;

	unsigned int time = p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 3];
	int page_index = floor((p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 2]) / SIZE_OF_PAGE);
	unsigned int time_of_use = p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 1];
	int* current_time = p_params->current_time;
	HANDLE semaphore = p_params->semaphore;
	Page* page_table = p_params->page_table;

	Page page_read = read_page_table_protected(page_table, (p_params->page_table_readers_writers_parmas), page_index);


	if (page_read.valid == false) {
		// a frame is free if it's not in any of the pages(regardless if end time has passed)
		int index_of_free_frame = -1;
		int index_of_page_where_end_time_has_passed = -1;

		iterate_over_page_table_and_search_for_avaliable_frame(p_params, &index_of_free_frame, &index_of_page_where_end_time_has_passed);

		//check if free frame was not found
		if (index_of_free_frame == -1 && index_of_page_where_end_time_has_passed == -1) {

			//signal to_main that goes to waiting mode, so that main could continue.
			ReleaseSemaphore(semaphore, 1, NULL);

			waiting_mode(time_of_use, page_index, p_params);

		}

		else {

			//found a frame that can be evicted then used.
			if (index_of_page_where_end_time_has_passed != -1) {


				// needs to be mutex protected
				WaitForSingleObject(output_file_mutex, INFINITE);

				//print_eviction_line_to_output(time, index_of_page_where_end_time_has_passed,page_read.frame_num, E)
				page_read = read_page_table_protected(page_table, (p_params->page_table_readers_writers_parmas), index_of_page_where_end_time_has_passed);
				write_to_output_from_offset = write_to_output(p_params->path_to_output, index_of_page_where_end_time_has_passed, page_read.frame_num, time, true, write_to_output_from_offset);
				// print placement line to output(time, page_index,page_read.frame_num, p)
				write_to_output_from_offset = write_to_output(p_params->path_to_output, page_index, page_read.frame_num, time, false, write_to_output_from_offset);

				ReleaseMutex(output_file_mutex);

				//update page table
				Page new_page;
				new_page.valid = true;
				new_page.end_time = time + time_of_use;
				new_page.frame_num = page_read.frame_num;
				write_to_page_table_protected(page_table, p_params->page_table_readers_writers_parmas, page_index, new_page);

				Page change_valid_to_false_in_old_page;
				change_valid_to_false_in_old_page.end_time = time_of_use + page_read.end_time;
				change_valid_to_false_in_old_page.valid = false;
				change_valid_to_false_in_old_page.frame_num = page_read.frame_num;

				write_to_page_table_protected(p_params->page_table, p_params->page_table_readers_writers_parmas, index_of_page_where_end_time_has_passed, change_valid_to_false_in_old_page);


			}

			//found frame that wasn't in any page
			else {
				// needs to be mutex protected
				WaitForSingleObject(output_file_mutex, INFINITE);
				// print placement line to output(time, page_index,index_of_free_frame, p)
				write_to_output_from_offset = write_to_output(p_params->path_to_output, page_index, index_of_free_frame, time, false, write_to_output_from_offset);

				ReleaseMutex(output_file_mutex);


				//update page table
				Page new_page;
				new_page.valid = true;
				new_page.end_time = time + time_of_use;
				new_page.frame_num = index_of_free_frame;
				write_to_page_table_protected(page_table, p_params->page_table_readers_writers_parmas, page_index, new_page);

			}
		}


	}

	//this page already has a frame
	else {

		//update end time of table if needed.
		if (page_read.end_time < (time + time_of_use)) {
			Page new_page;
			new_page.valid = true;
			new_page.end_time = time + time_of_use;
			new_page.frame_num = page_read.frame_num;
			write_to_page_table_protected(page_table, p_params->page_table_readers_writers_parmas, page_index, new_page);

		}
	}


	printf("Finished working on a row number %d\n", p_params->row_index);

	//signaled to_main that finished work, so that main could continue.
	ReleaseSemaphore(semaphore, 1, NULL);
	return ROW_THREAD__CODE_SUCCESS;
}

//Creates an array of pages, allocated on heap so should be freed in caller. Each page is initialized to valid=0.
//if failed, will return null. this should be checked in caller and handled accordingly!
Page* create_and_init_page_table(unsigned int num_of_pages) {

	Page* page_table = calloc(num_of_pages, sizeof(Page));


	if (page_table == NULL) {
		printf("Memory allocation to page_table array failed!");
		free(page_table);
		return NULL;
	}
	else {
		for (unsigned int i = 0; i < num_of_pages; i++)
		{
			page_table[i].valid = false;

		}
	}
	return page_table;
}


//based on reader/writers solution presented in tirgul.
//Clock_reader_writers_params are global, and whenever a thread wants to read the clock it does it through this protected zone.
unsigned int read_current_time_protected(ReadersWritersParam* clock_readers_writers_parmas, unsigned int* current_time) {
	unsigned int current_t;


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

	current_t = *current_time;

	//end ofcritical section for readers

	WaitForSingleObject(clock_readers_writers_parmas->mutex, INFINITE);
	clock_readers_writers_parmas->readers -= 1;

	if (clock_readers_writers_parmas->readers == 0) {

		ReleaseSemaphore(clock_readers_writers_parmas->room_empty_semaphore, 1, NULL);

	}


	ReleaseMutex(clock_readers_writers_parmas->mutex);

	return current_t;
}

//based on reader/writers solution presented in tirgul.
//Clock_reader_writers_params are global, and whenever a thread wants to write to the clock it does it through this protected zone.
void write_to_current_time_protected(int updated_time, ReadersWritersParam* clock_readers_writers_parmas, int* current_time) {

	WaitForSingleObject(clock_readers_writers_parmas->turn_slide_mutex, INFINITE);
	WaitForSingleObject(clock_readers_writers_parmas->room_empty_semaphore, INFINITE);


	//critical section for writers

	*current_time = updated_time;

	//end ofcritical section for writers

	ReleaseMutex(clock_readers_writers_parmas->turn_slide_mutex);
	ReleaseSemaphore(clock_readers_writers_parmas->room_empty_semaphore, 1, NULL);




}


// based on reader / writers solution presented in tirgul.
//Page_table_reader_writers_params are global, and whenever a thread wants to read the page_table it does it through this protected zone.
Page read_page_table_protected(Page* page_table, ReadersWritersParam* page_table_readers_writers_parmas, int index_of_page_to_access) {
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


// based on reader / writers solution presented in tirgul.
//Page_table_reader_writers_params are global, and whenever a thread wants to write to the page_table it does it through this protected zone.
void write_to_page_table_protected(Page* page_table, ReadersWritersParam* page_table_readers_writers_parmas, int index_of_page_to_access, Page new_page_to_write) {

	WaitForSingleObject(page_table_readers_writers_parmas->turn_slide_mutex, INFINITE);
	WaitForSingleObject(page_table_readers_writers_parmas->room_empty_semaphore, INFINITE);

	//critical section for writers

	page_table[index_of_page_to_access] = new_page_to_write;

	//end ofcritical section for writers

	ReleaseMutex(page_table_readers_writers_parmas->turn_slide_mutex);
	ReleaseSemaphore(page_table_readers_writers_parmas->room_empty_semaphore, 1, NULL);




}

//this function does not need to be read/write protected, since by here all threads but main have finished.
void print_left_over_evictions(Page* page_table, unsigned int num_of_pages, char* path_to_output) {
	unsigned int current_max = 0;

	for (unsigned int i = 0; i < num_of_pages; i++) {
		if (page_table->valid == true) {

			if (current_max < (page_table->end_time)) {

				current_max = page_table->end_time;
			}

		}

		page_table++;
	}

	page_table = page_table - num_of_pages;

	for (unsigned int i = 0; i < num_of_pages; i++) {
		if (page_table->valid == true)

			write_to_output_from_offset = write_to_output(path_to_output, i, page_table->frame_num, current_max, true, write_to_output_from_offset);
		page_table++;
	}


}

