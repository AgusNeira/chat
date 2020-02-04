
/*
 * Circular buffer
 *
 * - Dynamically allocated
 * - Size checking
 * 
 */

//#define PARTIAL_RW

#include <mutex>

class Buffer {
public:
	Buffer(const int size): size_(size) {
		this->buff_ = new char[size];
		this->head_ = 0;
		this->tail_ = 0;
	}
	~Buffer(){
		delete[] this->buff_;
	}

/**************** 
 * Input/Output *
 ***************/

	int put(const char *, int *);
	int put(const std::string &);
	int get(char *, int *);
	int get(std::string &, int);

	bool is_full(){
		return (head_ + 1) % size_ == tail_;
	}
	bool is_empty(){
		return head_ == tail_;
	}
	int count(){
		if (head_ >= tail_) return head_ - tail_;
		else return head_ + size_ - tail_;
	}

private:
	std::mutex buff_mutex;

	char *buff_;
	int head_;
	int tail_;

	const int size_;
};
