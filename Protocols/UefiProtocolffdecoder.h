/** @file
  UefiProtocolffdecoder Protocol is used by decoding video, it is customized by 
  saiumr and it is just a practice. 

  Copyright (c) 2022, Byo Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __FFDECODER_PROTOCOL_H__
#define __FFDECODER_PROTOCOL_H__

#include <Uefi.h>

#define EFI_FFDECODER_PROTOCOL_GUID \
    { 0xce387492, 0xabcd, 0x112d, { 0x3d, 0x83, 0xca, 0x89, 0x8c, 0x23, 0xcd, 0x67 } }

// compatible UEFI spec 1.1
#define FFDECODER_PROTOCOL EFI_FFDECODER_PROTOCOL_GUID

typedef struct _EFI_FFDECODER_PROTOCOL EFI_FFDECODER_PROTOCOL;


/**
  Open video.

  This function open video file by specified path.

  @param[in]      This        The EFI_FFDECODER_PROTOCOL instance.
  @param[in]      FileName    Video file path.

  @retval EFI_SUCCESS         Everything is ok.
  @retval EFI_NOT_FOUND       cannot found file.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_VIDEO)(
  IN EFI_FFDECODER_PROTOCOL *This,
  IN CHAR16                 *FileName
  );

/**
  Close video.

  This function close video by instance.

  @param[in]      This        The EFI_FFDECODER_PROTOCOL instance.

  @retval EFI_SUCCESS         Successfully close video.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_CLOSE_VIDEO)(
  IN EFI_FFDECODER_PROTOCOL *This
  );

/**
  Query video frame.

  This function open video file by specified path.

  @param[in]      This        The EFI_FFDECODER_PROTOCOL instance.
  @param[out]     pFrame      Retrieves frame from video.

  @retval EFI_SUCCESS         Everything is ok.
  @retval EFI_NO_MEDIA        There isn't valid video opened.
  @retval EFI_END_OF_MEDIA    Already reach the video end.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_QUERY_FRAME)(
  IN  EFI_FFDECODER_PROTOCOL *This,
  OUT AVFrame                **pFrame
  );
// Fake struct
typedef struct {
  int a;
  int b;
} AVFrame;


/**
  Get video frame's width and height(px)

  This function is used by getting video frame width and height.

  @param[in]      This        The EFI_FFDECODER_PROTOCOL instance.
  @param[out]     Width       Video frame width.
  @param[out]     Height      Video frame height.

  @retval EFI_SUCCESS         Everything is ok.
  @retval EFI_NO_MEDIA        There isn't valid video opened.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_QUERY_FRAME_SIZE)(
  IN  EFI_FFDECODER_PROTOCOL *This,
  OUT UINT32                 *Width,
  OUT UINT32                 *Height
  );

/**
  Setting retrieved video frame's width and height.

  This function is used by setting retrieves video frame's with and height, 
  if the width and height were different from original frame then modify 
  original frame with params and output.

  @param[in]      This        The EFI_FFDECODER_PROTOCOL instance.
  @param[out]     Width       Video frame width.
  @param[out]     Height      Video frame height.

  @retval EFI_SUCCESS         Everything is ok.
  @retval EFI_NO_MEDIA        There isn't valid video opened.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRAME_SIZE)(
  IN EFI_FFDECODER_PROTOCOL *This,
  IN UINT32                 *Width,
  IN UINT32                 *Height
  );


struct _EFI_FFDECODER_PROTOCOL {
  UINT64 Revision;
  EFI_OPEN_VIDEO          OpenVideo;
  EFI_CLOSE_VIDEO         CloseVideo;
  EFI_QUERY_FRAME         QueryFrame;
  EFI_QUERY_FRAME_SIZE    QueryFrameSize;
  EFI_SET_FRAME_SIZE      SetFrameSize;
};

extern EFI_GUID gEfiFFDecoderProtocolGUID;

#endif  // __FFDECODER_PROTOCOL_H__