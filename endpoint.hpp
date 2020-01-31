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
	
private:
	int sockfd;
}
