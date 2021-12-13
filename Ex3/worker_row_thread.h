//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#ifndef WORKER_ROW_THREAD_H
#define WORKER_ROW_THREAD_H

#include <stddef.h>
#include <stdbool.h>

typedef struct Pages{

	size_t frame_num;
	bool valid;
	size_t end_time;

} Page;


Page* create_and_init_page_table(size_t num_of_pages);



#endif
