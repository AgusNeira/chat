#include <iostream>
#include <cstring>

#include "bufferedendpoint.hpp"
#include "pollmanager.hpp"
#include "protocol.hpp"
#include "user.hpp"

/*
 * Client prototype
 *
 * Protocol compliant
 *
 * Global messages
 * User identification
 *
 */

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

	char username[20];
	int user_id;

	BufferedEndpoint endpoint(argv[1], argv[2], &status);

	if (status != 0){
		std::cerr<<"Couldn't open endpoint to "<<argv[1]<<" in port "<<argv[2]<<std::endl;
		exit(1);
	}

	PollManager pollmg(2);
	pollmg.add_fd((int) endpoint);
	pollmg.add_fd(0); // stdin

	UserTables usrTables;

	/*
	 * Log-in to server
	 */

	std::cout<<"Welcome!\nWhat's your name?: ";
	endpoint.set(OP_CODE, USR_ID);
	endpoint.stdin_to_body();
	endpoint.send();

	/*
	 * Get user id
	 */

	endpoint.receive();
	if (endpoint.get(OP_CODE) != USR_ID || endpoint.get(MSG_SIZE) != 5){
		std::cout<<"Oops... something went wrong\n";
		exit(1);
	}
	user_id = endpoint.get(BODY); // It's only 1 byte

	/*
	 * Get user tables
	 */
	endpoint.receive();
	if (endpoint.get(OP_CODE != USR_TABLES) {
		std::cout<<"Oops... something went wrong\n";
		exit(1);
	}
	

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

void load_user_tables(Endpoint *ep, UserTables *tables){}

