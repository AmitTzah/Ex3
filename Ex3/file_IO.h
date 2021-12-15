//includes functions related to file handling

#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>


int WinReadFromFile(char* pathToFile, char* buffer_for_bytes_read, int num_of_bytes_to_read, int read_from_offset);

int WinWriteToFile(char* pathToFile, char* stringToWrite, int StringLen, int write_from_offset);

int num_of_rows_in_a_file(char* pathToFile);

int return_numbers_of_chars_in_file(char * pathToFile);

void parse_row_to_array_of_ints(char* pathToFile, int index_of_wanted_line, int* arr_to_hold_parsed_values, int arr_size);

int Write_to_output(char* pathToFile, int virtual_page_num, int physycal_page_num, int time, BOOL E, int write_from_offset);

int get_num_of_digits_in_an_int_number(int num);

int Write_to_output(char* pathToFile, int virtual_page_num, int physycal_page_num, int time, BOOL need_to_empty, int write_from_offset);

void add_member_to_sort_array(int* arr, int array_size, int num_to_add);
#endif

