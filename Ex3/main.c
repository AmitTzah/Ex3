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


int main(int argc, char* argv[]) {

	size_t num_of_virtual_bits_index = atoi(argv[1]) - 12;
	size_t num_of_physycal_bits_index = atoi(argv[2]) - 12;

	size_t num_of_frames = pow(2.0, num_of_physycal_bits_index);
	size_t num_of_pages = pow(2.0, num_of_virtual_bits_index);

	Page* page_table = create_and_init_page_table(num_of_pages);


	char* path_to_input_file = argv[3];
	size_t i = 0;
	int clock[6] = { 0, 100, 900, 1000, 1500, 2000};

	int current_time;

	size_t overall_num_of_threads = num_of_rows_in_a_file(path_to_input_file);

	HANDLE* array_of_thread_pointers = calloc(overall_num_of_threads, sizeof(HANDLE));
	DWORD* p_thread_ids = calloc(overall_num_of_threads, sizeof(DWORD));
	ROW_THREAD_params_t* p_parameters_struct=NULL;
	
	DWORD wait_code;


	ROW_THREAD_params_t* array_of_thread_parameters_structs = calloc(overall_num_of_threads, sizeof(ROW_THREAD_params_t));

	if (array_of_thread_pointers == NULL || p_thread_ids == NULL || array_of_thread_parameters_structs == NULL) {
		printf("Memory allocation to array of thread pointers failed in main!");
		exit(1);
	}


	//initialize array_of_thread_parameters_structs 
	for (size_t j = 0; j < overall_num_of_threads; j++)

	{
		array_of_thread_parameters_structs[j].page_table = page_table;
	
	}

	while (i< overall_num_of_threads){


	//put inside protected area for clock writers
		current_time = clock[i];
	

	//this thread will either finish/or will get put into waiting mode, then it will update the clock
	p_parameters_struct = &(array_of_thread_parameters_structs[i]);
	p_parameters_struct->current_time = clock;
	p_parameters_struct->row_index = i;
	//p_parameters_struct->parsed_row_array=

	array_of_thread_pointers[i] = CreateThreadSimple(worker_row_thread, p_parameters_struct, &(p_thread_ids[i]));

	

	

	//the thread will singnal once finished or puts into waitining mode.
	
	//If the thread needs to wait for a frame, this wiil signal so as to allow main to create more threads!
	//Signal_when_thread_is_put_to_waiting_mode_or_finishes()




	i++;

	}


	//print_left_over_evictions(page_table, output_file)
	//free_handles


	return SUCCESS_CODE;

}



/*
For every critical section where both readersand writers can access the same resource(be it page_table or clock) we need to employ the writer - reader solution.
For that we need :

the following globals :
int readers = 0;
Mutex = mutex
roomEmpty.

(for every critical section, we need another set of those three.)

*/