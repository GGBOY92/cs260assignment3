Readme File for CS260 Assignment 3: File Sharing
Date: 11/26/2010
Team: Steven Liss, Michael Travaglione

Instructions:
==============

Server port and IP are read from config.txt
Client specifies which port it's listening on for UDP transfers.
Client must also specify directory of files to share on the server and where to receive transfers.

config file has the following format:
<port number>
<IP address> 
<client UDP port>
<send path>
<receive path>

Need to edit the IP field to whatever the server IP is. 
This is displayed when the server is executed.


Notes:
======
*Config file needs to be one directory up from the executable. File must be in this location.*

type '/help' to view all valid commands when running the client application. all commands start with a forward slash.
type '/prog' to view the current progress all current file transfers. This gives a time estimate.

Format for send/receive paths:

example of valid paths - 

<send path> C:\\file_share\\send\\
<receive path> C:\\file_share\\recv\\
                                   ^^
                                   trailing backslashes are needed

Server displays it's local IP address and reads in only the port number.

WARNING: Send and receive paths MUST exist.


Building Project:
=================

Visual Studio Solution includes multiple projects. 
Please set the file_client and file_server as the startup projects and build them to generate the executables for the two applications.

Program follows guidelines given for assignment:
================================================
1. Clients connect to a server via TCP connections. When connected, 
clients are able to specify what files are available to be transfered.
The command for this is '/show'

2. List of files on the server is a combination of lists of files each client advertises that
it has. We assume there are no duplicate names as specified in the assignment description.
Every time a client joins, it sends it's list of files which are then added to the master list.

3. Any client may request to download any file, including their own. When this occurs, the server notifies the 
client that owns the file to be downloaded that someone wants to download that file. The server 
then indicates to the client that is trying to download the file the IP and Port 
of the client that has the file. The acutal file transfer is done through UDP sockets.

4. If the server crashes while a file transfer is in progress, that file transfer is still be able to complete 
successfully, however no additional transfers will be possible.

5. When a client disconnects or crashes for some reason, their files are removed from the list. 

Extra Credit:
=============

We have attempted the following extra credit:

1. (5 points) ability to receive files from multiple hosts simultaneously 
i.e. Host A can receive a file from both Host B and Host C at the same time.

2. (10 points) Any number of hosts can send any number of files to any other 
number of hosts at the same time. I should be able to send 20 files from Host A 
to Host B at the same time while Host B also receives 30 files from Host C while 
sending 10 files to Host A. Note: if you get credit for this, you will automatically 
also get credit for #1 also, so this is effectively worth a full letter grade and a half.

3. (5 points) Some sort of progress indicator that will tell me how long it will take to 
download the remainder of a file. It does not need to be exactly accurate but if you advertise 
that I will need one additional minute to receive the rest of a file, it should take about one 
minute to get it.

=========================================================================================

Thanks for reading.