#include <stdio.h>
#include <stdint.h>
#include <string.h>

//Map structure of the PCAP (Packet Capture) file using structs
//Define the 24-byte global PCAP header
//Which prefaces the version of PCAP being used
typedef struct pcap_hdr_s {
    uint32_t special_number; //number to identify PCAP
    uint16_t version_major; //major version number
    uint16_t version_minor; //minor version number
    int32_t thiszone; //GMT to local time
    uint32_t sigfigs; //accuracy of timestamps
    uint32_t snaplen; //max length of captured packet
    uint32_t network; //data link type (usually 1 for ethernet)
} __attribute((packed)) pcap_hdr_t;

//Define the 16-byte packet header which precedes every individual packet.
typedef struct pcaprec_hdr_s {
    uint32_t ts_sec; //timestamp seconds
    uint32_t ts_usec; //timestamp microseconds
    uint32_t incl_len; //number of bytes of packet saved in file
    uint32_t orig_len; //actual length of packet on wire
} __attribute__((packed)) pcaprec_hdr_t;

int main() {
    //open (sample) file in binary read mode
    FILE *fp = fopen("sample.pcap", "rb");
    if (!fp) {
        printf("Error: Could nto open PCAP file.\n");
        return 1;
    }

    //read the global header
    pcap_hdr_t global_header;
    fread(&global_header, sizeof(pcap_hdr_t), 1, fp);
    printf("Successfully opened PCAP file. Special Number: 0x%X\n", global_header.special_number)

    pcaprec_hdr_t packet_header;
    if (fread(&packet_header, sizeof(pcaprec_hdr_t), 1, fp) ==1) {
        printf("First packet found. Size in file: %u bytes\n", packet_header.incl_len);

        //allocate memory and read the actual packet payload
        uint8_t *packet_data = malloc(packet_header.incl_len);
        fread(packet_data, 1, packet_header.incl_len, fp);
  
     //parsing: after iits raw bytes in memory, peel it back
   //not complete yet
   //
   //
   //
   
    free(packet_data);
    }

    fclose(fp);
    return 0;
}
