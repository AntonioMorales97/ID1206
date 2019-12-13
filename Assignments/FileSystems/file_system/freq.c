#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define FREQ_MAX 32

unsigned long freq[FREQ_MAX];

void add_to_freq(unsigned long size){
	if(size != 0){
		int j = 0;
		int n = 2;
		while(size / n != 0 && j < FREQ_MAX){
			n = 2*n;
			j++;
		}
		freq[j]++;
	}
}

unsigned long  count(char *path) {

	unsigned long total = 0;

	DIR * dirp = opendir(path);

	char subdir[1024];

	struct dirent *entry;

	struct stat file_st;
  if(dirp == NULL){
    printf("not able to open %s\n", path);
    return 0;
  }
	while((entry = readdir(dirp)) != NULL) {

		switch(entry -> d_type) {
			case DT_BLK :		// This is a block device
				//printf("b:");
				break;
			case DT_CHR :		// This is a character device
				//printf("c:");
				break;
			case DT_DIR :		// This is a directory
        if((strcmp(entry -> d_name , ".") == 0) | (strcmp (entry -> d_name , ".." ) == 0)) {
    		    break;
    	  }
				//printf("d:");
				sprintf(subdir, "%s/%s", path, entry -> d_name);
				total += count(subdir);
				break;
			case DT_FIFO :		// This is a named pipe
				//printf("p:");
				break;
			case DT_LNK :		// This is a symbolic link
				//printf("l:");
				break;
			case DT_REG :		// This is a regular file
				//printf("f:");
				if(fstatat(dirfd(dirp), entry -> d_name, &file_st, 0) == 0){
					add_to_freq(file_st.st_size);
				}
				total++;
				break;
			case DT_SOCK : 		// This is a UNIX domain socket
				//printf("s:");
				break;
			case DT_UNKNOWN :	// The file type is unknown
				//printf("u:");
				break;
			default :
				break;
		}
	}
	closedir(dirp);
	return total;
}


int main(int argc, char *argv[]) {
	if(argc <2) {
		perror("usage: total <dir>\n");
		return -1;
	}

	char *path = argv[1];
	unsigned long total = count(path);
	//printf("The directory %s contains %lu files\n", path, total);

	printf("#The directory %s: number of files smaller than 2^k:\n", path);
	printf("#k\tnumber\n");

	for(int j = 0; j < FREQ_MAX; j++){
		printf("%d\t%lu\n", (j+1), freq[j]);
	}

  return 0;
}
