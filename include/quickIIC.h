#ifndef QUICKIIC_H
#define QUICKIIC_H

#include "quicklib.h"

char IICgETsTRING(byte address, byte atlasValidity, byte readBin, byte readBinBytes);
#define IIcGetStr(address) IICgETsTRING(address, 0, 0, IIC_STR_LEN - 1)
#if USE_ATLAS_PRTCL
    #define IIcGetAtlas(address) IICgETsTRING(address, 1, 0, IIC_STR_LEN - 1)
#endif
#define IIcGetBytes(address, byteCnt) IICgETsTRING(address, 0, 1, byteCnt)

char IICsEtSTR(byte address, char *strIN, byte term, byte setBin, byte setBinBytes);
#define IIcSetStr(address, strIN, term) IICsEtSTR(address, strIN, term, 0 ,0)
#define IIcSetBytes(address, strIN, byteCnt) IICsEtSTR(address, strIN, 0, 1 ,byteCnt)

#endif
