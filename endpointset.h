
#include <algorithm>
#include <mutex>
#include <tuple>
#include <vector>

#include "endpoint.hpp"

class EndpointSet {
public:
	typedef std::tuple

	struct BufferHandle {
		BufferHandle(int fd) : mtx() {}

		char data[256];
		std::mutex mtx;
	};

	EndpointSet() : ends_() {}

	void add_fd(int fd){
		ends_.emplace(fd);
	}
	void remove_fd(int fd){
		
		ends_.erase(it);
	}

	bool lock_buffer(int fd){

	const  find(int fd) const {
		return std::find_if(ends_.begin(), ends_.end(), [](const Endpoint& other){
				return (int) other == fd;
		});
	}
private:
	EndpointSet::BufferHandle buffs_[10];
	std::vector<std::tuple<Endpoint, BufferHandle*>> ends_;
};
