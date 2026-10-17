//Define packet layouts using strict byte alignment. 
//#pragma pack(push,1) ensures the compiler does not add padding*****
//Allows us to cast raw network buffers (pre-allocated block of raw RAM used to hold data temporarily) directly into C++ strcuts.
//Structs are a user-defined data type that allows you to group related variables of different data types under a single name

#include <iostream> //provides standard I/O streams to print text to the console for debugging and logs
#include <cstdint> //Defines exact-wdith integer types like uint32_t to ensure variables are the exact same size on every computer architecture
#include <cstring> //contains raw memory and string manipulation utitilies (memcpy, memset)
#include <vector> //Standard C++ dynamic array container (included for general data management)
#include <netinet/in.h> //A system library that provides tools for network programming, functions like ntohl and ntohs which swap bytes from network order (big/little-endian) 
//Also for byteswapping (ntohs/ntohl)

#pragma pack(push, 1)//Forces the compiler to use 1-byte aligment, eliminating padding. The struct fields pack tightly against each other, matching the raw byte layout coming from the exchange exactly
//CPUs prefer tor ead data frm memory addresses that align with 
//their size (4 byte integer likes to sit at a memory address that is a multiple of four), 
//to achieve this, compilers secretly insert invisible, empty bytes: padding, between fields in a C++ struct.
//Incorrect alignment causes corrupted data.

//***MoldUDP64 Headers***
//Transport layer wrapper: Sits on top of standard UDP (multicast: sends one signle copy of the packet to a specific "multicast IP address," transmits the data once to thousands of firms, ensuring incredibly low latency) to deliver ITCH data
//Adds strict session management and packet numbers
//If a packet is lost in transit, the sequence numbers let a client spot the gap and ask for a retransmission

struct MoldUDP64Header {
    char     Session[10];      // Identifies the market data session
    uint64_t SequenceNumber;   // Sequence number of the first message in this packet
    uint16_t MessageCount;     // Number of ITCH messages contained in this packet
};

struct MoldUDP64Block {
    uint16_t MessageLength;    // Length of the following ITCH message
    // Followed immediately by raw ITCH message payload
};

//Sample ITCH: Dev by Nasdaq, sends full-depth, order-level market data
//Shows every order****** addition, cancellation, modification, and execution
//Market participants use ti to track price levels and roder queues accurately
struct ITCHCommonHeader {
    char MessageType;          // e.g., 'A' for Add Order, 'E' for Order Executed
};

struct ITCHAddOrderMessage {
    char     MessageType;      // 'A'
    uint16_t StockLocate;
    uint16_t TrackingNumber;
    uint64_t Timestamp;        // Nanoseconds since midnight
    uint64_t OrderReferenceNumber;
    char     BuySellIndicator; // 'B' or 'S'
    uint32_t Shares;
    char     Stock[8];         // Right-padded***** with spaces
    uint32_t Price;            // Integer with 4 decimal places implied
};

#pragma pack(pop)

//How they work together:
//Exchanges like Nasdaq package raw ITCH messages inside MoldUDP64 packets.
//Efficient: Using UDP multicast***** means one broadcast reaches thousands of trading firms instantly without extra exchange bandwith.
//Bceause UDP can drop packets, systems par MoldUDP64/ITCH with a TCP*****-based recovery protocol like SoupBinTCP????) to fetch missed historical states****