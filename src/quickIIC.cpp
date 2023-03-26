
#include "quickIIC.h"

// global IIC I/O buffer
char iicStr[IIC_STR_LEN];

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
    
    #if USE_ATLAS_PRTCL
        byte firstChar = 1;
    #endif

    iicStr[0] = 0;

    // Request up to IIC_STR_LEN bytes
    Wire.requestFrom(address, readBinBytes);

    while (Wire.available() && count < IIC_STR_LEN - 1 && count < readBinBytes){

        byte c = Wire.read();

        #if USE_ATLAS_PRTCL
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
        #endif
            // regular chars

            iicStr[(int)count] = ByteToChar(c);

            if (c == 0 && !readBin){
                // regular End Of String
                break;
            }

            count++;

            iicStr[(int)count] = 0;
            
        #if USE_ATLAS_PRTCL
            }    
        #endif
    }

    i2c_error = Wire.endTransmission();  // Check for I2C communication errors

    if (i2c_error == 0 || count < 0){
        return count;
    }
    else{
        return -3;
    }
}

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
