/*CSci4061 F2018 Assignment 2
*section:  1
* date: 10.10.18
* name: Ali Adam, Abdirahman Abdirahman, Mohamed Said, Hassan Ali
* id: 5330610, 5330419, 5115657, 4570634 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "comm.h"
#include "util.h"

/* -----------Functions that implement server functionality -------------------------*/

/*
 * Returns the empty slot on success, or -1 on failure mohamed
 */
int find_empty_slot(USER * user_list) {
	// iterate through the user_list and check m_status to see if any slot is EMPTY
	// return the index of the empty slot
    int i = 0;
	for(i=0;i<MAX_USER;i++) {
    	if(user_list[i].m_status == SLOT_EMPTY) {
			return i;
		}
	}
	return -1;
}

/*
 * list the existing users on the server shell
 */

int list_users(int idx, USER * user_list)
{
	int i, flag = 0;
	char buf[MAX_MSG] = {}, *s = NULL;

	/* construct a list of user names */
	s = buf;
	strncpy(s, "---connecetd user list---\n", strlen("---connecetd user list---\n"));
	s += strlen("---connecetd user list---\n");
  // if you find any slot which is not empty, print that m_user_id
	for (i = 0; i < MAX_USER; i++) {   // iterate through the user list
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		flag = 1;
		strncpy(s, user_list[i].m_user_id, strlen(user_list[i].m_user_id));
		s = s + strlen(user_list[i].m_user_id);
		strncpy(s, "\n", 1);
		s++;
	}
  // if every slot is empty, print "<no users>""
	if (flag == 0) {
		strcpy(buf, "<no users>\n");

	} else {
		s--;
		strncpy(s, "\0", 1);
	}

	if(idx < 0) {
		printf("%s",buf);
	} else {
		/* write to the given pipe fd */
		if (write(user_list[idx].m_fd_to_user, buf, strlen(buf) + 1) < 0)
			perror("writing to server shell");
	}

  if(flag!=0)
  {
    printf("\n");
  }
	// return 0 on success
	return 0;
}

/*
 * add a new user
 */
int add_user(int idx, USER * user_list, int pid, char * user_id, int pipe_to_child, int pipe_to_parent)
{
  // populate the user_list structure with the arguments passed to this function
  user_list[idx].m_pid=pid;
  strcpy(user_list[idx].m_user_id, user_id);
  user_list[idx].m_fd_to_user= pipe_to_child;
  user_list[idx].m_fd_to_server= pipe_to_parent;
  user_list[idx].m_status = SLOT_FULL;

	// return the index of user added
	return idx;
}

/*
 * Kill a user
 */
void kill_user(int idx, USER * user_list) {
	// kill a user (specified by idx) by using the systemcall kill()
  kill(user_list[idx].m_pid,SIGKILL);
	// then call waitpid on the user SIGKILL
  waitpid(user_list[idx].m_pid,NULL,WNOHANG);
}

/*
 * Perform cleanup actions after the used has been killed
 */
void cleanup_user(int idx, USER * user_list)
{
	// m_pid should be set back to -1
  user_list[idx].m_pid=-1;
	// m_user_id should be set to zero, using memset()
  memset(user_list[idx].m_user_id,'\0',strlen(user_list[idx].m_user_id));
	// close all the fd
  close(user_list[idx].m_fd_to_user);
  close(user_list[idx].m_fd_to_server);
	// set the value of all fd back to -1
  user_list[idx].m_fd_to_user=-1;
  user_list[idx].m_fd_to_server=-1;
	// set the status back to empty
  user_list[idx].m_status=SLOT_EMPTY;
}

/*
 * Kills the user and performs cleanup
 */
void kick_user(int idx, USER * user_list) {
	// should kill_user()
  kill_user(idx,user_list);
	// then perform cleanup_user()
  cleanup_user(idx, user_list);

}


/*
 * broadcast message to all users
 */
int broadcast_msg(USER * user_list, char *buf, char *sender)
{
  int i;
  char introMsg[MAX_MSG] = "";
  char *seg1 = NULL;
  if (buf[0] != '\n'){
     for (i = 0;i<MAX_USER;i++)       //iterate over the user_list and if a slot is full, and the user is not the sender itself,
     {
       if (user_list[i].m_status == SLOT_EMPTY)
         continue;
       if (strcmp("admin", sender) == 0 )  // send message to all users
       {
         strcat(introMsg,"admin");
         strcat(introMsg,": ");
         strcat(introMsg,buf);
         write(user_list[i].m_fd_to_user,introMsg,strlen(introMsg));
       }
       else if ((user_list[i].m_status == SLOT_FULL) && (strcmp(user_list[i].m_user_id, sender) != 0)) // send the message to all users except user that sent
       {
         strcat(introMsg,sender);
         strcat(introMsg,": ");
         strcat(introMsg,buf);
         write(user_list[i].m_fd_to_user,introMsg,strlen(introMsg));
       }
       memset(introMsg,0,MAX_MSG); // clear buuffer
     }
  }
  memset(buf,0,strlen(buf));
  //return zero on success
  return 0;
}

/*
 * Cleanup user chat boxes
 */
void kill_and_cleanup_users(USER * user_list)
{
  for (int i = 0; i < MAX_USER; i++) {
		if (user_list[i].m_status == SLOT_EMPTY)// go over the user list and check for any empty slots
			continue;
	  kick_user(i , user_list);// call kill user and cleanup user for each of those users.
  }
}
/*
 * find user index for given user name
 */
int find_user_index(USER * user_list, char * user_id)
{
	// go over the  user list to return the index of the user which matches the argument user_id
	// return -1 if not found
  int i, user_idx = -1;

	if (user_id == NULL) {
		fprintf(stderr, "NULL name passed.\n");
		return user_idx;
	}
	for (i=0;i<MAX_USER;i++) {
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		if (strcmp(user_list[i].m_user_id, user_id) == 0) {
			return i;
		}
	}

	return -1;
}

/*
 * given a command's input buffer, extract name
 */
int extract_name(char * buf, char * user_name)
{
  char inbuf[MAX_MSG];
  char * tokens[16];
  strcpy(inbuf, buf);

  int token_cnt = parse_line(inbuf, tokens, " ");

  if(token_cnt >= 2) {
      strcpy(user_name, tokens[1]);
      return 0;
  }

  return -1;
}

int extract_text(char *buf, char * text)
{
  char inbuf[MAX_MSG];
  char * tokens[16];
  char * s = NULL;
  strcpy(inbuf, buf);

  int token_cnt = parse_line(inbuf, tokens, " ");

  if(token_cnt >= 3) {
      s = strchr(buf, ' ');   //Find " "
      s = strchr(s+1, ' ');

      strcpy(text, s+1);
      return 0;
  }
  return -1;

}

/*
 * send personal message
 */
void send_p2p_msg(int idx, USER * user_list, char *buf)
{
  char inbuf [MAX_MSG];
  char text [MAX_MSG];
  char recUser [MAX_USER_ID];
  strcpy(inbuf, buf);
  int name = extract_name(inbuf, recUser);           // get the target user by name using extract_name() function
  int RecID = find_user_index(user_list, recUser);    // find the user id using find_user_index()


  if(RecID == -1 ){// if user not found, write back to the original user "User not found", using the write()function on pipes.
      char errorname[] = "User not found";
      write (user_list[idx].m_fd_to_user,errorname,strlen(errorname));
  }else{// if the user is found then write the message that the user wants to send to that user.
    int textnum = extract_text(inbuf,text);
    if (text[0] =='\\'){
    char errortext[] = "message not found";
    write (user_list[idx].m_fd_to_user,errortext,strlen(errortext));

    }else{
      char introMsg[]="message from ";
      strcat(introMsg,user_list[idx].m_user_id);
      strcat(introMsg," : ");
      write(user_list[RecID].m_fd_to_user,introMsg,strlen(introMsg));
      write(user_list[RecID].m_fd_to_user,text,strlen(text));
   }
    memset(inbuf,0,strlen(inbuf));
    memset(buf,0,strlen(buf));
    memset(recUser,0,strlen(recUser));
  }
  }

void init_user_list(USER * user_list) {

	int i=0;
	for(i=0;i<MAX_USER;i++) {                 //iterate over the MAX_USER
		user_list[i].m_pid = -1;
		memset(user_list[i].m_user_id, '\0', MAX_USER_ID);  //memset() all m_user_id to zero
		user_list[i].m_fd_to_user = -1;         //set all fd to -1
		user_list[i].m_fd_to_server = -1;
		user_list[i].m_status = SLOT_EMPTY;     //set the status to be EMPTY
	}
}

void kick_init(char * buf, USER * user_list)
{
  char inbuf [MAX_MSG];
  char recUser [MAX_USER_ID];
  strcpy(inbuf, buf);

  int name = extract_name(inbuf, recUser);           // get the target user by name using extract_name() function

  int RecID = find_user_index(user_list, recUser);    // find the user id using find_user_index()

  if(RecID == -1)                       // if user not found, write back to the original user "User not found",
  {                                     //using the write()function on pipes.
    printf("user not found\n");
    print_prompt("admin");
  }
  else
  {
    kick_user(RecID,user_list);
  }

  memset(recUser,0,strlen(recUser));
  memset(inbuf,0,strlen(inbuf));
}

/* ---------------------End of the functions that implementServer functionality -----------------*/


/* ---------------------Start of the Main function ----------------------------------------------*/
int main(int argc, char * argv[])
{
  int pidStatus;
	ssize_t nbytes;
  char seg [] = "seg";

	setup_connection("SOME_UNIQUE_ID"); // Specifies the connection point as argument.

	USER user_list[MAX_USER];
	init_user_list(user_list);   // Initialize user list

	char buf[MAX_MSG];
	fcntl(0, F_SETFL, fcntl(0, F_GETFL)| O_NONBLOCK);
	print_prompt("admin");
  int x;

	while(1) {
		/* ------------------------YOUR CODE FOR MAIN--------------------------------*/

    int pipe_SERVER_reading_from_child[2];
    int pipe_SERVER_Writing_to_child[2];
    int pipe_child_reading_from_user[2];
    int pipe_child_writing_to_user[2];
    char user_id[MAX_USER_ID];

    if (x = get_connection(user_id, pipe_child_writing_to_user,pipe_child_reading_from_user) == 0){

        // Handling a new connection using get_connection
        pipe(pipe_SERVER_Writing_to_child);
        pipe(pipe_SERVER_reading_from_child);
        pid_t pid = fork();

        if (pid==0){ // child: user
          ssize_t nread;
          close(pipe_SERVER_Writing_to_child[1]);
          close(pipe_SERVER_reading_from_child[0]);
          close(pipe_child_writing_to_user[0]);
          close(pipe_child_reading_from_user[1]);
          fcntl(pipe_child_reading_from_user[0], F_SETFL, fcntl(pipe_child_reading_from_user[0], F_GETFL)| O_NONBLOCK);
          fcntl(pipe_SERVER_Writing_to_child[0], F_SETFL, fcntl(pipe_SERVER_Writing_to_child[0], F_GETFL)| O_NONBLOCK);

          while(1){
            char exit[]= "\\exit";
            if ((nread = read(pipe_child_reading_from_user[0],buf,MAX_MSG)) > -1){
              if(nread == 0){
              write (pipe_SERVER_reading_from_child[1],exit,strlen(exit));
              }
              write (pipe_SERVER_reading_from_child[1],buf,nread);
            }
            if ((nread = read(pipe_SERVER_Writing_to_child[0],buf,MAX_MSG)) > 0){
              write (pipe_child_writing_to_user[1],buf,nread);
            }
            usleep(100);
          }
        }else if(pid > 0){ // Parent: server
          // Server process: Add a new user information into an empty slot
            waitpid(pid,&pidStatus,WNOHANG);
            int  n = find_empty_slot(user_list);
            int name = find_user_index(user_list,user_id);

            fcntl(pipe_SERVER_reading_from_child[0], F_SETFL, fcntl(pipe_SERVER_reading_from_child[0], F_GETFL)| O_NONBLOCK);
            close(pipe_SERVER_Writing_to_child[0]);
            close(pipe_SERVER_reading_from_child[1]);
            close(pipe_child_reading_from_user[0]);
            close(pipe_child_reading_from_user[1]);
            close(pipe_child_writing_to_user[0]);
            close(pipe_child_writing_to_user[1]);

            if (n == -1 ){
              char error[] = "Max User Reached";
              write(pipe_SERVER_Writing_to_child[1],error,strlen(error));

            }else if ((n != -1) && (name != -1) ){
              char error[] = "This User ID is Taken";
              write(pipe_SERVER_Writing_to_child[1],error,strlen(error));

            }else{
              add_user(n,user_list,pid,user_id,pipe_SERVER_Writing_to_child[1],pipe_SERVER_reading_from_child[0]);
              printf("%s has joined the server\n", user_id);
              print_prompt("admin");
            }
          }
      }
      // poll child processes and handle user commands
      for (int i=0;i<MAX_USER;i++){
        if (user_list[i].m_status == SLOT_EMPTY)
          continue;
        else{
          if ((nbytes = read(user_list[i].m_fd_to_server,buf,MAX_MSG)) > 0){
            if (buf[strlen(buf)-1] == '\n') // changes last element to
                buf[strlen(buf)-1] = '\0';
            if (strlen(buf) >= 1){
              switch (get_command_type(buf)){  // checks which command is used
                case 0:
                  list_users(i,user_list);
                  print_prompt("admin");
                  break;
                case 2:
                  send_p2p_msg(i,user_list,buf);
                  break;
                case 3:
                  write(user_list[i].m_fd_to_user,seg,strlen(seg));
                  cleanup_user(i,user_list);
                  break;
                case 4:
                  printf("%s has left the chat\n",user_list[i].m_user_id);
                  kick_user(i , user_list);
                  print_prompt("admin");
                  break;
                default:
                  broadcast_msg(user_list, buf, user_list[i].m_user_id);
                  strlen(buf);
                  memset(buf,0,strlen(buf));
                  break;
                }
              }
            }
          }
        }
        	// Poll stdin (input from the terminal) and handle admnistrative command
        if ((nbytes = read(0,buf,MAX_MSG)) > 0){
          print_prompt("admin");
          if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';
          if (strlen(buf) >= 1){
                switch (get_command_type(buf)){ // checks which comand is used
                  case 0:
                    list_users(-1,user_list);
                    print_prompt("admin");
                    break;
                  case 1:
                    kick_init(buf,user_list);
                    break;
                  case 4:
                    kill_and_cleanup_users(user_list);
                    printf("Session has ended\n");
                    exit(-1);
                    break;
                  default:
                    broadcast_msg(user_list, buf, "admin");
                    break;
                }
              }
            }
        memset(buf, 0, MAX_MSG * sizeof(char));  // clear buffer
        usleep(100);
  }

  return(0);
	}


/* --------------------End of the main function ----------------------------------------*/
