
#include <mutex>
#include <vector>

#include "endpoint.hpp"

class EndpointSet {
public:
	struct Buffer {
		Buffer(int fd) : mtx() {}

		char data[256];
		std::mutex mtx;
	};

	EndpointSet() : ends_() {}

	void add_fd(int fd){
		ends_.emplace(fd);
	}
	void remove_fd(int fd){
	}
private:
	EndpointSet::Buffer buffs_[10];
	std::vector<Endpoint> ends_;
};
