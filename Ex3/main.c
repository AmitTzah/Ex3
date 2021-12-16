/*
Authors:

Amit Tzah 316062959
Tomer Shimshi 203200480


Project: Ex3
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>


#include "create_and_handle_processes.h"
#include "worker_row_thread.h"
#include "HardCodedData.h"
#include "file_IO.h"

int write_to_output_from_offset;
HANDLE output_file_mutex;

int main(int argc, char* argv[]) {


	
	char* path_to_input_file = argv[3];

	// ##### DELETE THIS ###################
	char* output_pathe = (char*)malloc(sizeof(char));
	find_output_path(path_to_input_file, output_pathe);

	unsigned int current_time;
	DWORD wait_code;
		
	unsigned int overall_num_of_threads = num_of_rows_in_a_file(path_to_input_file);
	ROW_THREAD_params_t* p_parameters_struct = NULL;

	write_to_output_from_offset = 0;

	unsigned int num_of_virtual_bits_index = atoi(argv[1]) - 12;
	unsigned int num_of_physycal_bits_index = atoi(argv[2]) - 12;

	unsigned int num_of_frames = pow(2.0, num_of_physycal_bits_index);
	unsigned int num_of_pages = pow(2.0, num_of_virtual_bits_index);
	
	// the following are parameters used for the reader - writer solution presented in tirgul7
	ReadersWritersParam clock_readers_writers_parmas = create_and_init_readers_writers_param_struct(overall_num_of_threads);
	ReadersWritersParam page_table_readers_writers_parmas = create_and_init_readers_writers_param_struct(overall_num_of_threads);

	output_file_mutex = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL);  /* un-named */


	Page* page_table = create_and_init_page_table(num_of_pages);
	HANDLE* array_of_thread_pointers = calloc(overall_num_of_threads, sizeof(HANDLE));
	DWORD* p_thread_ids = calloc(overall_num_of_threads, sizeof(DWORD));
	ROW_THREAD_params_t* array_of_thread_parameters_structs = calloc(overall_num_of_threads, sizeof(ROW_THREAD_params_t));
	int* parsed_row_array = calloc(NUM_OF_ROW_VARIABLES, sizeof(int));
	HANDLE* array_of_semaphore_objects = create_and_init_array_semaphore_objects(overall_num_of_threads, 0, 2);

	if (array_of_thread_pointers == NULL || p_thread_ids == NULL || array_of_thread_parameters_structs == NULL 
		|| output_file_mutex==NULL|| parsed_row_array==NULL || page_table==NULL || array_of_semaphore_objects==NULL) {
		
		printf("Memory allocation failed in main!");
		goto Main_Cleanup;

	}

	//initialize array_of_thread_parameters_structs 
	for (unsigned int j = 0; j < overall_num_of_threads; j++)

	{
		array_of_thread_parameters_structs[j].page_table = page_table;
	
	}

	unsigned int i = 0;

	while (i< overall_num_of_threads){

	parse_row_to_array_of_ints(path_to_input_file, i, parsed_row_array, NUM_OF_ROW_VARIABLES);

	//update current_time to next time.
	write_to_current_time_protected(parsed_row_array[0],  &clock_readers_writers_parmas, &current_time);

	//this Sleep() function allows the threads which are currently waiting for a frame, to check if frames were freed up in the gap bettwen the current and previous time.
	//It does not slow down the program! since if there are no other threads ready to run, the function returns immediately. 
	Sleep(THREAD_TIMEOUT_IN_MS);

	//pass thread parameters
	p_parameters_struct = &(array_of_thread_parameters_structs[i]);
	p_parameters_struct->current_time = &current_time;
	p_parameters_struct->row_index = i;
	p_parameters_struct->clock_readers_writers_parmas = &clock_readers_writers_parmas;
	p_parameters_struct->page_table_readers_writers_parmas = &page_table_readers_writers_parmas;
	p_parameters_struct->size_of_page_table = num_of_pages;
	p_parameters_struct->num_of_frames = num_of_frames;
	p_parameters_struct->parsed_row_array = parsed_row_array;
	p_parameters_struct->page_table = page_table;
	p_parameters_struct->semaphore = array_of_semaphore_objects[i];

	//this thread will either finish/or will get put into waiting mode, then it will update the clock
	array_of_thread_pointers[i] = CreateThreadSimple(worker_row_thread, p_parameters_struct, &(p_thread_ids[i]));

	if (array_of_thread_pointers[i] == NULL)
	{
		
		goto Main_Cleanup;
	}


	//the thread will signal once it finishes or goes into waitining mode.
	//If the thread needs to wait for a frame, this wiil signal, so as to allow main to create more threads!
	wait_code = WaitForSingleObject(array_of_semaphore_objects[i], INFINITE);

	if (wait_code == WAIT_FAILED)
	{
		const int error = GetLastError();
		printf("Error when waiting for  thread, error code: %d\n", error);
		goto Main_Cleanup;
	}

	i++;


	}

	//Allow threads that are still in waiting mode after the last clock time to catch finished frames.
	write_to_current_time_protected(INFINITE, &clock_readers_writers_parmas, &current_time);
	Sleep(THREAD_TIMEOUT_IN_MS);
	
	print_left_over_evictions(page_table, num_of_pages);
	
Main_Cleanup:

	close_array_of_handles(array_of_thread_pointers, overall_num_of_threads);
	close_array_of_handles(array_of_semaphore_objects, overall_num_of_threads);
	if(output_file_mutex!=NULL) CloseHandle(output_file_mutex);

	if (clock_readers_writers_parmas.mutex != NULL) CloseHandle(clock_readers_writers_parmas.mutex);
	if (clock_readers_writers_parmas.room_empty_semaphore != NULL) CloseHandle(clock_readers_writers_parmas.room_empty_semaphore);
	if (clock_readers_writers_parmas.turn_slide_mutex != NULL) CloseHandle(clock_readers_writers_parmas.turn_slide_mutex);

	if (page_table_readers_writers_parmas.mutex != NULL) CloseHandle(page_table_readers_writers_parmas.mutex);
	if (page_table_readers_writers_parmas.room_empty_semaphore != NULL) CloseHandle(page_table_readers_writers_parmas.room_empty_semaphore);
	if (page_table_readers_writers_parmas.turn_slide_mutex != NULL) CloseHandle(page_table_readers_writers_parmas.turn_slide_mutex);

	free(array_of_thread_pointers);
	free(p_thread_ids);
	free(array_of_thread_parameters_structs);
	free(page_table);
	free(parsed_row_array);
	free(array_of_semaphore_objects);

	return 0;

}

