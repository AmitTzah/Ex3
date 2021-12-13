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



main(int argc, char* argv[]) {

	size_t num_of_virtual_bits_index = atoi(argv[1]) - 12;
	size_t num_of_physycal_bits_index = atoi(argv[2]) - 12;

	size_t num_of_frames = pow(2.0, num_of_physycal_bits_index);
	size_t num_of_pages = pow(2.0, num_of_virtual_bits_index);

	Page* page_table = create_and_init_page_table(num_of_pages);


	char* path_to_input_file = argv[3];
	int i = 0;
	int clock[6] = { 0, 100, 900, 1000, 1500, 2000 };

	int current_time;


	//paging_table = array of stucts.Each struct has(frame_num, valid bit, end_time);

	

	while (i<6){

	//put inside protected area for clock writers
		current_time = clock[i];
	

	/*
	//this thread will either finish/or will get put into waiting mode, then it will update the clock
	thread_at_current_time = create_thread(&current_time, input_row, page_table, output_file);

	if (failed thread creation{
	free_and_close_all_handles();

	}

	*/

	//the thread will singnal once finished or puts into waitining mode.
	//If the thread needs to wait for a frame, this wiil signal so as to allow main to create more threads!
	//Signal_when_thread_is_put_to_waiting_mode_or_finishes()




	i++;
}


//print_left_over_evictions(page_table, output_file)
//free_handles

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