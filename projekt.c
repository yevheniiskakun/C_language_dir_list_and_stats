#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h> 
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 1000

int found_times = 0;
int files_amount = 0;
int folders_amount = 0;
char word[50];

void* user_interface(void *arg)
{
		printf("Searching...\n");
		sleep(1);
		printf("Results\n");
}

void* list_dir (void *args)
{	
    sleep(2);
    char *ld_args = (char*)args;
    DIR * d;
    char file_path[100];
    char str[BUFFER_SIZE];
    char dir_name[100];
    strcpy(dir_name, &ld_args[0]);
    printf("Word: %s\n", word);
    char *pos;
    /* Open the directory specified by "dir_name". */

    d = opendir (dir_name);

    /* Check it was opened. */
    if (! d) {
        fprintf (stderr, "Cannot open directory '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
    while (1) {
        struct dirent * entry;
        const char * d_name;

        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir (d);
        if (! entry) {
            /* There are no more entries in this directory, so break
               out of the while loop. */
            break;
        }
        d_name = entry->d_name;
        /* Print the name of the file and directory. */
	printf ("%s/%s\n", dir_name, d_name);
	char file_name[100];
        strcpy(file_name, entry->d_name);
        FILE *fptr;
	fptr = fopen(file_name, "r");
        if(strchr(file_name, '.') == 0)
		{
			printf("Unnable to check file %s because it is directory \n", file_name);
			folders_amount++;
			continue;
		}
		else{
		files_amount++;
		if(fptr == NULL)
		{
			printf("Unnable to open file %s \n", file_name);
			exit(EXIT_FAILURE);
		}
		while ((fgets(str, BUFFER_SIZE, fptr)) != NULL)
		    {
			
			
			// Find first occurrence of word in str
			pos = strstr(str, word);

			if (pos != NULL)
			{
			    // First index of word in str is 
			    // Memory address of pos - memory
			    // address of str.
			    printf("*Find match in file %s\n", file_name);
			    found_times++;
			    break;
			}
		    }
				
			}	


        if (entry->d_type & DT_DIR) {

            /* Check that the directory is not "d" or d's parent. */
            
            if (strcmp (d_name, "..") != 0 &&
                strcmp (d_name, ".") != 0) {
                int path_length;
                char path[PATH_MAX];
                
 
                path_length = snprintf (path, PATH_MAX,
                                        "%s/%s", dir_name, d_name);
                printf ("%s\n", path);
                if (path_length >= PATH_MAX) {
                    fprintf (stderr, "Path length has got too long.\n");
                    exit (EXIT_FAILURE);
                }
                
               
                /* Recursively call "list_dir" with the new path. */
                list_dir (ld_args);
                
            }
	}
    }
    /* After going through all the entries, close the directory. */
    if (closedir (d)) {
        fprintf (stderr, "Could not close '%s': %s\n",
                 dir_name, strerror (errno));
        exit (EXIT_FAILURE);
    }
   
}

int main ()
{
    
    // Directory path to list files
    char path[100];
    char temp;
    char ld_args[3][200];
    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);
    
    printf("Enter word to search: ");
    scanf("%c", &temp);    // clear the buffer
    scanf("%[^\n]", word);
    strcpy(ld_args[0], path);
   
    //list_dir (ld_args);
    pthread_t first_thread; 
    pthread_t second_thread; 
    if ( pthread_create( &first_thread, NULL, user_interface, NULL))
	{
		perror("Can not create first thread \n"); 
		return -1;
	}
	if ( pthread_create( &second_thread, NULL, list_dir, (void *)ld_args)) 
	{
		perror("Can not create second thread \n"); 
		return -1;
	}
	if ( pthread_join ( first_thread, NULL ) ) 
	{ 
		perror("Can not join first thread \n"); 
		return -1;
	}if ( pthread_join ( second_thread, NULL ) ) 
	{ 
		perror("Can not join second thread \n"); 
		return -1;
	}
    
     printf("---------------Statistics--------------- \n");
    printf("Found %i folder(s) \n", folders_amount);
    printf("Searched in %i file(s) \n", files_amount);
    printf("Found word %s in %i file(s) \n", word, found_times);
    printf("----------------------------------------- \n");
    return 0;
}
