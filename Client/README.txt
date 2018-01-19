Program Specs for CSC 3350 Assignment:

Your application will consist of two separate programs, SERVER.exe and CLIENT.exe.  Both programs will use sockets configured to use the TCP/IP protocols.  
The user will manually start the server and then start the client to establish a socket communication linkage. 

When the SERVER starts, it will bind itself to listen for connection requests on a system selected serverport-number. It will print out its server HostName and server-port-number to the user, 
and then wait for an incoming client connection request.  When a client initiates a connection request, the SERVER will create and connect a server socket to communicate with the client.   
 
Once the SERVER is running, the CLIENT program can be started using command-line parameters to specify the server-port-number and (optional) HostName.  The CLIENT must create a client socket 
and initiate a connection request to the SERVER, thereby establishing a socket-based network communication path. 
 
Once the connection is established, both the SERVER and the CLIENT applications must engage in a "talk" session.  Any message buffers received through the socket must be displayed to the user, 
prefixed on the screen with "<".  Any lines of text typed in by the user must be displayed to the user locally, prefixed on the screen with ">", and then sent to the other process as a message-buffer 
using the socket. 
 
If the user of either the SERVER or the CLIENT types "exit", then both applications should quit talking and terminate.  If either the user types "exit", or if a "exit" message is received via the 
socket, then terminate the talk. 

For Server.exe:
Once a socket and a HostName object are setup, the SERVER uses the bind( … ) call to link the socket to the HostName object.  Then the SERVER must print out on the screen a short message with 
it's serverHostName and the port-number that it is listening to.  Use getsockname( … ) with the socket and the HostName parameters to update the HostName fields with the specific information from 
the binding operation.  The sin_port field contains the Namespace_portnumber that is in use.   
 
Then the SERVER registers itself as a listener, and uses the accept ( … ) function to wait for a connection request from a client.  Note that accept( … ) returns a new socket handle that is to be used 
in communicating with the client application.  For this lab, the original socket (that was used for the listening) is not needed after connecting with a client, and should be closed. 
 
