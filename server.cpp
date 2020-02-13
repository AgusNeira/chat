#include <iostream>
#include <mutex>

#include "pollmanager.hpp"
#include "listener.hpp"
#include "protocol.hpp"

struct safeBuffer {
	char data[256];
	std::mutex mx; // for mutex use mx.lock() or mx.try_lock()
} buffers[10];

int main(int argc, char *argv[]){
	if (argc != 2){
		std::cout<<"Usage: "<<argv[0]<<" <port>\n";
		exit(0);
	}

	int status;

	Listener listener(argv[1], 5, &status);
	if (status != 0){
		std::cerr<<"Couldn't open listener\n";
		exit(1);
	}

	PollManager pollmg(5);
	pollmg.add_fd((int) listener);
	
}
