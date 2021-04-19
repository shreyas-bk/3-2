//NAME:SHREYAS BHAT KERA
//ID:2018A7PS1119P
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#define BUFFERSIZE 512
#define PDR .1
#define TIME 2
void die(char *err)
{
		perror(err);
		exit(1);
}
typedef struct packet
{
	int size_in_bytes;
	int sq_num;
	int last_packet;
	int ack_or_data;
	char data[BUFFERSIZE];
}
PACKET;
PACKET packet,ack;
bool flag = false;
int s,recv_len;
struct sockaddr_in self, other;
int self_len = sizeof(self);
void handle_alarm(int signal)
{
	if (flag == false)
	{
		if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr * ) & self, self_len) == -1) 
			die("sendto()");
		printf("RESENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
		alarm(TIME);
	}
}
int main(int argc, char** argv)
{
}
