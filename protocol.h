#ifndef PROTOCOL_
#define PROTOCOL_

// Operation codes
enum Opcodes {
	GlobalMessage = 0x00,
	PrivateMessage = 0x01,
	UserID = 0x02,
	UserTables = 0x03
};

/*
 * The following enums specifiy static positions within
 * the messages that comply to the protocol. They are used
 * as indexes to access specific bytes
 */

// General header bytes
namespace Header {
	enum {
		opcode = 0,
		blockSize = 1
	};
	namespace message {
		enum {
			sender = 2,
			receiver = 3
		};
	};
	namespace userTables {
		enum {
			blocksLeft = 2,
			users = 3
		};
	};
	const int size = 4;
};
namespace UserBlock {
	enum {
		userID = 0,
		size = 1,
		username = 2
	};
};

#endif
