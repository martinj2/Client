/*-------------------------------------------------------------------------------------*/
/*|NAME: Jennie Martin****************************************************************|*/
/*|CLASS: CSC 3350 - Tindall**********************************************************|*/
/*|ASSIGNMENT: Lab 6- IPC Socket Talk- Client*****************************************|*/
/*|DESCRIPTION: Connects up to already-runner Server program via a specified port num*|*/
/*|*************and then the applications engage in a talk session via sockets until**|*/
/*|*************exit is specified.****************************************************|*/
/*-------------------------------------------------------------------------------------*/



#include <stdio.h>    //required to use I/O
#include <conio.h>    //required for stdin listening function
#include <string.h>   //required for character string processing
#include <Winsock2.h> //required library to use sockets
#pragma comment(lib,"WS2_32")

#define buffsize 200 //maximum buffer size
#define HOSTNAMELEN 128 //host name length

/*-------------------------------------------------------------------*/
/*|FUNCTION PURPOSE: Engage in a talk session between server and****|*/
/*|******************client.****************************************|/
/*|PARAMETERS: socket***********************************************|*/
/*|RETURNS: nothing*************************************************|*/
/*-------------------------------------------------------------------*/
void talk(SOCKET skt);

int main(int argc, char *argv[]) //uses commandline
{

	WORD wVersionRequested;      //holds version of WSA
	WSADATA wsaData;             //holds WSA data
	SOCKET ConnSkt;              //socket for the client program to connect
	SOCKADDR_IN  theSrvr;        //references the server

	int result = 0, serverPortNumber = 0, error = 0, hostname = 0; //result code from trying to connect programs
										  //server port number
	struct hostent  *host;                //points to host network address table

	char serverHostName[HOSTNAMELEN];     //character string to hold server host name

	//Initialize character buffer
	strcpy_s(serverHostName, HOSTNAMELEN, "");

	if ((argc < 2) && (argc > 3))
	{				 //if there are too many or too few arguments on the commandline 
					 //print the usage directions
		printf("  [Usage] Client.exe <portNumber> [<serverHostName>]\n");
		return(0); //exit the program
	}
	else {
		serverPortNumber = atoi(argv[1]); //set the port number
		if (argc == 3)
		{ //if a server host name was specified 
			strcpy_s(serverHostName, HOSTNAMELEN, argv[2]);
		}
		else //otherwise assume local machine is running server app
		{
			hostname = 1;
		}
	}
	//Intro app code
	printf("#######################################\n");
	printf("TALK-CLIENT: Jennie Martin Version\n");
	printf("#######################################\n");

	// initialize socket package
	wVersionRequested = MAKEWORD(2, 2);  // Version 2.0

	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		fprintf(stderr, "Process failed on WinSock startup\n");
		ExitProcess(0);
	};

	//obtain gethostname and port number of server
	if (hostname != 0) { //if the host name was not given, get it
		result = gethostname(serverHostName, HOSTNAMELEN);

		if (result != 0) {
			printf("The host name could not be gotten.\n");
			exit(1);
		}
	}
	printf("\n Server Host: %s", serverHostName);
	printf("\n Server Port Number: %d", serverPortNumber);
	printf("\n Making Connection Request");

	//get server-host network address table
	host = gethostbyname(serverHostName);
	if (host == NULL)
	{
		fprintf(stderr, "Error getting server-host network address table!\n");
		ExitProcess(0);
	}

	//Issue a connection-request to the server
	ZeroMemory(&theSrvr, sizeof(SOCKADDR_IN));
	theSrvr.sin_family = AF_INET;
	theSrvr.sin_port = htons((u_short)serverPortNumber);
	CopyMemory(&theSrvr.sin_addr,     // Host 
		host->h_addr_list[0], host->h_length);

	// Create a “server connection” socket for the specific connection: ConnSkt 
	ConnSkt = socket(AF_INET, SOCK_STREAM, 0);

	result = connect(ConnSkt, (const SOCKADDR *)&theSrvr, sizeof(theSrvr));
	error = GetLastError();

	//if result is non-zero then an error occured
	if (result != 0)
	{
		fprintf(stderr, "Server connection failed, %d.\n", error);
		ExitProcess(0);
	}
	else {
		printf("...connected\n");
	}


	//Send/Recv data through the ConnSkt
	talk(ConnSkt);

	//When finished, close the ConnSkt
	closesocket(ConnSkt);

	//close the socket package
	WSACleanup(); //release all network socket-related resources


	return(0);
}


//function for "talking" between application
void talk(SOCKET skt)
{
	int flags = 0, nRead = 0, end = 0; // send/receive flags
									   //how many characters come in
									   //sentinal value to end
	char buffer[buffsize]; //line buffer for talk text
	strcpy_s(buffer, buffsize, ""); // initialize line buffer to nothing

	unsigned long  on = 1;
	ioctlsocket(skt, FIONBIO, &on);  // Set NB I/O on

	printf(">");
	while (end != 1) { //while the end flag is false

		if (_kbhit()) //if the user wrote a line from stdin
		{
			memset(buffer, 0, buffsize); // initialize line buffer to nothing
										 // input entire line    . . .    . . . 
			gets_s(buffer, buffsize);
			buffer[strlen(buffer)] = '\n'; //add a newline to the end
										   // Send the buffer
			send(skt, buffer, strlen(buffer), flags); // include newline

													  // remove \n from the end of the line before checking for exit condition
			if (buffer[strlen(buffer) - 1] == '\n') {
				buffer[strlen(buffer) - 1] = 0;
			}

			//if exit encountered
			if ((_stricmp(buffer, "exit\0") == 0))
			{
				//set end flag to true
				end = 1;
			}
			printf(">");
		}

		memset(buffer, 0, buffsize);// initialize line buffer to nothing
									//Receive one line at a time in a fixed-size buffer
		nRead = recv(skt, buffer, buffsize, flags);
		if (nRead > 0)
		{
			printf("\n<");
			// Process line(s) in received buffer contents    . . . . . . 
			buffer[nRead] = '\0';  // stuff NULL into position following  
								   // the actual incoming buffer content
								   // print out the buffer

								   //if there were multiple lines sent, put the < character before each of them
			for (int k = 0; k < (nRead - 1); k++)
			{
				if (buffer[k] == '\n')
				{
					printf("\n<");
				}
				else
					printf("%c", buffer[k]);
			}
			printf("\n");

			// remove \n from the end of the line before exit condition check
			if (buffer[strlen(buffer) - 1] == '\n') {
				buffer[strlen(buffer) - 1] = 0;
			}

			//if exit is encountered from other user
			if (_stricmp(buffer, "exit\0") == 0)
			{
				//set end flag to true
				end = 1;
			}
			printf(">");
		}

		memset(buffer, 0, buffsize); // initialize line buffer to nothing
		Sleep(500); // Be sure to force a timeslice if polling
					// to break up a CPU-bound polling loop
	}
	return;
}