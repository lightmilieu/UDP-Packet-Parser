# UDP-Packet-Parser

Abstract 

*In high-frequency trading, a FPGA acts as a hardware packet parser, reading binary (UDP) packets straight off the wire, extracting a price in nanoseconds, and deciding whether to buy or sell instantly. This project is aimed to be a software version of a packet parser in C.*

Description of file structure

Code

This **code** will read a file containing raw network data and parse it into an array of readable C structs. The C program examines the exact "specifications" of the packet.

Methods

Uses [Wireshark](https://www.wireshark.org/) and unpacks **insert .pcap file**. The C program opens the file and loops through thousands of UDP packets, parses out the data, and calculates the average price of an asset over time.

References

