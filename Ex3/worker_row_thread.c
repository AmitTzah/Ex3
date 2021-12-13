//includes functions and structs related to operation of a worker thread(which is project specific). Each thread handles a row from the input file.


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "worker_row_thread.h"



DWORD WINAPI worker_row_thread(LPVOID lpParam) {
	/*
	readdingand writing to table needs to be protected.
		1. the page is empty(valid bit is 0) -

		the thread searches in rising index for an empty frame OR a frame in which the end_time has passed :

	a.it can't find one: it gets into a waiting mode, all the threads that are in waiting mode are in a race condition to fill a frame that opens up.


		b.it finds one : if it's a frame where end_time has passed, the Eviction line is printed to output, it updates_page table then placement is printed, then it finishes its work.


		2.the page is filled -


		a.the time is updated as need, then the thread finishes its work. (nothing is printed)
		*/
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







/*
bool write_to_file(int num_of_real_pages, bool need_to_empty, int frame_num, int i)
{
	int j;
	LONG previous_count;
	bool release_res;
	char* Line_To_Write = (char*)malloc(sizeof(char) * Max_Size_of_Line);
	char temp_str[Max_Size_of_Line];
	if (Line_To_Write == NULL)
	{
		printf("error alocating memory for the output file write string");
		exit(1);

	}
	int things_to_write[2];// enter the waned ints to here
	int num_of_bits_to_write = 0;// saves the number of bits yo write to the output file
	bool foud_page_to_empty = FALSE;

	if (need_to_empty == TRUE)
	{
		for (j = 0; j < num_of_real_pages; j++)
		{
			if (*clock > real_pages[j].End_Time && real_pages[j].valid == TRUE)// meaning the current page finished its time
			{// we need to remove it from the table and updte the output file
				vir_pages[real_pages[j].Frame_num].valid = FALSE;
				real_pages[j].valid = FALSE;
				release_res = ReleaseSemaphore(
					vacent_pages_semaphore,
					1, 		// Signal that exactly one cell was emptied
					&previous_count);
				if (release_res == FALSE) {
					const int error = GetLastError();
					printf("Error when realisng semaphore  mutex error num: %d\n", error);
					exit(1);
				}

				//printf("*semaphore_count =: %d\n", *semaphore_count);
				// now we need to write it to the output file:
				things_to_write[0] = j;
				things_to_write[1] = real_pages[j].Frame_num;
				foud_page_to_empty = TRUE;

			}
		}
	}
	else
	{
		things_to_write[0] = i;
		things_to_write[1] = frame_num;


	}
	if (foud_page_to_empty == TRUE || need_to_empty == FALSE)
	{

		num_of_bits_to_write = get_num_of_digits_in_an_int_number(*clock);
		sprintf(Line_To_Write, "%d", *clock);
		strcat(Line_To_Write, " ");

		sprintf(temp_str, "%d", things_to_write[1]);
		num_of_bits_to_write += get_num_of_digits_in_an_int_number(things_to_write[0]);


		strcat(Line_To_Write, temp_str);
		strcat(Line_To_Write, " ");
		sprintf(temp_str, "%d", things_to_write[0]);
		num_of_bits_to_write += get_num_of_digits_in_an_int_number(things_to_write[1]);
		strcat(Line_To_Write, temp_str);
		strcat(Line_To_Write, " ");
		if (need_to_empty == TRUE)
		{
			strcat(Line_To_Write, "E");
		}
		else
		{
			strcat(Line_To_Write, "P");
		}

		num_of_bits_to_write += 4;// added the spaces and char to the sum
		printf("wrote to output: %s\n", Line_To_Write);
		WinWriteToFile(Output_file_path, Line_To_Write, num_of_bits_to_write, *output_file_offset);
		*output_file_offset += num_of_bits_to_write;
		// start new line
		WinWriteToFile(Output_file_path, "\r\n", 4, *output_file_offset);
		*output_file_offset += 2;
		//return TRUE;
	}
	return TRUE;
}

*/

