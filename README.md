INTRODUCTION
------------
	This is an HTTP WebServer design which can process and service the HTTP request of the client. 
	This WebServer is designed to handle mutiple client connections parallely. In this project, I 
	have considered client as web browser. This web server implementation is tested with Google 
	Chrome and Firefox web browser. Also, I have used pipelining methodology to serice the  
	persistant connection of the client continously.

BUILD and RUN STEPS
-------------------
	This project is built and tested on Ubuntu 16.04 OS. This project only supports Linux based 
	development environment.

	Note:- The build system for this project uses GNU MAKE utility.

	PROJECT BUILD STEPS
	===================

	1.) Go to the project folder, cd [FolderName]/

	2.) Run make

	RUN STEPS
	=========

	./server [PORT NUMBER]

IMPLEMENTATION SUMMARY
----------------------

	This HTTP server services the GET and POST request of the client.

	GET Request
	===========
	
		In GET request, the server returns the requested file. If no file is requested then
		the server returns the default file. The server processes the HTTP request and sends 
		back the response in HTTP format.

	POST Request
	============

		In POST request, the client requests a file with some data to the server. The server 
		then processes the request and then sends back the requested file with the data 
		appended in the beginning of the file.

	Error Cases
	===========

	HTTP METHOD ->	This server only supports GET, POST and HEAD method. Any other method 
					requested by a client will result in "500 Internal Server Error".

	HTTP VERSION -> This server only supports HTTP/1.1 and HTTP/1.0 . Any other version
                    requested by a client will result in "500 Internal Server Error".

	HTTP URL	->	This server supports all the files present in the "www" folder. Files can be
					inserted or removed from the "www" folder without any modification in the
					code piece. Any other version requested by a client will result in "500 
					Internal Server Error".

	HTTP CONNECTION -> This server supports Connection type - "keep-alive" and "Close"

TESTING
-------

	1.) Post functionality can be tested by running test_post.sh script on one terminal
		and server on another terminal

	2.) Pipelining functionality can be tested by running test_conn.sh script on one terminal
		and server on another terminal.

	3.) On Web broser, this can be tested using url --> localhost:[Port Number]/
