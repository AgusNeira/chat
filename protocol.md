# Protocol details

Each message sent will have a 4-byte header (may change in future versions).\
The first two bytes in this header are common to every type of message,\
while the two remaining are left blank, and used by some messages only.\
These last two bytes are referred to as extra bytes.

+ Opcode (1 byte) (`OP_CODE` macro)
+ Block size (1 byte) (Message maximum size is 256 characters) (`MSG_SIZE` macro)\

Optional extra bytes might be added according to the opcode

## Opcodes:

### Messages

+ 0x00: global message (`MSG_GLOBAL`)\
	Extra bytes:
	- Sender (1 byte)
+ 0x01: private message (`MSG_PRIVATE`)\
	Extra bytes:
	- Sender (1 byte)
	- Receiver (1 byte)

### Users

A username may be of up to 20 characters, and user IDs are always 1-byte long (max 256 users)

+ 0x02: user identification (`USR_ID`)\
	When opened, the client program will prompt for a username.
	Upon connection, the username will be sent to the server under this opcode
	As a response, the server will send a message with the same opcode, providing
	a user ID
+ 0x03: user tables (`USR_TABLES`)\
	When a new client joins, the server wil inform him of all the users currently logged in
	As this might occupy more than 256 characters, two bytes are used for paging.\
	Extra bytes:\
	- Current page (1 byte)
	- Total pages (1 byte)\
	Each user will be sent as:
	- User ID (1 byte)
	- Username size (1 byte) (max 16)
	- Username (_n_ bytes)\

	The user tables are sent after the client has been identified (after `USR_ID` exchange)
