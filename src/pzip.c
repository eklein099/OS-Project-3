#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

int n_threads_g = 0;
char *input_chars_g; 
int input_chars_size_g = 0;
struct zipped_char *zipped_chars_g;
int *zipped_chars_count_g;
int *char_frequency_g;
int size;
pthread_mutex_t lock;

static void *thread_zip(void *nv){

	int n = (int)(nv);
	int start = size*n;
	int end = size*n + size;

	struct zipped_char *local_zipped = malloc(sizeof(struct zipped_char)*(input_chars_size_g)/n_threads_g);
	struct zipped_char *zip = local_zipped;
        int count = 0;

        for(int i = start; i < end; i++){
                //different char or first char
                if(i == start | input_chars_g[i] != input_chars_g[i-1]){
                        if(i != 0){
                                zip++;
                        }

                        zip->character = input_chars_g[i];
                        zip->occurence = 1;
                        count++;
                }else{  //same char
                        zip->occurence++;
                }
		//lock
                char_frequency_g[input_chars_g[i]-97]++;
		//unlock
        }
	//lock
        *zipped_chars_count_g = count;
	//unlock
 	
 	//barier	
	//copy everything over


	free(local_zipped);
	
	return NULL;
}

static void serial(int n_threads, char *input_chars, int input_chars_size,
          struct zipped_char *zipped_chars, int *zipped_chars_count,
          int *char_frequency)
{

        struct zipped_char *zip = zipped_chars;
        int count = 0;

        for(int i = 0; i < input_chars_size; i++){
                //different char or first char
                if(i == 0 | input_chars[i] != input_chars[i-1]){
                        if(i != 0){
                                zip++;
                        }

                        zip->character = input_chars[i];
                        zip->occurence = 1;
                        count++;
                }else{  //same char
                        zip->occurence++;
                }
                char_frequency[input_chars[i]-97]++;
        }
        *zipped_chars_count = count;
        //printf("%d",*zipped_chars_count);
}



/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */
void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	pthread_mutex_init(&lock,NULL); //do some error handling here

	n_threads_g = n_threads;
	input_chars_g = input_chars;
	input_chars_size_g = input_chars_size;
	zipped_chars_g = zipped_chars;
	zipped_chars_count_g = zipped_chars_count;
	char_frequency_g = char_frequency;
	size = input_chars_size_g/n_threads_g;

	printf("input_chars_size_g: %d\n",input_chars_size_g);
	printf("n_threads_g: %d\n",n_threads_g);

	serial(n_threads_g, input_chars_g, input_chars_size_g, zipped_chars_g, zipped_chars_count_g, char_frequency_g);

	pthread_mutex_destroy(&lock);
}
