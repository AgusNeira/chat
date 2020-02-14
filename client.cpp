#include <iostream>
#include <cstring>

#include "endpoint.hpp"
#include "pollmanager.hpp"
#include "protocol.hpp"
/*
 * Client prototype
 *
 * Protocol compliant
 *
 * Global messages
 * User identification
 *
 */

int proto_recv(Endpoint *ep, char *buffer, int max);

int main(int argc, char *argv[]){

	/*
	 * Command-line arguments check
	 */

	if (argc != 3){
		std::cout<<"Usage: "<<argv[0]<<" <host> <port>\n";
		exit(0);
	}

	/*
	 * Initialization
	 */

	int len;
	int status;

	char buff[256];
	char username[20];
	int user_id;

	Endpoint endpoint(argv[1], argv[2], &status);

	if (status != 0){
		std::cerr<<"Couldn't open endpoint to "<<argv[1]<<" in port "<<argv[2]<<std::endl;
		exit(1);
	}

	PollManager pollmg(2);
	pollmg.add_fd((int) endpoint);
	pollmg.add_fd(0); // stdin

	/*
	 * Log-in to server
	 */

	std::cout<<"Welcome!\nWhat's your name?: ";
	std::cin.getline(username, 20);

	len = std::strlen(username) + HEADER_SIZE;
	buff[OP_CODE] = USR_ID;
	buff[MSG_SIZE] = len;
	std::strcpy(buff + HEADER_SIZE, username);
	
	endpoint.send_msg(buff);

	/*
	 * Get user id
	 */

	endpoint.recv_msg(buff);
	if (buff[OP_CODE] != USR_ID || buff[MSG_SIZE] != 5){
		std::cout<<"Oops... something went wrong\n";
		exit(1);
	}
	user_id = buff[BODY];

	for(;;){
		int revs = pollmg.do_poll();

		if (revs > 0){
			for (int i = 0; i < 2; i++){
				if (pollmg[i]->revents & POLLIN){
					if (pollmg[i]->fd == 0){ // STDIN. sends global message
						std::cin.getline((buff + BODY), 256 - HEADER_SIZE);
						len = std::strlen(buff + BODY) + HEADER_SIZE;
						buff[OP_CODE] = MSG_GLOBAL;
						buff[MSG_SIZE] = len;
						buff[MSG_SENDER] = user_id;
						
						status = endpoint.send_msg(buff, &len);
						if (status == -1) printf("Failed to send\n");

					} else {
						len = 256;
						len = endpoint.do_recv(buff, len);
						if (len < 0) printf("Failed to receive\n");
						if (len == 0) {
							printf("Connection closed");
							exit(1);
						}
						printf("Receiving %d bytes\n", len);
						std::cout.write(buff, len);
						std::cout<<std::endl;
						std::memset(buff, 0, 256);
					}
				}
			}
		}
	}

	return 0;
}
