# api freame structure 
  the basics packes which are recived and transmitted are know as frames. the farmes are not send and received throught the serial interface but instead of that is sent throught wireless messaging itself as well as some other info abt the destination/source.


  when the overall all device is in api mode, all of the data input and output modules through the serial interface is done in frames that will tend to define the overall operation or the event within the device.

  Api frame structure: 
  |Start delimiter|length|Frame data         |checksum |
  |1              |2     |4 5 6 7 ..... n    |n + 1    |
  |0x7E           |MSB   |LSB                |Single byte |
  msb -> most sig bit 
  lsb -> least sig bit
  all of the data in frames which is transmitted is in hex. any of the data which is received to the serial interface prior to the start delimiter is just silently discarded by the XBee. if the frame are not received correctly, or the check sum fails the same thing happens

  ## start delimiter
    the start delimiter is the first byte is a special sequence of 0x7E, which is at the start of every single frame. 
  ## Lenght 
    the length field specifies the total number of bytes including the frame sata field. The two-byte value exclude the start delimiter, the length and the checksum.
  ## Frame Data
    the frame data is the infomation which is recived or transmitted. 
    - Frame Type is the api frame type identifier. it helps with interpreting the frame data .
    - Data contain the data itself
  ## checksum 
    checksum is the checksum basically. its found by doing the has sum of all the api frame bytes that came before it other than the first 3 bytes (start delimiter, and length).
    calculate the checksum of an api frame 
      - all of the byte for the package, other than first 3 byte
      - from the result keep, only the lowest 8 bits 
      - subtract this quantiy from 0xFF 
      (all of the values are going to be inside of hex (remember this pls pls pls psl))
      - for verifing the checksum, we are going to add all of the values other than the starting delimiter and the lenght and then after that when we get the value for the check sum if the last two digits on the farr right are the same then that means that the check sum is correct



# transmit request - 0x10 
  the frame types is used in order to send the payload data as an rf packet to a specific destination. This frame type is mainly used inorder to transmit data from one remote device to the other. 
  the endpoint are defined as SE and EP commands and teh cluster ID defined by CI command - excluding 802.15.4. 
  Np command reads the max number of payload bytes that can be sent. 
  ## 64 bit addressing 
    - inroder to brodcast the teransmission all we have to do is set the destination address to 0x000000000000FFFF. 
    - for unicast transimission, set the 64 bit address field to the address of the desired destion node.
  ## formatting
    |Offset |Size|Frame Field | Description |
    |----------|-------|--------|
    |0 | 8 bit| Start Delimiter | indicates the start of the api frame|
    |1|16 bits| Lenght| the length of the payload|
    |3| 8 bit| Frame Type | Transmits the Requrest - 0x10|
    |4| 8 bit| Frame ID| Identifies the data frame for the host and then after that tells if the overall sequence is going to have a response or not, is set to 0 if the device will not immpity any respone|
