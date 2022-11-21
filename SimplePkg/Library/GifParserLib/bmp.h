#ifndef _BMP_
#define _BMP_

#include <Base.h>
#pragma pack(1)

typedef struct {
  // [14 Bytes] BMP file header
  CHAR8      CharB;              // 'B'
  CHAR8      CharM;              // 'M'
  UINT32    Size;               // bmp picture file size by byte
  UINT16    Reserved[2];        // 0x0000
  UINT32    ImageOffset;        // address offset start with 0x00 to image data, always 0x36 [image header is 54 Bytes, image data follow it]
  
  // [40 Bytes] BMP information header
  UINT32    HeaderSize;     // 24bit for b,g,r pic always value = 0x28 
  UINT32    PixelWidth;         // image width
  UINT32    PixelHeight;        // image height
  UINT16    Planes;             // Must be 1
  UINT16    BitPerPixel;        // 1, 4, 8, or 24, one pixel occupy how many bit, for example, if 24, r8 g8 b8 -> one pixel
  UINT32    CompressionType;    // 0 for not compression
  UINT32    ImageSize;          // Compressed image size in bytes
  UINT32    XPixelsPerMeter;    // pixel/meter, you can set this not too much carefully
  UINT32    YPixelsPerMeter;    // same as XPixelsPerMeter
  UINT32    NumberOfColors;     // bmp actually used color index amount, 0 for use all color index
  UINT32    ImportantColors;    // 0 for all color is important
} BMP_IMAGE_HEADER;
// here should be image data, start address = 0x36 + 1
// BMP storage color order is b, g, r

#pragma pack()

#endif