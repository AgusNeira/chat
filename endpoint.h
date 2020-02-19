#ifndef ENDPOINT_
#define ENDPOINT_

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

#include "bufferhandle.h"
#include "protocol.h"

/*
 * Endpoint class
 *
 * An endpoint is a wrapper for a socket, implementing functions
 * for connection and message exchanging.
 *
 * Each Endpoint is closely related to a BufferHandle, in order to
 * communicate directly and avoid the creation of temporaries.
 * Each client Endpoint owns a BufferHandle, since it is the only
 * class needing one (supposedly).
 * Meanwhile, server Endpoints are managed within an EndpointSet,
 * which holds a series of BufferHandles to be requested by each
 * endpoint. This allows the server to save memory, by declaring
 * only a few buffers for all the endpoints.
 *
 */

enum EndpointType { Server, Client };

template <EndpointType et>
class Endpoint {
public:
	struct ConnectionFailed { const std::string msg; };
	enum { BufferUnavailable = 1 };

	/*
	 * Constructor for server endpoints
	 * the socket file descriptor is usually retrieved from
	 * an accept() call of a Listener
	 */
	template <typename dummy = void, typename = std::enable_if_t<et == Server, dummy>>
	Endpoint(int sockfd): sockfd(sockfd), handle_(nullptr) {}

	/*
	 * Constructor for client endpoints
	 *
	 * Establishes a connection with a specified
	 * host and port.
	 * Throws ConnectionFailed with an error message
	 * if the connection can't be established.
	 */
	template <typename dummy = void, typename = std::enable_if_t<et == Client, dummy>>
	Endpoint(const char *host, const char *port){
		struct addrinfo hints, *res, *p;
		int rv;

		memset(&hints, 0 , sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		
		if ((rv = getaddrinfo(host, port, &hints, &res)) != 0)
			throw ConnectionFailed{std::string("getaddrinfo: ") + std::string(gai_strerror(rv))};

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
		
		if (p == NULL) throw ConnectionFailed{"socket or bind"};

		freeaddrinfo(res);

		handle_ = new BufferHandle;
	}

	~Endpoint(){
		if (et == Client) delete handle_;
	}

	explicit operator int(){
		return this->sockfd;
	}
	/*
	 * receive()
	 *
	 * return 0 on succes, -1 on failure.
	 *
	 * Will throw BufferUnavailable, if the
	 * Endpoint is of Server type and attach()
	 * hasn't been called.
	 *
	 * uses the available BufferHandle and
	 * loads it with a protocol-compliant
	 * block from the socket. (see protocol.md)
	 */
	int receive(){
		if (handle_ == nullptr) throw BufferUnavailable;

		int temp_len = recv(this->sockfd, handle_->buffer(), Header::size, 0);
		int total;

		if (temp_len != Header::size) return -1;
		total = Header::size;

		temp_len = handle_[Header::blockSize];
		temp_len -= Header::size;
		
		while (temp_len > 0){
			int n = recv(this->sockfd, handle_->buffer() + total, temp_len, 0);

			if (n == -1) return -1;

			temp_len -= n;
			total += n;
		}

		return total;
	}

	/*
	 * send()
	 *
	 * returns 0 on success, -1 on failure.
	 *
	 * Will throw BufferUnavailable if the endpoint
	 * is of Server type and attach() hasn't been called.
	 * 
	 * Uses the data within the available BufferHandle,
	 * processes it as if it were protocol-compliant
	 * in order to get the size of the block to send,
	 * and sends it through the socket.
	 * 
	 */
	int send(){
		if (handle_ == nullptr) throw BufferUnavailable;

		int total = 0;
		int bytesleft = handle_[Header::blockSize];
		int n;
		
		while (bytesleft > 0){
			n = ::send(this->sockfd, handle_->buffer() + total, bytesleft, 0);

			if (n == -1) return -1;
			total += n;
			bytesleft -= n;
		}

		return  0;
	}
	
	/*
	 * stdin_to_body()
	 *
	 * throws BufferUnavailable if the endpoint is of
	 * type Server and attach() hasn't been called.
	 *
	 * Uses the standard input strem defined in <iostream>
	 * and the available BufferHandle to load the user input into
	 * the body of the buffer.
	 */
	void stdin_to_body(){
		std::cin.getline(body(), 256 - Header::size);
		handle_[Header::blockSize] = std::strlen(body()) + Header::size;
	}

	/*
	 * attach(BufferHandle *buf);
	 *
	 * buf - The BufferHandle to attach
	 *
	 * Given a pointer to a BufferHandle, this function locks the BufferHandle's
	 * std::mutex and copies the pointer to this->handle_
	 * Blocks the current thread until buf->mtx is unlocked.
	 * If there is a BufferHandle already attached to this endpoint, the
	 * function does nothing.
	 *
	 * This function is only enabled for Server Endpoints, as Client ones
	 * use the same BufferHandle throughout their life.
	 */
	template <typename Dummy = void>
	auto attach(BufferHandle *buf) -> std::enable_if_t<et == Server> {
		if (handle_ != nullptr) return;

		buf->mtx.lock();
		handle_ = buf;
	}

	/*
	 * try_attach(BufferHandle *buf)
	 *
	 * buf - The BufferHandle to try to attach
	 *
	 * Given a pointer to a BufferHandle, this function tries to lock
	 * its std::mutex. If succesfull, it copies the pointer to this->handle_
	 * and returns true. Otherwise returns false.
	 * If there already is a BufferHandle attached to this endpoint,
	 * the function does nothing.
	 * 
	 * This function is only enabled for Server Endpoints, as Client ones
	 * use the same BufferHandle throughout their life.
	 */
	template <typename Dummy = bool>
	auto try_attach(BufferHandle *buf) -> std::enable_if_t<et == Server> {
		if (handle_ != nullptr) return;

		if (buf->mtx.try_lock()){
			handle_ = buf;
			return true;
		} else return false;
	}

	/*
	 * release()
	 *
	 * This function unlocks the std::mutex of the attached BufferHandle,
	 * and removes the pointer to it. If no BufferHandle is attached,
	 * it does nothing.
	 * This function is only enabled for Server Endpoints, as Client ones
	 * use the same BufferHandle throughout their life.
	 */
	template <typename Dummy = void>
	auto release() -> std::enable_if_t<et == Server> {
		if (handle_  == nullptr) return;
		handle_->mtx.unlock();
		handle_ = nullptr;
	}

	char * 			buffer() 							{ return handle_->data; }
	char * 			body() 								{ return handle_ ->data + Header::size; }

	const char&		operator[](int i)					{ return handle_->data[i];	}
	const char&		get(unsigned int i) const 			{ return handle_->data[i];	}
	void 			set(unsigned int i, char c)			{ handle_->data[i] = c;	}

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
	BufferHandle *handle_;
};

#endif
