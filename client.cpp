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

int main(int argc, char *argv[]){
	if (argc != 3){
		std::cout<<"Usage: "<<argv[0]<<" <host> <port>\n";
		exit(0);
	}

	int len = 256;
	int status;

	char buff[256];
	Endpoint endpoint(argv[1], argv[2], &status);

	if (status != 0){
		std::cerr<<"Couldn't open endpoint to "<<argv[1]<<" in port "<<argv[2]<<std::endl;
		exit(1);
	}

	PollManager pollmg(2);
	pollmg.add_fd((int) endpoint);
	pollmg.add_fd(0); // stdin

	for(;;){
		int revs = pollmg.do_poll();

		if (revs > 0){
			for (int i = 0; i < 2; i++){
				if (pollmg[i]->revents & POLLIN){
					if (pollmg[i]->fd == 0){
						std::cin.getline((buff + 3), 253); // First 3 bytes will be used as header
						len = strlen(buff);
						printf("About to send %d bytes\n", len);

						len = endpoint.sendall(buff, &len);
						if (len < 0) printf("Failed to send\n");
						std::memset(buff, 0, 256);
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

