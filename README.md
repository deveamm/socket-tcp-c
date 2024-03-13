The function of this program system is very easy and begins by having a client and a server, both using the C language and with communication through Sockets. The “servidor.c” program will only be responsible for receiving instructions from “cliente.c”, which will only be 4, these being:

1. Send a file:
It consists of the client sending a file to the server and it will classify it according to its extension.
• The categories are: text (.txt), programs (.c), images (.jpg, .bmp, .gif, etc.), and others (the other extensions).
• When the server reads the file name, it will store it in a folder named after the category.

2. List sent files:
• It consists of the client requesting that the server show them the files they have at that moment by category or all.

3. Operations:
You must allow the Client to choose one of these operations and the server will send the response
• Send a string Uppercase to lowercase: a string is sent and converted accordingly
• Send a string Uppercase to lowercase: a string is sent and converted accordingly
• Add N numbers

4. History:
The program must keep a record of the connection history, that is, the IP of the machine that connected, the name of the user, and including date and time.
