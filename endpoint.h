#ifndef ENDPOINT_
#define ENDPOINT_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
 * Endpoint class
 *
 * An endpoint is a wrapper for a socket, implementing functions
 * for connection and message exchanging.
 *
 * Endpoints should be used with 256-bytes buffers. If less, the
 * receiving method might crash.
 *
 * For client programs, BufferedEndpoint is implemented, in order
 * to clarify the usage of the objects, and to avoid buffer overflows-
 * For server programs, EndpointSet manages multiple endpoints under
 * a multi-threaded environment, while saving memory on buffers.
 *
 */

class Endpoint {
public:
	Endpoint(int sockfd): sockfd(sockfd) {}
	Endpoint(const char *host, const char *port, int *status){
		struct addrinfo hints, *res, *p;
		int rv;

		memset(&hints, 0 , sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		if ((rv = getaddrinfo(host, port, &hints, &res)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			*status = -1;
			return;
		}
		
		for (p = res; p != NULL; p = p->ai_next){
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
			fprintf(stderr, "client: failed to connect\n");
			*status = -1;
			return;
		}

		freeaddrinfo(res);
		*status = 0;
	}

	explicit operator int(){
		return this->sockfd;
	}

	static void *get_in_addr(struct sockaddr *sa){

		if (sa->sa_family == AF_INET){
			return &(((struct sockaddr_in *) sa)->sin_addr);
		} else if (sa->sa_family == AF_INET6){
			return &(((struct sockaddr_in6 *) sa)->sin6_addr);
		}
		return nullptr;
	}
protected:
	int sockfd;
};

#endif
