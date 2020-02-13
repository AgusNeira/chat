#include "endpoint.hpp"
#include "buffer.hpp"
#include "pollmanager.hpp"

#include <iostream>
#include <cstring>

class Client : protected Endpoint {
public:
	Client(const char *host, const char *port): Endpoint(host, port, status), pollmg(2) {
		if (*(this->status) != 0) {
			std::cerr<<"Couldn't open socket to "<<host<<" on "<<port<<std::endl;
			exit(1);
		}

		pollmg.add_fd(this->sockfd);
		pollmg.add_fd(STDIN);

		for(;;){
			int revs = pollmg.do_poll();

			if (revs > 0){
			 	for (int i = 0; i < 2; i++){
					if (pollmg[i].revents & POLLIN){
						if (pollmg[i].fd == 0){ // stdin
							std::cin.getline(buffer, 256);
							buflen = std::strlen(buffer);
						} else { // socket

						}
					}
				}
			}
		}
	};

private:
	char buffer[256];
	int buflen;

	int *status;
	PollManager pollmg;
};
	
