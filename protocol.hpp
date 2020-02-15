#ifndef PROTOCOL_
#define PROTOCOL_

// General header bytes
#define OP_CODE	0
#define MSG_SIZE 1

// Bytes for messages
// (receiver byte is ignored for global messages)
#define MSG_SENDER 2
#define MSG_RECEIVER 3

// Bytes for USR_TABLES
#define PAGE_CURR 2
#define PAGES_TOTAL	3

// End of header
#define BODY 4
#define HEADER_SIZE 4

enum class OPCodes : unsigned char {
	MSG_GLOBAL = 0x00,
	MSG_PRIVATE = 0x01,
	USR_ID = 0x02,
	USR_TABLES = 0x03
};

#endif
