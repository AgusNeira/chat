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

class Endoint {
public:
	Enpoint(int sockfd): this->sockfd(sockfd) {};

	Endpoint(const char *host, const char *port, int *status){
		struct addrinfo hints, *res, *p;
		int rv;

		memset(&hints, 0 , sizeof hints);
		hints.ai_familiy = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		if ((rv = getaddrinfo(host, port, &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			*status = -1;
			return;
		}
		
		for (p = res; p != NULL, p = p->ai_next){

			this->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (this->sockfd == -1){
				perror("client: socket");
				continue;
			}

			if (connect(this->sockfd, p->ai_addr, p->ai_addrlen) == -1){
				perror("client: connect");
				close(this->sockfd);
				continue;
			}
			break;
		}
		
		if (p == NULL) {
			fprintf(stderr, "client: failed to connect");
			*status = -1;
			return;
		}

		freeaddrinfo(res);
		*status = 0;
	}

	int send(char *buff, int len){
		return send(this->sockfd, buff, len, 0);
	}
	int recv(char *buff, int len){
		return recv(this->sockfd, buff, len, 0);
	}

	int sendall(char* buff, int *len){
		int total = 0;
		int bytesleft = *len;
		int n;
		
		while (total < *len){
			n = send(this->sockfd, buff + total, bytesleft, 0);

			if (n == -1) break;
			total += n;
			bytesleft -= n;
		}

		*len = total;

		return n == -1 ? -1 : 0;
	}

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
