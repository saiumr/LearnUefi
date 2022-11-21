#ifndef __GIF_PARSER_LIB_INTERNALS__
#define __GIF_PARSER_LIB_INTERNALS__

#include <Uefi.h>
#include <Library/GifParserLib.h>
#include <Library/DebugLib.h>
#include <Library/BmpSupportLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleFileSystem.h>
#include "bmp.h"
#include "lzw/lzw.h"

#endif