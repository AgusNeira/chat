## Protocol details

Each message sent is preceded by the following 2-byte header:

+ Type of operation (1 byte)
+ Message size (1 byte)

# Operation types:

+ 0x00: global message
+ 0x01: private message
	- This operation implies that the next byte after the header will be the receiver of the
	message
+ 0x02: user identification: used when a user connects, along with a username
