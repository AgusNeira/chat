#include <iostream>
#include <string>
#include <poll.h>

#include "buffer.hpp"

int main(){
	std::string line;
	char choice;
	int len;
	int status;
	struct pollfd pfd;
	pfd.fd = 0;
	pfd.events = POLLIN;

	Buffer buff(256);

	printf("Usage:\n");
	printf("\t+<string>\tadds a string to buffer\n");
	printf("\t-<n>\tgets n chars from buffer\n");
	printf("\ts\t prints size, tail and head of the buffer\n");
	printf("\tq\texits\n");

	for(;;){
		int rtor = poll(&pfd, 1, -1);

		if (rtor == 1){
			choice = std::cin.get();
			std::getline(std::cin, line);
			len = line.length();

			switch (choice){
				case 'q':
					return 0;
				case '+':
					std::cout<<"Appending "<< len <<"-size string to buffer\n";
					status = buff.put(line);
					std::cout<<"Status: "<< status <<"\n";
					break;
				case '-':
					char *endp;
					len = std::stoi(line, nullptr, 10);
					line.clear();
					std::cout<<"Getting "<< len <<" chars from buffer\n";
					status = buff.get(line, len);
					std::cout<<"Status: "<< status <<"\n";
					std::cout<<"String: "<< line <<"\n";
					break;
				case 's':
					std::cout<<"Buffer size: "<< buff.count() <<"\n";
			}
			line.clear();
		}
	}

	return 0;
}
