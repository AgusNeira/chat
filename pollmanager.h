#include <cstdlib>
#include <poll.h>

struct PollManager {
public:
	PollManager(int s): size_(s), init_size(s) {
		this->fds = new pollfd[s];
		this->count = 0;
	}
	
	void add_fd(int newfd){
		if (this->count == this->size_){
			this->size_ += this->init_size;
			struct pollfd *newarr = new pollfd[this->size_];

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
		if (i >= this->size_) return nullptr;
		return &(this->fds[i]);
	}

	operator struct pollfd*(){
		return this->fds;
	}

	int do_poll(){
		return poll(this->fds, this->count, -1);
	}

	int size(){ return this->size_; }
	
private:
	struct pollfd *fds;
	int count;
	int size_;
	const int init_size;
};
