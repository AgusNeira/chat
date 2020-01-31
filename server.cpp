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

#define PORT "2000"
#define POLLFD_SIZE 5

void *get_in_addr(struct sockaddr *sa){

	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *) sa)->sin_addr);
	} else if (sa->sa_family == AF_INET6){
		return &(((struct sockaddr_in6 *) sa)->sin6_addr);
	}
	return nullptr;
}

int get_listener(){
	int listener;
	int yes = 1;
	int status;

	struct addrinfo hints, *res, *p;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}

	for (p = res; p != NULL; p = p->ai_next){
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0){
			continue;
		}

		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if(bind(listener, p->ai_addr, p->ai_addrlen) < 0){
			close(listener);
			continue;
		}
		break;
	}

	if (p == NULL){
		return -1;
	}

	freeaddrinfo(res);

	if (listen(listener, 10) == -1){
		return -1;
	}

	return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size){

	if (*fd_count == *fd_size){
		*fd_size += POLLFD_SIZE;

		*pfds = (struct pollfd *) realloc(*pfds, sizeof(**pfds) * (*fd_size));
	}

	(*pfds)[*fd_count].fd = newfd;
	(*pfds)[*fd_count].events = POLLIN;

	(*fd_count)++;
}

void del_from_pfds(struct pollfd *pfds[], int i, int *fd_count){
	(*pfds)[i] = (*pfds)[*fd_count - 1];
	(*fd_count)--;
}


int main(int argc, char *argv[]){
	
	int listener;

	int newfd;
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;

	char buff[256];

	char remoteIP[INET6_ADDRSTRLEN];

	int fd_count = 0;
	int fd_size = POLLFD_SIZE;
	struct pollfd *pfds = (struct pollfd *) malloc(sizeof *pfds * fd_size);

	listener = get_listener();
	if (listener == -1){
		fprintf(stderr, "error getting listening socket\n");
		exit(1);
	} else
		printf("Server listening on port %s\n", PORT);

	pfds[0].fd = listener;
	pfds[0].events = POLLIN;
	fd_count++;

	for(;;){ // Main loop
		int poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1){
			perror("poll");
			exit(1);
		}

		// Run through all the file descriptors polled
		for (int i = 0; i < fd_count; i++){

			if(pfds[i].revents & POLLIN){

				if (pfds[i].fd == listener){
					newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);

					if (newfd == -1){
						perror("accept");
					} else {
						add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

						printf("server: new connection from %s on socket %d\n",
								inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr),
									remoteIP, INET6_ADDRSTRLEN),
								newfd);
					}
				} else { // If not listener, then regular client
					int nbytes = recv(pfds[i].fd, buff, sizeof buff, 0);
					int sender_fd = pfds[i].fd;

					if (nbytes <= 0){ // Connection closed or recv error
						if (nbytes == 0){
							printf("server: socket %d hung up\n", sender_fd);
						} else {
							perror("recv");
						}

						close(sender_fd);
						del_from_pfds(&pfds, i, &fd_count);

					} else {
						
						for (int j = 0; j < fd_count; j++){
							int dest_fd = pfds[j].fd;

							if(dest_fd != listener && dest_fd != sender_fd){
								if (send(dest_fd, buff, nbytes, 0) == -1){
									perror("send");
								}
							}
						}
					}
				}
			}
		}
	}

	return 0;
}
