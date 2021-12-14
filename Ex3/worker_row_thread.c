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
	int page_number = floor((p_params->parsed_row_array[NUM_OF_ROW_VARIABLES - 2]) / SIZE_OF_PAGE);
	int time_of_use = p_params->parsed_row_array[NUM_OF_ROW_VARIABLES-1];
	int* current_time = p_params->current_time;
	HANDLE semaphore = p_params->semaphore;
	Page* page_table=p_params->page_table;

	//page_table->valid should be inside readers
	if (page_table->valid == false) {
		int index_of_free_frame=0;
		int index_of_page_where_end_time_has_passed=0;
		//this function should be protected by reader
		//iterate_over_page_table_and_search_for_avaliable_frame(page_table, &index_of_free_frame, &index_of_page_where_end_time_has_passed)

		//check if free frame was not found
		if (index_of_free_frame == -1 && index_of_page_where_end_time_has_passed == -1) {

			//signal to_main that goes to waiting mode, so that main could continue.
			ReleaseSemaphore(semaphore, 1, NULL);

			//waiting_mode()

		}

		else {

			if (index_of_page_where_end_time_has_passed != -1) {

				//print eviction line to output
				// print placement line to output
				//update page table
			
			}

			else {

			// print placement line to output
			//update page table

			}
		}


	}

	//this page already has a frame
	else {
		
		//update end_time of table if needed.
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

