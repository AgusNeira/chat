#ifndef BUFFERED_ENDPOINT_
#define BUFFERED_ENDPOINT_

#include "endpoint.hpp"
#include "protocol.hpp"

#include <iostream>
#include <cstring>

class BufferedEndpoint : protected Endpoint {
public:
	BufferedEndpoint(const char *host, const char *port, int *status)
		: Endpoint(host, port, status) {}

	void receive(){
		this->recv_msg(this->buffer_);
	}
	void send(){
		this->send_msg(this->buffer_);
	}

	void stdin_to_body(){
		std::cin.getline(buffer_ + BODY, 256 - HEADER_SIZE);
		buffer_[MSG_SIZE] = std::strlen(buffer_ + BODY);
	}

	void set(int i, unsigned char op){ buffer_[i] = op;	}
	unsigned char get(int i){ return buffer_[i]; }

	char * buffer(){ return this->buffer_; }

protected:
	char buffer_[256];
	const int size = 256;
};

#endif
