#ifndef BUFFERED_ENDPOINT_
#define BUFFERED_ENDPOINT_

#include "endpoint.h"
#include "protocol.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

class BufferedEndpoint : public Endpoint {
public:
	BufferedEndpoint(const char *host, const char *port, int *status)
		: Endpoint(host, port, status) {}

	int receive(){
		int temp_len = recv(this->sockfd, buffer_, Header::size, 0);
		int total;

		if (temp_len != Header::size) return -1;
		total = Header::size;

		temp_len = buffer_[Header::blockSize];
		temp_len -= Header::size;
		
		while (temp_len > 0){
			int n = recv(this->sockfd, buffer_ + total, temp_len, 0);

			if (n == -1) return -1;

			temp_len -= n;
			total += n;
		}

		return total;
	}

	int send(){
		int total = 0;
		int bytesleft = buffer_[Header::blockSize];
		int n;
		
		while (bytesleft > 0){
			n = ::send(this->sockfd, buffer_ + total, bytesleft, 0);

			if (n == -1) return -1;
			total += n;
			bytesleft -= n;
		}

		return  0;
	}

	void stdin_to_body(){
		std::cin.getline(buffer_ + Header::size, 256 - Header::size);
		buffer_[Header::blockSize] = std::strlen(buffer_ + Header::size);
	}

	void 			set(int i, unsigned char op){ buffer_[i] = op;	}
	unsigned char 	get(int i)					{ return buffer_[i]; }
	char * 			body()						{ return this->buffer_ + Header::size; }

	char * 			buffer()					{ return this->buffer_; }
	char * 			end()						{ return this->buffer_ + 255; }

protected:
	char buffer_[256];
	const int size = 256;
};

#endif
