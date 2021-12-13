0 5 2000 //page 0
100 8195 1000 // page 2
900 4555 200 //page 1
1000 9999 200 //page 2
1500 1024 800// page 0
2000 15656 100 //page 3

//paging_table = array of stucts.Each struct has(frame_num, valid bit, end_time);

main() {
	int i = 0;
	int clock[5] = (0, 100, 900, 1000, 1500, 2000)
	

	while (clock hasn't finished){
		open_mutex();

	if (failed mutex creation) {
		free_and_close_all_handles();

	}

	current_time = clock[i];
	close_mutex();

	//give some time for threads in waiting mode to replace an evicted frame.
	wait();


	//this thread will either finish/or will get put into waiting mode, then it will update the clock
	thread_at_current_time = create_thread(&current_time, input_row, page_table, output_file);
	if (failed thread creation{
	free_and_close_all_handles();

		}

		//the thread will singnal once finished or puts into waitining mode.
		//If the thread needs to wait for a frame, this wiil signal so as to allow main to create more threads!
		Signal_when_thread_is_put_to_waiting_mode_or_finishes()


		//give some time for threads in waiting mode to catch the evicted frame.
		wait()

		i++;
}


print_left_over_evictions(page_table, output_file)
free_handles

}
/*
thread_function(&current_time, parsed_input_row, page_table, output_file) {
	
	readding and writing to table needs to be protected.
	1. the page is empty (valid bit is 0)-

		the thread searches in rising index for an empty frame OR a frame in which the end_time has passed :

			a. it can't find one: it gets into a waiting mode, all the threads that are in waiting mode are in a race condition to fill a frame that opens up.


			b. it finds one: if it's a frame where end_time has passed, the Eviction line is printed to output, it updates_page table then placement is printed, then it finishes its work.


	2.the page is filled-


		a. the time is updated as need, then the thread finishes its work. (nothing is printed)


	


}
*/
/*
waiting_mode(&current_time, thread, page_table, output_file) {

	//contastnly reading page_table to find a frame where end_time<=current_time
	//when finds such frame, it prints evict as needed and update the page table, then print placement.
	//should be sempahore protected as the read/write problem code.



}
*/


/*
For every critical section where both readersand writers can access the same resource(be it page_table or clock) we need to employ the writer - reader solution.
For that we need :

the following globals :
int readers = 0;
Mutex = mutex
roomEmpty.

(for every critical section, we need another set of those three.)

*/