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
	int port_self = strtol(argv[1],NULL,10);
  	int port_other = strtol(argv[2],NULL,10);
	int current_state = 0;
	if (port_self<port_other)
	{
		current_state = 1;
	}
	printf("INput File:");
	char input_file[20];
	scanf("%s",input_file);
	printf("Output File:");
	char output_file[20];
	scanf("%s",output_file);
	FILE *fp_input;     
	fp_input = fopen(input_file, "r"); 
	if(fp_input == NULL)
	{
		printf("cannot open input file");
		return 1;
	}
	FILE *fp_output;     
	fp_output = fopen(output_file, "w"); 
	if(fp_output == NULL)
	{
		printf("cannot open output file");
		return 1;
	}
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	memset((char * ) & self, 0, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(port_self);
	self.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(s, (struct sockaddr * ) &self, sizeof(self)) == -1)
	{
		die("bind");
	}
	memset((char * ) & other, 0, sizeof(other));
	other.sin_family = AF_INET;
	other.sin_port = htons(port_other);
	other.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	bool flag_sending = true, flag_receiving = true;
	int sender_state = 0;
	int receiver_state = 0;
	while(flag_sending == true && flag_receiving == true)
	{
		switch (current_state)
		{
			case 0:
				if(flag_sending == false)
				{
					current_state==1;
					continue;
				}
				signal(SIGALRM, handle_alarm);
				//start
				switch (sender_state)
				{
				case 0:
					if(fgets(packet.data, BUFFERSIZE, fp_input))
					{
						packet.sq_num = 0;
						packet.ack_or_data = 1;
						int check = fgetc(fp_input);
						if (check==EOF)
						{
							packet.last_packet = 1;
						}
						else
						{
							packet.last_packet = 0;
						}
						ungetc(check, fp_input);
						packet.size_in_bytes = strlen(packet.data);
						if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr *) & self, self_len) == -1)
						{
							die("sendto()");
						}
						printf("SENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
						sender_state = 1;
						flag = false; 
						alarm(TIME);
						break;
					}
					else
					{
						flag_sending = false;
					}
				case 1:
					if (recvfrom(s, &ack, sizeof(ack), 0, (struct sockaddr *) & self, &self_len) == -1)
					{
						die("recvfrom()");
					}
					if (ack.sq_num == 0)
					{
						printf("RCVD ACK: for PKT with Seq. No. %d\n", ack.sq_num);
						sender_state = 2;
						flag = true;
						break;
					}	
				case 2:
					if(fgets(packet.data, BUFFERSIZE, fp_input))
					{
						packet.sq_num = 1;
						packet.ack_or_data = 1;
						int check = fgetc(fp_input);
						if (check==EOF)
						{
							packet.last_packet = 1;
						}
						else
						{
							packet.last_packet = 0;
						}
						ungetc(check, fp_input);
						packet.size_in_bytes = strlen(packet.data);
						if (sendto(s, & packet, sizeof(packet), 0, (struct sockaddr * ) & self, self_len) == -1)
						{
							die("sendto()");
						}
						printf("SENT DATA: Seq. no. %d  of size %d\n", packet.sq_num, packet.size_in_bytes);
						sender_state = 3;
						flag = false; 
						alarm(TIME);
						break;
					}
					else
					{
						flag_sending = false;
					}
				case 3:
					if (recvfrom(s, & ack, sizeof(ack), 0, (struct sockaddr * ) & self, & self_len) == -1)
					{
						die("recvfrom()");
					}
					if (ack.sq_num == 1)
					{
						printf("RCVD ACK: for PKT with Seq. No. %d\n", ack.sq_num);
						sender_state = 0;
						flag = true;
						break;
					}
				}
				//end
				break;
			case 1:
				if(flag_receiving == false)
				{
					current_state==0;
					continue;
				}
				//start
				switch (receiver_state)
				{
					case 0:
						fflush(stdout);
						if ((recv_len = recvfrom(s, & packet, BUFFERSIZE, 0, (struct sockaddr * ) & other, & self_len)) == -1)
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
							fprintf(fp_output,"%s",packet.data);
							ack.sq_num = 0;
							ack.ack_or_data = 0;
							if (sendto(s, &ack, recv_len, 0, (struct sockaddr * ) & other, self_len) == -1)
							{
									die("sendto()");
							}
							printf("SENT ACK: for PKT with seq.no. %d\n",ack.sq_num);
							if (packet.last_packet == 1)
							{
								flag_receiving = false;
							}
							receiver_state = 1;
							break;
						}
						break; 
					case 1:
						fflush(stdout);
						if ((recv_len = recvfrom(s, & packet, BUFFERSIZE, 0, (struct sockaddr * ) & other, & self_len)) == -1)
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
							fprintf(fp_output,"%s",packet.data);
							ack.sq_num = 1;
							ack.ack_or_data = 0;
							if (sendto(s, & ack, recv_len, 0, (struct sockaddr * ) & other, self_len) == -1)
							{
									die("sendto()");
							}
							printf("SENT ACK: for PKT with seq.no. %d\n",ack.sq_num);
							if (packet.last_packet == 1)
							{
								flag_receiving = false;
								
							}
							receiver_state = 0;
							break;
						} 
						receiver_state = 1;
						break;
					}
				//end
				break;
		}
	}
	close(s);
	fclose(fp_output);
	fclose(fp_input);
	return 0;
}

