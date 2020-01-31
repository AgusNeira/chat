#include <cstdlib>
#include <poll.h>

struct PollManager {
public:
	PollManager(){
		this->fds = new pollfd[5];
		this->size = 5;
		this->count = 0;
	}
	
	void add_fd(int newfd){
		if (this->count == this->size){
			this->size += 5;
			struct pollfds *newarr = new pollfd[this->size];

			for (int i = 0; i < this->count; i++){
				newarr[i] = this->fds[i];
			}

			delete[] this->fds;
			this->fds = newarr;
		}

		this->fds[this->count].fd = newfd;
		this->fds[this->count].events = POLLIN;
		this->count++;
	}

	void del_fd(int i){
		if (i >= this->count) return;

		this->fds[i] = this->fds[this->count -1];
		this->count--;
	}

	struct pollfd * operator[](int i){
		if (i >= this->size) return nullptr;
		return this->fds[i];
	}

	int poll(){
		return poll(this->fds, this->count, -1);
	}
	
private:
	struct pollfd *fds;
	int count;
	int size;
}
