#include "buffer.hpp"

int Buffer::put(const char *data, int *len){
	const std::lock_guard<std::mutex> lock(this->buff_mutex);

	if (size_ - this->count() < *len) {
		*len = 0;
		return -1; // Full buffer
	}

	int total = 0;
	for (; total < *len; total++){
		*(buff_ + head_) = data[total];
		head_ = (head_ + 1) % size_;
	}

	return 0;
}
int Buffer::put(const std::string &str){
	const std::lock_guard<std::mutex> lock(this->buff_mutex);
	int len = str.length();

	if (size_ - this->count() < len) return -1; // Full buffer

	if (size_ - head_ < len){
		int first_len = size_ - head_;
		str.copy(buff_ + head_, first_len);
		head_ = 0;
		str.copy(buff_ + head_, len - first_len, first_len);
		head_ += len - first_len;
	} else {
		str.copy((buff_ + head_), len);
		head_ += len;
	}

	return 0;
}

int Buffer::get(char *data, int *len){
	const std::lock_guard<std::mutex> lock(this->buff_mutex);

	if (this->count() < *len){
		*len = 0;
		return -1; // Empty buffer
	}
		
	int total = 0;
	for (; total < *len; total++){

		data[total] = *(buff_ + tail_);
		tail_ = (tail_ + 1) % size_;
	}

	return 0;
}

int Buffer::get(std::string &str, int len){
	const std::lock_guard<std::mutex> lock(this->buff_mutex);

	if (this->count() < len) return -1;
	
	if (size_ - head_ < len){
		int first_len = size_ - head_;
		str.assign(buff_ + tail_, first_len);
		tail_ = 0;
		str.append(buff_ + tail_, len - first_len);
		tail_ += len - first_len;
	} else {
		str.assign(buff_ + tail_, len);
		tail_ += len;
	}

	return 0;
}
