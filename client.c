/*CSci4061 F2018 Assignment 2
*section:  1
* date: 10.10.18
* name: Ali Adam, Abdirahman Abdirahman, Mohamed Said, Hassan Ali
* id: 5330610, 5330419, 5115657, 4570634 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "comm.h"
#include "util.h"




/* -------------------------Main function for the client ----------------------*/
void main(int argc, char * argv[]) {

	int pipe_user_reading_from_server[2], pipe_user_writing_to_server[2];

  char *seg = NULL;
	ssize_t nread;
	char buf[MAX_MSG];
	// You will need to get user name as a parameter, argv[1].

	if(connect_to_server("SOME_UNIQUE_ID", argv[1], pipe_user_reading_from_server, pipe_user_writing_to_server) == -1) {
		exit(-1);
	}

	close(pipe_user_reading_from_server[1]);
  close(pipe_user_writing_to_server[0]);

	fcntl(pipe_user_reading_from_server[0], F_SETFL, fcntl(pipe_user_reading_from_server[0], F_GETFL)| O_NONBLOCK);
	fcntl(0, F_SETFL, fcntl(0, F_GETFL)| O_NONBLOCK);
	print_prompt(argv[1]);
	/* -------------- YOUR CODE STARTS HERE -----------------------------------*/

	while(1){
	// poll pipe retrieved and print it to sdiout
	if ((nread = read(pipe_user_reading_from_server[0],buf,MAX_MSG)) > -1){
		if (nread == 0){
			exit(-1);
		}else{
			if(strcmp(buf,"seg") == 0)
				*seg = 1;
			if (strcmp(buf, "Max User Reached")== 0){
				printf("%s\n",buf);
				exit(-1);
			}
			if (strcmp(buf, "This User ID is Taken") == 0){
				printf("%s\n",buf);
				exit(-1);
			}
			printf("\n");
			write (1,buf,nread);
			printf("\n");
			print_prompt(argv[1]);
			memset(buf,'\0',strlen(buf));
		}
}

	// Poll stdin (input from the terminal) and send it to server (child process) via pipe
	if ((nread = read(0,buf,MAX_MSG)) > 0){
		write (pipe_user_writing_to_server[1],buf,nread);
		print_prompt(argv[1]);
		memset(buf,0,strlen(buf));
}
	usleep(100);
}
	/* -------------- YOUR CODE ENDS HERE -----------------------------------*/
}

/*--------------------------End of main for the client --------------------------*/
