#include <iostream>
#include <cstring>

#include "endpoint.hpp"
#include "pollmanager.hpp"

int main(){
	const char host[] = "localhost";
	const char port[] = "9034";
	int len = 256;
	int status;

	char buff[256];
	Endpoint endpoint(host, port, &status);
	PollManager pollmg(2);

	if (status != 0){
		std::cerr<<"Couldn't open endpoint to "<<host<<" in port "<<port<<std::endl;
		exit(1);
	}

	pollmg.add_fd((int) endpoint);
	pollmg.add_fd(0); // stdin

	for(;;){
		int revs = pollmg.do_poll();

		if (revs > 0){
			for (int i = 0; i < 2; i++){
				if (pollmg[i]->revents & POLLIN){
					if (pollmg[i]->fd == 0){
						std::cin.getline(buff, 256);
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

