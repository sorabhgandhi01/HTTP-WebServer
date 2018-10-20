#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUF_SIZE 1024
#define t_out 10

void print_error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void http_ok_resp(char *msg, ssize_t f_size, char *f_type, char *conn_status)
{
	snprintf(msg, 512, "HTTP/1.1 200 OK\r\n""Content-Type: %s\r\n""Connection: %s\r\n""Content-Length: %ld\r\n\r\n", f_type, conn_status, f_size);
}

void http_error_resp(char *msg, char *conn_status, int c_size)
{
	if (conn_status != NULL)
		snprintf(msg, 512, "HTTP/1.1 500 Internal Server Error\r\n""Content-Type: html\r\n""Connection: %s\r\n""Content-Length: %d\r\n\r\n", conn_status, c_size);
	else
		snprintf(msg, 512, "HTTP/1.1 500 Internal Server Error\r\n""Content-Type: html\r\n""Content-Length: %d\r\n\r\n", c_size);
}

void get_url_components(char *url, char *f_name, char *f_type)
{
	char type[5];
	int ptr = strlen(url);
	int i = 0, j = 0;
	
	memset(type, 0, sizeof(type));

	while (url[ptr] != '.') ptr--;

	for (i = ptr + 1; i < strlen(url); i++)
    {
        type[j] = url[i];
		j++;
    }
	type[j] = '\0';
	
	while (url[ptr] != '/') ptr--;

	j = 0;
	for (i = ptr + 1; i < strlen(url); i++)
	{
		f_name[j] = url[i];
		j++;
	}

	if (strcmp(type, "js") == 0) {
			strcpy(f_type, "css/");
            strcat(f_type, type);
	}
	else
{
	switch (type[0])
	{
		case 'h':
			strcpy(f_type, "text/");
			strcat(f_type, type);
			break;

		case 't':
			strcpy(f_type, "text/");
            strcat(f_type, type);
            break;

		case 'p':
			strcpy(f_type, "image/");
            strcat(f_type, type);
            break;

		case 'g':
			strcpy(f_type, "image/");
            strcat(f_type, type);
            break;

		case 'j':
			strcpy(f_type, "image/");
            strcat(f_type, type);
            break;

		case 'c':
			strcpy(f_type, "text/");
            strcat(f_type, type);
            break;

		/*case "js":
			strcpy(f_type, "css/");
            strcat(f_type, type);
            break;*/

		default:
			strcpy(f_type, "text/");
            strcat(f_type, "html");
            break;
	}
}
}


int main(int argc, char **argv)
{
	struct sockaddr_in server, client;
	struct stat st;
	struct timeval timeout;
	
	char r_buffer[MAX_BUF_SIZE];		//Stores the CLient Response
	char s_buffer[512];					//Stores HTTP_Ok_Response
	char e_buffer[512];					//Stores HTTP_Error_Response
	char method[10];					//Stores the Requested HTTP Method
	char url[30];						//Stores the Requested File Path
	char path[35];
	char version[10];					//Stores the HTTP Version
	char f_type[30];					//Stores the type of file requested
	char f_name[30];					//Stores the file name requested

	/*HTTP Error Message Content in HTML Format*/
	char error_msg[] = "<!DOCTYPE html><html><title>Invalid Request</title>""<pre><h1>500 Internal Server Error</h1></pre>""</html>\r\n";
	
	ssize_t length;
	ssize_t f_size;

	int sfd, cfd;
	int child_pid = 0;
	int rbytes;
	int option = 1;
	int c_size;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
		print_error("Server: socket");

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

	if (bind(sfd, (struct sockaddr *) &server, sizeof(server)) == -1)
		print_error("Server: bind");

	if (listen(sfd, 5) == -1)
		print_error("Server: listen");

	for (;;)
	{	
		length = sizeof(client);

		cfd = accept(sfd, (struct sockaddr *) &client, (socklen_t *) &length);
		if (cfd == -1)
		{
			perror("Server: accept\n");
			continue;
		}

		printf("Accepted a new connection = %d\n", cfd);

		child_pid = fork();
		printf("Created a child process --> %d\n", child_pid);
		
		if (child_pid < 0)
			print_error("Server: fork\n");

		if (child_pid > 0)
		{
			close(cfd);
			waitpid(0, NULL, WNOHANG);
		}

		if (child_pid == 0)
		{
			printf("In the child process\n");
			
			while ((recv(cfd, r_buffer, MAX_BUF_SIZE, 0)) > 0)
			{
			
				memset(s_buffer, 0, sizeof(s_buffer));
				memset(method, 0, sizeof(method));
				memset(url, 0, sizeof(url));
				memset(path, 0, sizeof(path));
				memset(version, 0, sizeof(version));
				memset(f_type, 0, sizeof(f_type));
				memset(f_name, 0, sizeof(f_name));

				sscanf(r_buffer, "%s %s %s", method, url, version);
				//printf("%s\n\n", r_buffer);

				char *conn_status = strstr(r_buffer, "Connection: keep-alive");

				if (conn_status)
				{
					//printf("-----------> Config timeout\n");
					timeout.tv_sec = t_out;
					setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof(struct timeval));
				}
				else {
					timeout.tv_sec = 0;
					setsockopt(cfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof(struct timeval));
				}

				printf("Method: %s\nPath: %s\nVersion: %s\n", method, url, version);

				/*Check for inappropriate method*/
				if ((strcmp(method, "GET") != 0) && (strcmp(method, "POST") != 0) && (strcmp(method, "HEAD") != 0))
				{
					c_size = strlen(error_msg);

					if (conn_status)
						http_error_resp(e_buffer, "keep-alive", c_size);
					else
						http_error_resp(e_buffer, "Close", c_size);

					send(cfd, e_buffer, strlen(e_buffer), 0);
					send(cfd, error_msg, strlen(error_msg), 0);
					
					if (conn_status) {
						continue;
					}
					else {
						printf("Closing Socket %d\n", client.sin_port);
						close(cfd);
						exit(0);
					}
				}

					/*Check for inappropriate version*/
				if ((strcmp(version, "HTTP/1.1") != 0) && (strcmp(version, "HTTP/1.0") != 0))
				{
					c_size = strlen(error_msg);

					if (conn_status)
						http_error_resp(e_buffer, "keep-alive", c_size);
					else
						http_error_resp(e_buffer, "Close", c_size);

					send(cfd, e_buffer, strlen(e_buffer), 0);
					send(cfd, error_msg, strlen(error_msg), 0);
					
					if (conn_status) {
						continue;
					}
					else {
						printf("Closing Socket %d\n", client.sin_port);
						close(cfd);
						exit(0);
					}
				}

				strcpy(path, "./www");

				if (strcmp(url,"/") == 0)
				{
					strcat(path, "/index.html");
				}
				else
				{
					strcat(path, url);
				}

				/*Check for inappropriate URL*/
				if (access(path, F_OK) != 0)
				{
					c_size = strlen(error_msg);

					if (conn_status)
						http_error_resp(e_buffer, "keep-alive", c_size);
					else
						http_error_resp(e_buffer, "Close", c_size);

					send(cfd, e_buffer, strlen(e_buffer), 0);
					send(cfd, error_msg, strlen(error_msg), 0);
					
					if (conn_status) {
						continue;
					}
					else {
						printf("Closing Socket %d\n", client.sin_port);
						close(cfd);
						exit(0);
					}
				}
				
				/*--------------------------------GET Request------------------------------*/
				if (strcmp(method, "GET") == 0)
				{
					memset(s_buffer, 0, sizeof(s_buffer));
					get_url_components(path, f_name, f_type);
					stat(path, &st);
					f_size = st.st_size;
					printf("Path -> %s    file_name -> %s file_type -> %s	fs -> %ld\n",path, f_name, f_type, f_size); 

					if (conn_status)
						http_ok_resp(s_buffer, f_size, f_type, "keep-alive");
					else
						http_ok_resp(s_buffer, f_size, f_type, "Close");
					
					printf("s_buffer --> %s\n", s_buffer);
					send(cfd, s_buffer, strlen(s_buffer), 0);

					int nsize;
					char *buffer = (char *) malloc(f_size*sizeof(char));
					FILE *fptr;

					fptr = fopen(path, "r");
					nsize = fread(buffer, 1, f_size, fptr);

					send(cfd, buffer, nsize, 0);
					
					fclose(fptr);
					free(buffer);

					if (conn_status == NULL) {
						printf("Closing Socket %d\n", client.sin_port);
						close(cfd);
						exit(0);
					}
				}
			}
			printf("Connection Close due to timeout for socket %d\n", client.sin_port);
		}
		close(cfd);
	}

	close(sfd);
	exit(EXIT_SUCCESS);
}
