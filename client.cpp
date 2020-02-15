#include <iostream>
#include <cstring>

#include "bufferedendpoint.h"
#include "pollmanager.h"
#include "protocol.h"
#include "user.h"

/*
 * Client prototype
 *
 * Protocol compliant
 *
 * Global messages
 * User identification
 *
 */

void loadUserTables(BufferedEndpoint *bep, UserTable* tables);

int main(int argc, char *argv[]){

	/*
	 * Command-line arguments check
	 */

	if (argc != 3){
		std::cout<<"Usage: "<<argv[0]<<" <host> <port>\n";
		exit(0);
	}

	/*
	 * Initialization
	 */

	int len;
	int status;

	char username[20];
	int user_id;

	BufferedEndpoint endpoint(argv[1], argv[2], &status);

	if (status != 0){
		std::cerr<<"Couldn't open endpoint to "<<argv[1]<<" in port "<<argv[2]<<std::endl;
		exit(1);
	}

	PollManager pollmg(2);
	pollmg.add_fd((int) endpoint);
	pollmg.add_fd(0); // stdin

	UserTable users;

	/*
	 * Log-in to server
	 */

	std::cout<<"Welcome!\nWhat's your name?: ";
	endpoint.set(Header::opcode, Opcodes::UserID);
	endpoint.stdin_to_body();
	endpoint.send();

	/*
	 * Get user id
	 */

	endpoint.receive();
	if (endpoint.get(Header::opcode) != Opcodes::UserID || endpoint.get(Header::blockSize) != 5){
		std::cout<<"Oops... something went wrong\n";
		exit(1);
	}
	user_id = *(endpoint.body()); // It's only 1 byte

	/*
	 * Get user tables
	 */
	loadUserTables(&endpoint, &users);	

	for(;;){
		int revs = pollmg.do_poll();

		if (revs > 0){
			for (int i = 0; i < 2; i++){
				if (pollmg[i]->revents & POLLIN){
					if (pollmg[i]->fd == 0){ // STDIN. sends global message
						endpoint.set(Header::opcode, Opcodes::GlobalMessage);
						endpoint.set(Header::message::sender, user_id);
						endpoint.stdin_to_body();
						status = endpoint.send();
						if (status == -1) std::cout<<"Failed to send\n";

					} else {
						endpoint.receive();
						switch (endpoint.get(Header::opcode)){
							case Opcodes::GlobalMessage:
								auto it = users.find(endpoint.get(Header::message::sender));
								std::cout<<"@"<<it->name<<":";
								std::cout.write(endpoint.body(), endpoint.get(Header::size));
								break;
						}
					}
				}
			}
		}
	}

	return 0;
}

void loadUserTables(BufferedEndpoint *bep, UserTable *tables){
	int blocksLeft;
	int usrlen, usrid;
	char *usrname;
	char *iUser;
	
	do {
		bep->receive();
		if (bep->get(Header::opcode) != Opcodes::UserTables) return;

		blocksLeft = bep->get(Header::userTables::blocksLeft);
	
		for (iUser = bep->body(); iUser < bep->end(); iUser += iUser[UserBlock::size]){
			usrid = iUser[UserBlock::userID];
			usrname = &(iUser[UserBlock::username]);
			usrlen = iUser[UserBlock::size] - 2;
	
			tables->add(usrid, usrname, usrlen);
		}
	} while (blocksLeft > 0);
}

