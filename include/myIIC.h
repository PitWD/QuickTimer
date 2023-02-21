#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "myESC.h"

#define IIC_STR_LEN 34
#define IIC_HLP_LEN 17

byte myAddress = 123;

// global IIC I/O buffer
char iicStr[IIC_STR_LEN];
char strHLP[IIC_HLP_LEN];
char strHLP2[IIC_HLP_LEN];
char strDefault[IIC_HLP_LEN];

char ByteToChar(byte valIN);

char IICgETsTRING(byte address, byte atlasValidity, byte readBin, byte readBinBytes){

    // returns
    //          >0 = successful read (count of chars)
    // IIC_STR_LEN = (string-len overflow)
    //           0 = nothing to read
    //          -1 = still processing (atlas)
    //          -2 = syntax error (atlas)
    //          -3 = IIC error
    //          -4 = unknown error (atlas)
  
    char count = 0;
    int i2c_error;
    byte firstChar = 1;

    iicStr[0] = 0;

    // Request up to IIC_STR_LEN bytes
    Wire.requestFrom(address, readBinBytes);

    while (Wire.available() && count < IIC_STR_LEN - 1 && count < readBinBytes){

        byte c = Wire.read();

        if (firstChar && atlasValidity){
            // 1st char in a atlas answer indicates the answers "quality"
            // 255 = no data to send
            // 254 = still processing
            // 2 = syntax error
            // 1 = valid

            if (c == 1){
                // Dummy to prevent break
                count = 1;
            }
            else if (c == 255){
                count = 0;
            }
            else if (c == 254){
                count = -1;
            }
            else if (c == 2){
                count = -2;
            }
            else{
                count = -4;
            }

            if (count < 1){
                break;
            }
            count = 0;
            firstChar = 0; 
        }
        else{
            // regular chars

            iicStr[(int)count] = ByteToChar(c);

            if (c == 0 && !readBin){
                // regular End Of String
                break;
            }

            count++;

            iicStr[(int)count] = 0;
        }    
    }

    i2c_error = Wire.endTransmission();  // Check for I2C communication errors

    if (i2c_error == 0 || count < 0){
        return count;
    }
    else{
        return -3;
    }
}
#define IIcGetStr(address) IICgETsTRING(address, 0, 0, IIC_STR_LEN - 1)
#define IIcGetAtlas(address) IICgETsTRING(address, 1, 0, IIC_STR_LEN - 1)
#define IIcGetBytes(address, byteCnt) IICgETsTRING(address, 0, 1, byteCnt)

char IICsEtSTR(byte address, char *strIN, byte term, byte setBin, byte setBinBytes){

    byte length = strlen(strIN);

    if (!setBin){
        if (!length){
            return 0;
        }
    }
    else{
        length = setBinBytes;
    }
    
    Wire.beginTransmission(address);


    for (byte i = 0; i < length + term; i++){
        Wire.write((byte)strIN[i]);
    }

    int i2c_error = Wire.endTransmission();

    if (i2c_error == 0){
        return length;
    }
    else{
        return -1;
    }
}
#define IIcSetStr(address, strIN, term) IICsEtSTR(address, strIN, term, 0 ,0)
#define IIcSetBytes(address, strIN, byteCnt) IICsEtSTR(address, strIN, 0, 1 ,byteCnt)

