//This implementation handles network byte ordering (big-endian to little-endian*** host conversion*****)
//and iterates through the variable-length message blocks inside a single UDP packet.

class MarketDataFeedHandler {
public:
    void HandleNetworkPacket(const uint8_t* buffer, size_t bytesReceived) {
        if (bytesReceived < sizeof(MoldUDP64Header)) {
            std::cerr << "Packet dropped: Short MoldUDP64 header.\n";
            return;
        }

        // 1. Parse MoldUDP64 Header
        auto* moldHeader = reinterpret_cast<const MoldUDP64Header*>(buffer);
        
        // Convert network byte order to host byte order
        uint64_t startSeq = __builtin_bswap64(moldHeader->SequenceNumber);
        uint16_t msgCount = ntohs(moldHeader->MessageCount);

        std::cout << "Received Packet | Session: " << std::string(moldHeader->Session, 10)
                  << " | Start Seq: " << startSeq 
                  << " | Messages: " << msgCount << "\n";

        // Handle Heartbeats (MessageCount == 0)
        if (msgCount == 0) {
            return; 
        }

        // 2. Iterate through MoldUDP64 blocks containing ITCH messages
        size_t offset = sizeof(MoldUDP64Header);
        
        for (uint16_t i = 0; i < msgCount; ++i) {
            if (offset + sizeof(MoldUDP64Block) > bytesReceived) {
                std::cerr << "Parsing truncated: Buffer overflow on block header.\n";
                break;
            }

            auto* block = reinterpret_cast<const MoldUDP64Block*>(buffer + offset);
            uint16_t itchLength = ntohs(block->MessageLength);
            offset += sizeof(MoldUDP64Block);

            if (offset + itchLength > bytesReceived) {
                std::cerr << "Parsing truncated: Buffer overflow on ITCH payload.\n";
                break;
            }

            // 3. Dispatch raw payload to ITCH Parser
            ParseITCHMessage(buffer + offset, itchLength, startSeq + i);
            offset += itchLength;
        }
    }

private:
    void ParseITCHMessage(const uint8_t* itchBuffer, uint16_t length, uint64_t absoluteSeq) {
        if (length < sizeof(ITCHCommonHeader)) return;

        auto* header = reinterpret_cast<const ITCHCommonHeader*>(itchBuffer);
        
        switch (header->MessageType) {
            case 'A': { // Add Order Message
                if (length < sizeof(ITCHAddOrderMessage)) return;
                auto* msg = reinterpret_cast<const ITCHAddOrderMessage*>(itchBuffer);
                
                // Extract stock ticker safely
                std::string ticker(msg->Stock, 8);
                ticker.erase(ticker.find_last_not_of(" ") + 1); // Clean padding

                uint32_t shares = ntohl(msg->Shares);
                double price = ntohl(msg->Price) / 10000.0; // 4 decimal places

                std::cout << "  [ITCH Seq " << absoluteSeq << "] Add Order -> " 
                          << msg->BuySellIndicator << " " << shares << " " 
                          << ticker << " @ $" << price << "\n";
                break;
            }
            
            case 'S':
                std::cout << "  [ITCH Seq " << absoluteSeq << "] System Event Message\n";
                break;

            default:
                std::cout << "  [ITCH Seq " << absoluteSeq << "] Unhandled Message Type: '" 
                          << header->MessageType << "'\n";
                break;
        }
    }
};