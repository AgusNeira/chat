# Protocol details

Each block (piece of data) sent will have a 4-byte header (may change in future versions).\
The first two bytes in this header are common to every type of block,\
while the two remaining are left blank, and used by some blocks only.\
These last two bytes are referred to as extra bytes. All blocks must be less than 256 bytes
in size.

All of the header bytes are defined in the header protocol.hpp.

+ Opcode (1 byte) (`Header::opcode`)
+ Block size (1 byte) (Message maximum size is 256 characters) (`Header::blockSize`)\

Optional extra bytes might be added according to the opcode

## Opcodes:

### Messages

+ 0x00: global message (`Opcodes::GlobalMessage`)\
	Extra bytes:
	- Sender (`Header::message::sender`)
+ 0x01: private message (`Opcodes::PrivateMessage`)\
	Extra bytes:
	- Sender (`Header::message::sender`)
	- Receiver (`Header::message::receiver`)

### Users

A username may be of up to 20 characters, and user IDs are always 1-byte long (max 256 users)

+ 0x02: user identification (`Opcodes::userID`)\
	When opened, the client program will prompt for a username.
	Upon connection, the username will be sent to the server under this opcode
	As a response, the server will send a message with the same opcode, providing
	a user ID
+ 0x03: user tables (`Opcodes::userTables`)\
	When a new client joins, the server wil inform him of all the users currently logged in
	As this might occupy more than 256 characters (the buffer size), the users are sent
	in many blocks.\
	Extra bytes:\
	- Remaining blocks (not including the current one) (`Header::userTables::blocksLeft`)
	- User blocks in this block (`Header::userTables::users`)\

	The content of this messages is divided in user blocks, each containing:
	- User ID (`UserBlock::userID`)
	- User block size (max 20) (`UserBlock::size`)
	- Username (_n_ bytes) (`UserBlock::username`)\

	The user tables are sent after the client has been identified (after `USR_ID` exchange)
