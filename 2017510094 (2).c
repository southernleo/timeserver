#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT_NUMBER 60000

int main(int argc, char *argv[]) {
int sock, client_sock, c, read_size;
struct sockaddr_in server, client;
char client_message[2000];

// Create socket
sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == -1) {
printf("Could not create socket");
}
puts("Socket created");

// Prepare the sockaddr_in structure
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(PORT_NUMBER);

// Bind
if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
perror("bind failed. Error");
return 1;
}
puts("bind done");

// Listen
listen(sock, 3);

// Accept and incoming connection
puts("Waiting for incoming connections...");
c = sizeof(struct sockaddr_in);

// Accept connection from an incoming client
client_sock = accept(sock, (struct sockaddr *) &client, (socklen_t *) &c);
if (client_sock < 0) {
perror("accept failed");
return 1;
}
puts("Connection accepted");

// Receive a message from client
while ((read_size = recv(client_sock, client_message, 2000, 0)) > 0) {
// Send the response to the client
char response[2000];
memset(response, 0, sizeof(response)); // Clear the response buffer
if (strcmp(client_message, "GET_TIME\n") == 0) {
  FILE *fp;
  char output[1000];
  fp = popen("date +'%T'", "r");
  fgets(output, 1000, fp);
  pclose(fp);
  sprintf(response, "Time: %s", output);
} else if (strcmp(client_message, "GET_DATE\n") == 0) {
  FILE *fp;
  char output[1000];
  fp = popen("date +'%D'", "r");
  fgets(output, 1000, fp);
  pclose(fp);
  sprintf(response, "Date: %s", output);
} else if (strcmp(client_message, "GET_TIME_DATE\n") == 0) {
  FILE *fp;
  char output[1000];
  fp = popen("date +'%T, %D'", "r");
  fgets(output, 1000, fp);
  pclose(fp);
  sprintf(response, "Time and date: %s", output);
}
else if (strcmp(client_message, "GET_TIME_ZONE\n") == 0) {
FILE *fp;
char output[1000];
fp = popen("date +'%Z'", "r");
fgets(output, 1000, fp);
pclose(fp);
sprintf(response, "Time zone: %s", output);
} else if (strcmp(client_message, "GET_DAY_OF_WEEK\n") == 0) {
FILE *fp;
char output[1000];
fp = popen("date +'%A'", "r");
fgets(output, 1000, fp);
pclose(fp);
sprintf(response, "Day of the week: %s", output);
} else if (strcmp(client_message, "CLOSE_SERVER\n") == 0) {
sprintf(response, "GOOD BYE");
} else {
sprintf(response, "INCORRECT REQUEST");
}

// Send the response back to the client
write(client_sock, response, strlen(response));

// Clear the message buffer
memset(client_message, 0, sizeof(client_message));

// Check if the client sent the CLOSE_SERVER command
if (strcmp(client_message, "CLOSE_SERVER\n") == 0) {
break;
}
}

if (read_size == 0) {
puts("Client disconnected");
fflush(stdout);
} else if (read_size == -1) {
perror("recv failed");
}

// Close the socket and clean up
close(client_sock);
close(sock);

return 0;
}
 

