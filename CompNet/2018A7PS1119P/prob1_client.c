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
#define PORT 8873
#define TIME 2
typedef struct packet
{
	int size_in_bytes;
	int sq_num;
	int last_packet;
	int ack_or_data;
	char data[BUFFERSIZE];
}
PACKET;
struct sockaddr_in server;
int server_len = sizeof(server);
PACKET packet, ack;
bool flag = false;
int s;
void die(char *err)
{
	perror(err);
	exit(1);
}
void handle_alarm(int signal)
{
	if (flag == false)
	{
		if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr * ) & server, server_len) == -1) 
			die("sendto()");
		printf("RESENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
		alarm(TIME);
	}
}
int main(void)
{
	signal(SIGALRM, handle_alarm);
	FILE *file_pointer;     
	file_pointer = fopen("input.txt", "r"); 
	if(file_pointer == NULL)
	{
		printf("cannot open file");
		return 1;
	}
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	memset((char *) &server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	int client_state = 0;
	while (1)
	{
		switch (client_state)
		{
		case 0:
			if(fgets(packet.data, BUFFERSIZE, file_pointer))
			{
				packet.sq_num = 0;
				packet.ack_or_data = 1;
				int check = fgetc(file_pointer);
				if (check==EOF)
				{
					packet.last_packet = 1;
				}
				else
				{
					packet.last_packet = 0;
				}
				ungetc(check, file_pointer);
				packet.size_in_bytes = strlen(packet.data);
				if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr *) & server, server_len) == -1)
				{
					die("sendto()");
				}
				printf("SENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
				client_state = 1;
				flag = false; 
				alarm(TIME);
				break;
			}
			else
			{
				close(s);
				fclose(file_pointer);
				return 0;
			}
		case 1:
			if (recvfrom(s, &ack, sizeof(ack), 0, (struct sockaddr *) & server, &server_len) == -1)
			{
				die("recvfrom()");
			}
			if (ack.sq_num == 0)
			{
				printf("RCVD ACK: for PKT with Seq. No. %d\n", ack.sq_num);
				client_state = 2;
				flag = true;
				break;
			}	
		case 2:
			if(fgets(packet.data, BUFFERSIZE, file_pointer))
			{
				packet.sq_num = 1;
				packet.ack_or_data = 1;
				int check = fgetc(file_pointer);
				if (check==EOF)
				{
					packet.last_packet = 1;
				}
				else
				{
					packet.last_packet = 0;
				}
				ungetc(check, file_pointer);
				packet.size_in_bytes = strlen(packet.data);
				if (sendto(s, & packet, sizeof(packet), 0, (struct sockaddr * ) & server, server_len) == -1)
				{
					die("sendto()");
				}
				printf("SENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
				client_state = 3;
				flag = false; 
				alarm(TIME);
				break;
			}
			else
			{
				close(s);
				fclose(file_pointer);
				return 0;
			}
		case 3:
			if (recvfrom(s, & ack, sizeof(ack), 0, (struct sockaddr * ) & server, & server_len) == -1)
			{
				die("recvfrom()");
			}
			if (ack.sq_num == 1)
			{
				printf("RCVD ACK: for PKT with Seq. No. %d\n", ack.sq_num);
				client_state = 0;
				flag = true;
				break;
			}
		}
	}
	close(s);
	fclose(file_pointer);
	return 0;
}
