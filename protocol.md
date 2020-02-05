### Protocol details

Each message sent is preceded by the following 2-byte header:

+ Opcode (1 byte)
+ Block size (1 byte) (Message maximum size is 256 characters)

Optional extra bytes might be added according to the opcode

## Opcodes:

# Messages

+ 0x00: global message (`MSG_GLOBAL`)
	Extra bytes:
	- Sender (1 byte)
+ 0x01: private message (`MSG_PRIVATE`)
	Extra bytes:
	- Sender (1 nyte)
	- Receiver (1 byte)

# Users

+ 0x02: user identification (`USR_ID`)
	When opened, the client program will prompt for a username.
	Upon connection, the username will be sent to the server under this opcode
	As a response, the server will send a message with the same opcode, providing
	a user ID
+ 0x03: user tables (`USR_TABLES`)
	When a new client joins, the server wil inform him of all the users currently logged in
	As this might occupy more than 256 characters, two bytes are used for paging.
	Extra bytes:
	- Current page (1 byte)
	- Total pages (1 byte)
	Each user will be sent as:
	- User ID (1 byte)
	- Username size (1 byte) (max 16)
	- Username (_n_ bytes)
