#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUF_SIZE 1024

void print_error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
/*
void http_resp(char *msg, char *f_type, char *f_name)
{
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg), "HTTP/1.1 200 OK\r\n""Content-Type: %s; charset=UTF-8\r\n\r\n"
								"<!DOCTYPE html>\r\n"
								"<html><head><title>GET Response</title>\r\n"
								"<img src=\"%s\"></html>\r\n", f_type, f_name);
}*/

void get_url_components(char *url, char *f_name, char *f_type)
{
	char type[5];
	int ptr = strlen(url);
	int i = 0, j = 0;
/*
	memset(f_name, 0, sizeof(f_name));
	memset(f_type, 0, sizeof(f_type));
	memset(type, 0, sizeof(type));*/

	while (url[ptr] != '.') ptr--;

	for (i = ptr + 1; i < strlen(url); i++)
    {
        type[j] = url[i];
		j++;
    }
	
	while (url[ptr] != '/') ptr--;

	j = 0;
	for (i = ptr + 1; i < strlen(url); i++)
	{
		f_name[j] = url[i];
		j++;
	}

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


int main(int argc, char **argv)
{
	struct sockaddr_in server, client;
	char r_buffer[MAX_BUF_SIZE];
	char method[10];
	char url[30];
	char path[35];
	char version[10];
	char f_type[30];
	char f_name[30];
	char *error_msg = "<!DOCTYPE html><html><title>500 Internal Server Error</title>""<pre><h1></h1></pre>""</html>\r\n";
	ssize_t length;
	int sfd, cfd;
	int child_pid = 0;
	int rbytes;
	int option = 1;

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

		if (child_pid < 0)
			print_error("Server: fork\n");

		if (child_pid > 0)
		{
			close(cfd);
			waitpid(0, NULL, WNOHANG);
		}

		if (child_pid == 0)
		{
			memset(r_buffer, 0, sizeof(r_buffer));
			memset(method, 0, sizeof(method));
			memset(url, 0, sizeof(url));
			memset(path, 0, sizeof(path));
			memset(version, 0, sizeof(version));
			memset(f_type, 0, sizeof(f_type));
			memset(f_name, 0, sizeof(f_name));

			printf("Created a child process --> %d\n", child_pid);
			
			rbytes = recv(cfd, r_buffer, MAX_BUF_SIZE, 0);
			if (rbytes < 0) {
				send(cfd, error_msg, strlen(error_msg), 0);
				close(cfd);
				exit(EXIT_FAILURE);
			}

			sscanf(r_buffer, "%s %s %s", method, url, version);

			printf("Method: %s\nPath: %s\nVersion: %s\n", method, url, version);

			if ((strcmp(method, "GET") != 0) && (strcmp(method, "POST") != 0) && (strcmp(method, "HEAD") != 0))
			{
				send(cfd, error_msg, strlen(error_msg), 0);
                close(cfd);
                exit(EXIT_FAILURE);
			}

			if ((strcmp(version, "HTTP/1.1") != 0) && (strcmp(version, "HTTP/1.0") != 0))
			{
				send(cfd, error_msg, strlen(error_msg), 0);
                close(cfd);
                exit(EXIT_FAILURE);
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
			printf("Path -> %s\n", path);
			get_url_components(path, f_name, f_type);

			printf("Path -> %s	file_name -> %s	file_type -> %s\n",path, f_name, f_type); 

/*			if (access(path, R_OK) != 0)
			{
				send(cfd, error_msg, strlen(error_msg), 0);
                close(cfd);
                exit(EXIT_FAILURE);
			}
*/

/*
			switch(method)
			{
				case "GET"
*/


		}
		
		//close(cfd);
	}

	close(sfd);
	exit(EXIT_SUCCESS);
}
