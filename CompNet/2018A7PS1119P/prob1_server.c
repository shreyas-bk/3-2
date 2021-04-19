//NAME:SHREYAS BHAT KERA
//ID:2018A7PS1119P
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFERSIZE 512
# define PORT 8873
# define PDR .1
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
int main(void)
{
		struct sockaddr_in server, client;
		int s, i;
		int server_len = sizeof(client), recv_len;
		int flag = 1;
		PACKET packet;
		PACKET ack;
		if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
			die("socket");
		}
		memset((char * ) & server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(s, (struct sockaddr * ) & server, sizeof(server)) == -1)
		{
			die("bind");
		}
		int server_state = 0;
		FILE *file_pointer;     
		file_pointer = fopen("output.txt", "w"); 
		if(file_pointer == NULL)
		{
			printf("cannot open file");
			return 1;
		}
		while (1)
		{
			switch (server_state)
			{
				case 0:
					fflush(stdout);
					if ((recv_len = recvfrom(s, & packet, BUFFERSIZE, 0, (struct sockaddr * ) & client, & server_len)) == -1)
					{
						die("recvfrom()");
					}
					if (rand()%100>100*(1-PDR))
					{
						flag = 0;
						printf("DROP DATA: Seq.no. %d of size %d\n",packet.sq_num,packet.size_in_bytes);
					}
					else
						flag = 1;
					if (packet.sq_num == 0 && flag)
					{
						printf("RCVD DATA: seq. no. %d of size %d\n", packet.sq_num, packet.size_in_bytes);
						fprintf(file_pointer,"%s",packet.data);
						ack.sq_num = 0;
						ack.ack_or_data = 0;
						if (sendto(s, &ack, recv_len, 0, (struct sockaddr * ) & client, server_len) == -1)
						{
								die("sendto()");
						}
						printf("SENT ACK: for PKT with seq.no. %d\n",ack.sq_num);
						if (packet.last_packet == 1)
						{
							close(s);
							fclose(file_pointer);
							return 0;
						}
						server_state = 1;
						break;
					}
					break; 
				case 1:
					fflush(stdout);
					if ((recv_len = recvfrom(s, & packet, BUFFERSIZE, 0, (struct sockaddr * ) & client, & server_len)) == -1)
					{
						die("recvfrom()");
					}
					if (rand()%100>100*(1-PDR))
					{
						flag = 0;
						printf("DROP DATA: Seq.no. %d of size %d\n",packet.sq_num,packet.size_in_bytes);
					}
					else
						flag = 1;
					if (packet.sq_num == 1 && flag)
					{
						printf("RCVD DATA: seq. no. %d of size %d\n", packet.sq_num, packet.size_in_bytes);
						fprintf(file_pointer,"%s",packet.data);
						ack.sq_num = 1;
						ack.ack_or_data = 0;
						if (sendto(s, & ack, recv_len, 0, (struct sockaddr * ) & client, server_len) == -1)
						{
								die("sendto()");
						}
						printf("SENT ACK: for PKT with seq.no. %d\n",ack.sq_num);
						if (packet.last_packet == 1)
						{
							close(s);
							fclose(file_pointer);
							return 0;
						}
						server_state = 0;
						break;
					} 
					server_state = 1;
					break;
				}
		}
		close(s);
		fclose(file_pointer);
		return 0;
}
