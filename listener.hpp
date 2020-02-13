#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

class Listener {
public:
	Listener(char *port, int backlog, int *status){
		int yes = 1;
		int ret;

		struct addrinfo hints, *res, *p;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		if((ret = getaddrinfo(NULL, PORT, &hints, &res)) != 0){
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
			*status = -1;
			return;
		}

		for (p = res; p != NULL; p = p->ai_next){
			this->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (listener < 0){
				continue;
			}

			setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

			if(bind(this->sockfd, p->ai_addr, p->ai_addrlen) < 0){
				close(this->sockfd);
				continue;
			}
			break;
		}

		if (p == NULL){
			*status = -1;
			return;
		}

		if (listen(this->sockfd, 10) == -1){
			*status = -1;
			return;
		}

		freeaddrinfo(res);
		*status = 0;
	}

	~Listener();

	int accept(){
		return accept(this->sockfd);
	}

	explicit operator int(){ return this->sockfd; }

	static get_in_addr(struct sockaddr *sa){

		if (sa->sa_family == AF_INET){
			return &(((struct sockaddr_in *) sa)->sin_addr);
		} else if (sa->sa_family == AF_INET6){
			return &(((struct sockaddr_in6 *) sa)->sin6_addr);
		}
		return nullptr;
	}

private:
	int sockfd;
}
