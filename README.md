# api freame structure 
  the basics packes which are recived and transmitted are know as frames. the farmes are not send and received throught the serial interface but instead of that is sent throught wireless messaging itself as well as some other info abt the destination/source.


  when the overall all device is in api mode, all of the data input and output modules through the serial interface is done in frames that will tend to define the overall operation or the event within the device.

  Api frame structure: 
  |Start delimiter||length||Frame data         ||checksum |
  |1              ||2     ||4 5 6 7 ..... n    ||n + 1    |
  |0x7E           ||MSB   ||LSB                ||Single byte |
