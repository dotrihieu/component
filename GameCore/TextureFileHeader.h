#ifndef TEXTURE_FILE_HEADER
#define TEXTURE_FILE_HEADER
#include "stdint.h"
struct PVRTexHeaderV2
{
    uint32_t headerLength;
    uint32_t height;
    uint32_t width;
    uint32_t numMipmaps;
    uint32_t flags;
    uint32_t dataLength;
    uint32_t bpp;
    uint32_t bitmaskRed;
    uint32_t bitmaskGreen;
    uint32_t bitmaskBlue;
    uint32_t bitmaskAlpha;
    uint32_t pvrTag;
    uint32_t numSurfs;
};
struct PVRTexHeaderV3{
    uint32_t    version;            
    uint32_t    flags;          
    uint64_t    pixelFormat;        
    uint32_t    colourSpace;        
    uint32_t    channelType;        
    uint32_t    height;         
    uint32_t    width;          
    uint32_t    depth;          
    uint32_t    numSurfaces;        
    uint32_t    numFaces;       
    uint32_t    numMipmaps;     
    uint32_t    metaDataSize;   
};
struct TGAHeadrer{
   uint8_t  idlength;
   uint8_t  colourmaptype;
   uint8_t  datatypecode;
   uint16_t colourmaporigin;
   uint16_t colourmaplength;
   uint8_t  colourmapdepth;
   uint16_t x_origin;
   uint16_t y_origin;
   uint16_t width;
   uint16_t height;
   uint8_t  bitsperpixel;
   uint8_t  imagedescriptor;
} ;
#endif