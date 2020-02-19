
#include <mutex>

#include "protocol.h"

struct BufferHandle {
	BufferHandle() : mtx() {}

	char * 			buffer()							{ return data; }
	char *			body()								{ return data + Header::size; }
	
	const char&		operator[](int i)					{ return data[i];	}
	const char&		get(unsigned int i) const 			{ return data[i];	}
	void 			set(unsigned int i, char c)			{ data[i] = c;	}
	
	char data[256];
	std::mutex mtx;
};
