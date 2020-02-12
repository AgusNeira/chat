#include "endpoint.hpp"
#include "buffer.hpp"
#include "pollmanager.hpp"


class Client : protected Endpoint, protected Buffer {
public:
	Client(const char *host, const char *port, int *status): Endpoint(host, port, status), Buffer(256) {};

private:
	PollManager pollmg;
};
	
