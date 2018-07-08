#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "spellchecker.h"

//load dictionary from dict file
char **dict_load(char *name){
	FILE *fp;
	char buff[MAX_LEN];
	char **dict;
	int index = 0;

	if((dict = malloc(DICT_SIZE * sizeof(char *))) == NULL){
		printf("%s\n" , "Cannot allocate");
		return NULL;
	}

	fp = fopen(name , "r");
	while(fgets(buff , sizeof(buff) , fp)){
		if((dict[index] = malloc(strlen(buff) * sizeof(char *) + 1)) == NULL){
			printf("%s\n" , "Cannot allocate");
			return NULL;
		}
		strncpy(dict[index] , buff , strlen(buff) - 1);
        index++;
	}
	fclose(fp);
	dict[index] = NULL;

	return dict;
}

//create socket that listen to any connection
//return the socket descriptor
int open_listenfd(char *port){
    int listenfd;
    int opt = 1;
    struct sockaddr_in serveraddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		return -1;
    }
 
    if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt , sizeof(opt))) < 0){  
    	return -1;
    }

   	memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons(strtol(port , NULL , 10)); 
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
		return -1;
    }

    if(listen(listenfd , LISTENQ) < 0){
    	return -1;
    }


    printf("SUCCESSFULLY created socket descriptor\n");
    printf("Listening on port %s\n", port);

    return listenfd;

}

//read in user input
ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;                    
    size_t totRead;                     
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;                       

    totRead = 0;
    for (;;) {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) {
            if (errno == EINTR)         
                continue;
            else
                return -1;              

        } else if (numRead == 0) {      
            if (totRead == 0)           
                return 0;
            else                       
                break;

        } else {                        
            if (totRead < n - 1) {      
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
                break;
        }
    }

    *buf = '\0';
    return totRead;
}



