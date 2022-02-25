#include "header.h"

unsigned int read_integer_input(void)
{
  unsigned int integerValue=0;  // Max value is 65535
  char incomingByte;

  while(!Serial.available()) {}
  
  if (Serial.available() > 0) 
  {   
      // something came across serial
      integerValue = 0;         // throw away previous integerValue
      while(1)
      {            
        // force into a loop until 'n' is received
        incomingByte = Serial.read();
        
        if (incomingByte == '\n') break;   // exit the while(1), we're done receiving
        if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
        integerValue *= 10;  // shift left 1 decimal place
        
        // convert ASCII to integer, add, and shift left 1 decimal place
        integerValue = ((incomingByte - 48) + integerValue);
      }
  }

  return integerValue;
}
