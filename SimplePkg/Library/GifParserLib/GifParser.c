#include "GifParserLibInternals.h"

#define ALLOC_COMPONENT_AMOUNT 512
#define ALLOC_COMPONENT_SIZE   ALLOC_COMPONENT_AMOUNT*sizeof(GIF_COMPONENT)
UINT16  gAllocComponentCount = 1;

// read memory and move file pointer 
BOOLEAN
_GIFParserMemRead(OUT VOID *dst, IN UINT8 **src, IN UINT32 size); 

BOOLEAN
_HandleExtension(IN CHAR8 **src, IN UINT8 label, OUT GIF **gif);

BOOLEAN
_HandleImageData(IN CHAR8 **src, OUT GIF **gif);

VOID
_RecordComponentOrder(IN GIF_COMPONENT key, OUT GIF **gif);

// You should not use this function, it help us debug when library function error.
VOID
_PrintBuffer(IN CHAR8 *buffer, IN UINTN len);

EFI_STATUS
EFIAPI
_LoadFile2Buffer (
  IN      CHAR16           *FileName,
  IN OUT  UINT8            **FileDataBuffer,
  IN OUT  UINT32           *DataLength
);

typedef struct {
	GIF_APP_EXT_DATA 				*app;
	GIF_COMMENT_EXT_DATA		*comment;
	GIF_GRAPHICS_EXT_DATA   *graphics;
	GIF_IMAGE_DATA          *image;
} _GIF_TAILER_POINTER;

_GIF_TAILER_POINTER gTailerPointer;

EFI_STATUS
EFIAPI
GIFParserLogoBltFromFileOrBuffer(IN CHAR16 *filename, IN CHAR8 *file_buffer) {
	EFI_STATUS                       Status;
  EFI_EVENT                        Events[2]       = {0};    // 0: Events[0] Time; 1: End Input
  UINTN                            EventIndex      = 0;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput = NULL;
  UINT32                           FrameSize       = 0;
  UINTN                            FrameIndex      = 0;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    **Bitmap        = NULL;
  UINTN                            GopBltSize      = 0;
  UINTN                            Height          = 0;
  UINTN                            Width           = 0;
  UINT8                            *Frame          = NULL;
	IMG_ANIMATION                    *animation      = NULL;
	DEBUG((EFI_D_INFO, "GIFParserLogoBltFromFileOrBuffer Start.\n"));

	if (filename) {	
		if ( !GIFParserGetAnimationFromFile(filename, &animation) ) {
			DEBUG((EFI_D_INFO, "Load file failed from file.\n"));
			return EFI_ABORTED;
		}
	} else if (file_buffer) {
		if ( !GIFParserGetAnimationFromFileBuffer(file_buffer, &animation) ) {
			DEBUG((EFI_D_INFO, "Load file failed from buffer.\n"));
			return EFI_ABORTED;
		}
	} else {
			DEBUG((EFI_D_INFO, "No file input.\n"));
			return EFI_ABORTED;
	}

  if (animation == NULL) {
    DEBUG((EFI_D_INFO, "animation is NULL.\n"));
    return EFI_ABORTED;
  }

  Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&GraphicsOutput);

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "Located Protocol Failed.\n"));
    return EFI_ABORTED;
  }

  Bitmap = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL **)AllocatePool(sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) * animation->count + 1);  // the last memory equals NULL.
  if (Bitmap == NULL) {
    DEBUG((EFI_D_INFO, "Bitmap AllocatePool failed.\n"));
    return EFI_ABORTED;
  }

  // load
  for (FrameIndex = 0; FrameIndex < animation->count; ++FrameIndex) {
		Bitmap[FrameIndex] = NULL;  // must set NULL
    Frame  = GIFParserAnimationFramesTransformBMP(animation, FrameIndex, &FrameSize);
    
		if (Frame) {
			Status = TranslateBmpToGopBlt(
								(VOID *)Frame,
								FrameSize,
								&Bitmap[FrameIndex],
								&GopBltSize,
								&Height,
								&Width
								);
			FreePool(Frame);
			FreePool(animation->frames[FrameIndex]);
			Frame = NULL;
			animation->frames[FrameIndex] = NULL;
		}
		else {
			DEBUG((EFI_D_INFO, "LOGO Frame is NULL\n"));
			return EFI_ABORTED;
		}
    DEBUG((EFI_D_INFO, "FrameSize: %u, Bitmap size should be: %u\n", FrameSize, GopBltSize));
		
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "Translate failed [Index = %u], Status = %u\n", FrameIndex, Status));
      for (FrameIndex = 0; FrameIndex < animation->count; ++FrameIndex) {
        if (Bitmap[FrameIndex]) FreePool(Bitmap[FrameIndex]);
      }
      if (Bitmap) FreePool(Bitmap);
      return EFI_ABORTED;
    }
  }

  // play
  Status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID*)NULL, &Events[0]);
  Status = gBS->SetTimer(Events[0], TimerPeriodic, animation->delays * 100 * 100);  // ms
  Events[1] = gST->ConIn->WaitForKey;

  for (FrameIndex = 0; FrameIndex < animation->count; ) {
    gBS->WaitForEvent(2, Events, &EventIndex);
    switch (EventIndex) {
      case 0:
      {
        Status = GraphicsOutput->Blt(
                  GraphicsOutput,
                  Bitmap[FrameIndex],
                  EfiBltBufferToVideo,
                  0, 
                  0, 
                  (GraphicsOutput->Mode->Info->HorizontalResolution - Width) / 2, 
                  (GraphicsOutput->Mode->Info->VerticalResolution - Height) / 2,
                  Width,
                  Height,
                  Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                  );
        
        if (EFI_ERROR(Status)) {
          DEBUG((EFI_D_INFO, "Blt Failed [Index = %u]\n", FrameIndex));
          for (FrameIndex = 0; FrameIndex < animation->count; FrameIndex++) {
            if (Bitmap[FrameIndex] != NULL) {
              FreePool(Bitmap[FrameIndex]);
            }
          }
          FreePool(Bitmap);
          return EFI_ABORTED;
        }

        FrameIndex++;
        if (FrameIndex == animation->count) {
          FrameIndex = 0;
        }
      }
        break;

      default:
        goto end;
        break;
    }
  }  

  end:
	// Do not close Events[1], it's a system global event.
  // gBS->CloseEvent(Events[1]);
  gBS->CloseEvent(Events[0]);

	// FreePool
  for (FrameIndex = 0; FrameIndex < animation->count; FrameIndex++) {
    if (Bitmap[FrameIndex]) {
			FreePool(Bitmap[FrameIndex]);
		}
		else { 
			break;
		}
  }
	if (Bitmap) FreePool(Bitmap);
	GIFParserClearAnimation(animation);

  return EFI_SUCCESS;
}

UINT8 *GIFParserAnimationFramesTransformBMP(IN IMG_ANIMATION *animation, IN UINTN frame_index, OUT UINT32 *frame_size) {
	DEBUG((EFI_D_INFO, "Now function: GIFParserAnimationFramesTransformBMP [frame_index = %llu]\n", frame_index));
	if (frame_index >= animation->count) {
		return NULL;
	}

	// one frame buffer
	UINT32 line_data = animation->w * sizeof(IMG_FRAME);
	if (line_data % 4 != 0) {
		line_data = (line_data / 4 + 1) * 4;
	}
	UINT32 line_diff = line_data - animation->w*3;
	UINT32 buffer_color_amount = line_data * animation->h; // 1 * extension width data * h (colors)
	*frame_size = 54 + buffer_color_amount;
	UINT8 *buffer = (UINT8 *)AllocatePool(*frame_size);
	//printf("bmp file size: %u\n", *frame_size);
	if (buffer == NULL) {
		return NULL;
	}
	BMP_IMAGE_HEADER bmp_header;
	bmp_header.CharB = 'B';
	bmp_header.CharM = 'M';
	bmp_header.Size  = *frame_size;  // bit_size/8 -> Byte size
	bmp_header.Reserved[0] = 0;
	bmp_header.Reserved[1] = 0;
	bmp_header.ImageOffset = 0x36;
	bmp_header.HeaderSize = 0x28;  // 40 Bytes
	bmp_header.PixelWidth  = animation->w;
	bmp_header.PixelHeight = animation->h;
	bmp_header.Planes = 1;
	bmp_header.BitPerPixel = 0x18;     // 24 bits per pixel
	bmp_header.CompressionType = 0;
	bmp_header.ImageSize = bmp_header.Size - bmp_header.ImageOffset;
	bmp_header.XPixelsPerMeter = 0x1625;
	bmp_header.YPixelsPerMeter = 0x1625;
	bmp_header.NumberOfColors  = 0;
	bmp_header.ImportantColors = 0;

	// header
	CopyMem(buffer, &bmp_header, 54);
	// image data
	UINTN index = 54;
	for (UINTN row = animation->h - 1; row >= 0; --row) {
		for (UINTN col = 0; col < animation->w; ++col) {
			buffer[index++] = animation->frames[frame_index][animation->w * row + col].b;
			buffer[index++] = animation->frames[frame_index][animation->w * row + col].g;
			buffer[index++] = animation->frames[frame_index][animation->w * row + col].r;
		}
		if (line_diff != 0) {
			for (UINTN diff = 0; diff < line_diff; ++diff) {
				buffer[index++] = 0;
			}
		}
		// row is unsigned value
		if (row == 0) {
			break;
		}
	}

	return buffer;
}

BOOLEAN
GIFParserGetAnimationFromFile(IN CHAR16 *filename, OUT IMG_ANIMATION **animation) {
	//printf("Now function: GIFParserGetAnimationFromFile\n");
	GIF   *gif = (GIF*)AllocatePool(sizeof(GIF));
	UINT32 buffer_size = 0;
	if (!GIFParserGetGifDataFromFile(filename, &gif, &buffer_size)) {
		return FALSE;
	}

	if (!GIFParserGetAnimationFromGif(gif, animation)) {
		return FALSE;
	}

	GIFParserClear(gif);

	return TRUE;
}

BOOLEAN
GIFParserGetAnimationFromFileBuffer(IN CHAR8 *file_buffer, OUT IMG_ANIMATION **animation) {
	GIF   *gif = (GIF*)AllocatePool(sizeof(GIF));
	
	DEBUG((EFI_D_INFO, "GIFParserGetAnimationFromFileBuffer\n"));
	if (!GIFParserGetGifDataFromFileBuffer(file_buffer, &gif)) {
		return FALSE;
	}

	if (!GIFParserGetAnimationFromGif(gif, animation)) {
		return FALSE;
	}

	GIFParserClear(gif);

	return TRUE;
}

BOOLEAN
GIFParserGetAnimationFromGif(IN GIF *gif, OUT IMG_ANIMATION **animation) {
	//printf("Now function: GIFParserGetAnimationFromGif\n");
	*animation = (IMG_ANIMATION *)AllocatePool(sizeof(IMG_ANIMATION));
	if ( (*animation) == NULL ) {
		return FALSE;
	}
	(*animation)->w = gif->LogicalScreenDescriptor.canvas_width;   // true area
	(*animation)->h = gif->LogicalScreenDescriptor.canvas_height;	
	if (gif->GraphicsExtHeader->next == NULL) {
		//printf("gif->GraphicsExtHeader->next == NULL\n");
	}
	(*animation)->delays = (UINT32)(gif->GraphicsExtHeader->next->graphics.delay_time * 10);  // ms
	(*animation)->count = gif->FramesCount;
	
	(*animation)->frames = (IMG_FRAME **)AllocatePool(sizeof(IMG_FRAME*) * (*animation)->count);
	if ((*animation)->frames == NULL) {
		return FALSE;
	}

	//printf("true width/height: %u, %u\n", (*animation)->w, (*animation)->h);

	typedef enum COLOR_KIND {
		LOCAL_COLOR,
		GLOBAL_COLOR
	} COLOR_KIND;
	UINTN pixel_count = (*animation)->w * (*animation)->h;
	UINT8 *color_index_list = (UINT8 *)AllocatePool(sizeof(UINT8) * pixel_count);
	COLOR_KIND *color_index_kind = (COLOR_KIND *)AllocatePool(sizeof(COLOR_KIND) * pixel_count);
	if (color_index_list == NULL) {
		return FALSE;
	}

	UINTN frame_count = 0;
	GIF_IMAGE_DATA  *prev_image = gif->ImageDataHeader;
	GIF_IMAGE_DATA  *image = prev_image->next;
	GIF_GRAPHICS_EXT_DATA *graphics = gif->GraphicsExtHeader->next;
	GIF_COLOR_TABLE transparency_color = {0, 0, 0};
	GIF_COLOR_TABLE bg_color = {255, 255, 255};

	if (((gif->LogicalScreenDescriptor.flag_color_table&0x1)) == 1) {
			bg_color.r = gif->GlobalColorTable[gif->LogicalScreenDescriptor.bg_color_index].r;
			bg_color.g = gif->GlobalColorTable[gif->LogicalScreenDescriptor.bg_color_index].g;
			bg_color.b = gif->GlobalColorTable[gif->LogicalScreenDescriptor.bg_color_index].b;
	}

	while (image != NULL) {
		//printf("frame count: [%llu]\n", frame_count);
		//printf("w = %u, h = %u, delay = %u, left = %u, top = %u\n", image->image_descriptor.width, image->image_descriptor.height, (*animation)->delays, \
		//image->image_descriptor.left, image->image_descriptor.top);
			
		UINT32 changed_data_size = image->image_descriptor.width * image->image_descriptor.height;  // exclude non-changed parts
		UINT8 *changed_color_index_list = (UINT8 *)AllocatePool(changed_data_size);
		UINT8 *changed_data = (UINT8 *)AllocatePool(sizeof(UINT8) * image->one_frame_data.data_sub_block_buffer.total_data_size);

		if (changed_data == NULL) {
			return FALSE;
		}

		GIF_DATA_SUB_BLOCK_NODE *p = image->one_frame_data.data_sub_block_buffer.header->next;
		UINTN changed_data_index = 0;
		while (p != NULL) {
			CopyMem(changed_data + changed_data_index, p->data, p->data_size);
			changed_data_index += p->data_size;
			p = p->next;
		}

		UINT8 bit_size = image->one_frame_data.LZW_Minimum_Code;
		UINT32 out_changed_data_size = 0;
		//printf("bit_size: %u, changed_data_index: %llu, changed_data_size: %u\n", bit_size, changed_data_index, changed_data_size);
		lzw_decompress(bit_size, changed_data_index, changed_data, (UINTN *)&out_changed_data_size, &changed_color_index_list);
		//printf("out_changed_data_size: %u\n", out_changed_data_size);

		// restore color
		IMG_FRAME *frame = (IMG_FRAME *)AllocatePool(sizeof(IMG_FRAME) * pixel_count);
		if (frame == NULL) {
			return FALSE;
		}

		if ((graphics->graphics.flag_transparency_used&0x1) == 1) {	
			if (image->local_color_table) {
				transparency_color.r = image->local_color_table[graphics->graphics.transparent_color_index].r;
				transparency_color.g = image->local_color_table[graphics->graphics.transparent_color_index].g;
				transparency_color.b = image->local_color_table[graphics->graphics.transparent_color_index].b;
			}
			else {
				transparency_color.r = gif->GlobalColorTable[graphics->graphics.transparent_color_index].r;
				transparency_color.g = gif->GlobalColorTable[graphics->graphics.transparent_color_index].g;
				transparency_color.b = gif->GlobalColorTable[graphics->graphics.transparent_color_index].b;
			}
		}

		//printf("image data size = %llu\n", sizeof(IMG_FRAME) * pixel_count);
		//if ((graphics->graphics.flag_transparency_used&0x1) == 1)
		//printf("transparency_color: %u %u %u\n", transparency_color.r, transparency_color.g, transparency_color.b);
		//if ((gif->LogicalScreenDescriptor.flag_color_table&0x1) == 1)
		//printf("bg_color: %u %u %u\n", bg_color.r, bg_color.g, bg_color.b);

		// In gif every frame's image data depends on previous frame. change changed data, reserved out of changed data back ground color index flush;
		UINTN global_part_index = 0;
		for (UINTN row = 0; row < (*animation)->h; ++row) {
			for (UINTN col = 0; col < (*animation)->w; ++col) {
				if ( col < image->image_descriptor.left || row < image->image_descriptor.top || \
				  col > image->image_descriptor.left + image->image_descriptor.width - 1 || \
					row > image->image_descriptor.top + image->image_descriptor.height - 1 ) {
					switch (graphics->graphics.flag_disposal_method&0x7) {
					// reserved previous frame pixels 
					case 0x01:
					{
						if (frame_count == 0) {
							if ((graphics->graphics.flag_transparency_used&0x1) == 1) {
								color_index_list[row * (*animation)->w + col] = graphics->graphics.transparent_color_index;
							}
							else if((gif->LogicalScreenDescriptor.flag_color_table&0x1) == 1) {
								color_index_list[row * (*animation)->w + col] = gif->LogicalScreenDescriptor.bg_color_index;
							}
						}
					}
					break;

					// repainted out of image range
					case 0x02:
					{
						if ((graphics->graphics.flag_transparency_used&0x1) == 1) {
							color_index_list[row * (*animation)->w + col] = graphics->graphics.transparent_color_index;
						}
						else if((gif->LogicalScreenDescriptor.flag_color_table&0x1) == 1) {
							color_index_list[row * (*animation)->w + col] = gif->LogicalScreenDescriptor.bg_color_index;
						}
					}
					break;

					// 3~7
					default:
						break;
					}
				}
				else if (frame_count != 0) {
					switch (graphics->graphics.flag_disposal_method&0x7) {
					// reserved transparent pixel as previous frame pixel 	
					case 0x01:
					{
						if ( (graphics->graphics.flag_transparency_used&0x1) == 1 && graphics->graphics.transparent_color_index == changed_color_index_list[global_part_index] )  {
							if (color_index_kind[row * (*animation)->w + col] == GLOBAL_COLOR) {
								frame[row * (*animation)->w + col].r = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].r;
								frame[row * (*animation)->w + col].g = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].g;
								frame[row * (*animation)->w + col].b = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].b;
							}
							else if ( ((prev_image->image_descriptor.flag_color_table&0x1)) == 1) {
								frame[row * (*animation)->w + col].r = prev_image->local_color_table[color_index_list[row * (*animation)->w + col]].r;
								frame[row * (*animation)->w + col].g = prev_image->local_color_table[color_index_list[row * (*animation)->w + col]].g;
								frame[row * (*animation)->w + col].b = prev_image->local_color_table[color_index_list[row * (*animation)->w + col]].b;
							}
							else {
								frame[row * (*animation)->w + col].r = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].r;
								frame[row * (*animation)->w + col].g = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].g;
								frame[row * (*animation)->w + col].b = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].b;
							}
							
							++global_part_index;
							continue;
						}
						else {
							color_index_list[row * (*animation)->w + col] = changed_color_index_list[global_part_index];
							++global_part_index;
						}
					}
					break;

					// repainted every pixels in image range
					case 0x02:
					{
						color_index_list[row * (*animation)->w + col] = changed_color_index_list[global_part_index];
						++global_part_index;						
					}
					break;

					// 0 && 3~7
					default:
						break;
					}
				}
				else {
					color_index_list[row * (*animation)->w + col] = changed_color_index_list[global_part_index];
					++global_part_index;
				}

				if ((image->image_descriptor.flag_color_table&0x1) == 1) {
					frame[row * (*animation)->w + col].r = image->local_color_table[color_index_list[row * (*animation)->w + col]].r;
					frame[row * (*animation)->w + col].g = image->local_color_table[color_index_list[row * (*animation)->w + col]].g;
					frame[row * (*animation)->w + col].b = image->local_color_table[color_index_list[row * (*animation)->w + col]].b;
					color_index_kind[row * (*animation)->w + col] = LOCAL_COLOR;
				}
				else if ((gif->LogicalScreenDescriptor.flag_color_table&0x1) == 1) {
					frame[row * (*animation)->w + col].r = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].r;
					frame[row * (*animation)->w + col].g = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].g;
					frame[row * (*animation)->w + col].b = gif->GlobalColorTable[color_index_list[row * (*animation)->w + col]].b;
					color_index_kind[row * (*animation)->w + col] = GLOBAL_COLOR;
				}
			}
		}

		// interface (4 groups)
		// Group0: every 8 lines, start by line 0
		// Group1: every 8 lines, start by line 4
		// Group2: every 4 lines, start by line 2
		// Group3: every 2 lines, start by line 1
		//printf("image->image_descriptor.flag_interlace&0x1: %u\n", image->image_descriptor.flag_interlace&0x1);
		if ((image->image_descriptor.flag_interlace&0x1) == 1) {
			IMG_FRAME *frame_raw = (IMG_FRAME *)AllocatePool(sizeof(IMG_FRAME) * pixel_count);
			UINTN interlace_frame_line = 0;
			for (UINTN group = 0; group < 4; ++group) {
				UINTN current_line = 0;
				UINTN step_line = 0;
				switch (group) {
				case 0:
					current_line = 0;
					step_line = 8;
					break;
				case 1:
					current_line = 4;
					step_line = 8;
					break;
				case 2:
					current_line = 2;
					step_line = 4;
					break;
				case 3:
					current_line = 1;
					step_line = 2;
					break;
				default:
					//printf("interface frame transform failed.\n");
					break;
				}

				// if you exchange `current_line` and `interlace_frame_line`, the image will be interlaced again.  
				while (current_line < (*animation)->h) {
					for (UINTN col = 0; col < (*animation)->w; ++col) {
						frame_raw[current_line * (*animation)->w + col].r = frame[interlace_frame_line * (*animation)->w + col].r;	
						frame_raw[current_line * (*animation)->w + col].g = frame[interlace_frame_line * (*animation)->w + col].g;	
						frame_raw[current_line * (*animation)->w + col].b = frame[interlace_frame_line * (*animation)->w + col].b;	
					}
					++interlace_frame_line;
					current_line += step_line;
				}
			}

			if(frame) FreePool(frame);
			frame = NULL;
			frame = frame_raw;
		}


		(*animation)->frames[frame_count] = frame;
		frame = NULL;
		++frame_count;
		prev_image = image;
		image = image->next;
		graphics = graphics->next;

		if (changed_data) FreePool(changed_data);
		changed_data = NULL;
		if (changed_color_index_list) FreePool(changed_color_index_list);
		changed_color_index_list = NULL;

	}

	if (color_index_list) FreePool(color_index_list);
	if (color_index_kind) FreePool(color_index_kind);
	return TRUE;
}

BOOLEAN
GIFParserClearAnimation(IN IMG_ANIMATION *animation) {
	//printf("Now function: GIFParserClearAnimation\n");
	if (animation) {
		for(UINTN i = 0; i < animation->count; ++i) {
			if (animation->frames[i]) {
				FreePool(animation->frames[i]);
			}
			else {
				return FALSE;
			} 
		}
		if (animation->frames) FreePool(animation->frames);
		FreePool(animation);
	}
	return TRUE;
}

BOOLEAN
GIFParserGetGifDataFromFile(IN CHAR16 *filename, OUT GIF **gif, OUT UINT32 *buffer_size) {
	CHAR8   *file_buffer = NULL;           // DO NOT FREE
	CHAR8   *file_buffer_header = NULL;    // FreePool this FreePool file_buffer
	UINT32  file_size = 0;
	//printf("Now Function: GIFParserGetGifDataFromFile\n");

	_LoadFile2Buffer(filename, &file_buffer, &file_size);
	
	if (file_size == 0) {
		return FALSE;
	}

	*buffer_size = file_size;
	file_buffer_header = file_buffer;

	if (!GIFParserGetGifDataFromFileBuffer(file_buffer, gif)) {
		return FALSE;
	}

	FreePool(file_buffer_header);

	return TRUE;
}

BOOLEAN
GIFParserGetGifDataFromFileBuffer(IN CHAR8 *file_buffer, OUT GIF **gif) {
	UINT8   c = 0;

	if ((*gif) == NULL || file_buffer == NULL) {
		//printf("GIFParserGetGifDataFromFile: pointer error.\n");
		if ((*gif) == NULL) {
			//printf("[gif pointer]\n");
		}
		else {
			//printf("[file buffer]\n");
		}
		return FALSE;
	}
	

	DEBUG((EFI_D_INFO, "GIFParserGetGifDataFromFileBuffer\n"));
	// Init
	(*gif)->GlobalColorTable = NULL;
	(*gif)->AppExtHeader = (GIF_APP_EXT_DATA *)AllocatePool(sizeof(GIF_APP_EXT_DATA));
	(*gif)->AppExtHeader->next = NULL;
	gTailerPointer.app = (*gif)->AppExtHeader;

	(*gif)->CommentExtHeader = (GIF_COMMENT_EXT_DATA *)AllocatePool(sizeof(GIF_COMMENT_EXT_DATA));
	(*gif)->CommentExtHeader->next = NULL;
	gTailerPointer.comment = (*gif)->CommentExtHeader;

	(*gif)->GraphicsExtHeader = (GIF_GRAPHICS_EXT_DATA *)AllocatePool(sizeof(GIF_GRAPHICS_EXT_DATA));
	(*gif)->GraphicsExtHeader->next = NULL;
	gTailerPointer.graphics = (*gif)->GraphicsExtHeader;

	(*gif)->ImageDataHeader = (GIF_IMAGE_DATA *)AllocatePool(sizeof(GIF_IMAGE_DATA));
	(*gif)->ImageDataHeader->next = NULL;
	gTailerPointer.image = (*gif)->ImageDataHeader;
	(*gif)->ImageDataHeader->local_color_table = NULL;

	(*gif)->FramesCount = 0;

	gAllocComponentCount = 1;
	(*gif)->ComponentOrder.component = (GIF_COMPONENT *)AllocatePool(gAllocComponentCount*ALLOC_COMPONENT_SIZE);
	(*gif)->ComponentOrder.size = 0;

	// Header Block 6 bytes
	_GIFParserMemRead(&(*gif)->Header, &file_buffer, 6);

	if ( AsciiStrnCmp((*gif)->Header.gif_signature, "GIF", 3) != 0 ) {
		//printf("GIFParserGetGifDataFromFile: this is not GIF file.\n");
		return FALSE;
	}

	if ( AsciiStrnCmp((*gif)->Header.gif_version, "89a", 3) != 0 ) {
		//printf("GIFParserGetGifDataFromFile: GIF version not supported, use version \"89a\".\n");
		return FALSE;
	}

	// Logical Screen Descriptor 7 Bytes
	_GIFParserMemRead(&(*gif)->LogicalScreenDescriptor, &file_buffer, 7);
	
	// Global color table
	(*gif)->GlobalColorTable = NULL;
	if ( ((*gif)->LogicalScreenDescriptor.flag_color_table&0x1) == 1 ) {
		UINT32 global_color_table_amount = (UINT32)(1<<(((*gif)->LogicalScreenDescriptor.flag_table_size&0x7) + 1));
		(*gif)->GlobalColorTable = (GIF_COLOR_TABLE *)AllocatePool((UINT32)(sizeof(GIF_COLOR_TABLE) * global_color_table_amount));
		_GIFParserMemRead((*gif)->GlobalColorTable, &file_buffer, (UINT32)(sizeof(GIF_COLOR_TABLE) * global_color_table_amount));
	}

	// Extensions and Image Data
	for ( ; ; ) {
		_GIFParserMemRead(&c, &file_buffer, 1);

		switch (c&0xFF) 
		{
			case 0x3B:
				(*gif)->trailer = 0x3B;
				goto end;
			
			case 0x21:
			{
				_GIFParserMemRead(&c, &file_buffer, 1);
				_HandleExtension(&file_buffer, c, gif);
				continue;
			}
				break;

			case 0x2C:
				_HandleImageData(&file_buffer, gif);
				break;

			default:
				break;
		}

#if 0
		// 0x3B  ; 
		if (c == 0x3B) {
			(*gif)->trailer = 0x3B;
			goto done;
		}
		// 0x21  !
		if (c == 0x21) {
			_GIFParserMemRead(&c, &file_buffer, 1);
			_HandleExtension(&file_buffer, c, gif);
			continue;
		}

		// 0x2C  ,
		if (c != 0x2C) {
			continue;
		}

		// Image Descriptor
		_HandleImageData(&file_buffer, gif);
#endif

	}

end:
	return TRUE;
}

UINT8 *GIFParserGetDataBufferFromGif(IN GIF *gif, IN UINT32 buffer_size) {
	//printf("Now function: GIFParserGetDataBufferFromGif\n");
	UINT8 *buffer = (CHAR8 *)AllocatePool(sizeof(UINT8) * buffer_size);
	UINTN index = 0;
	UINTN size = 0;

  // header && logical screen descriptor
	size = 13;
	CopyMem(buffer+index, gif, size);
	index += size;

	// Global color table
	if (gif->GlobalColorTable) {
		size = (UINTN)(1 << ( (gif->LogicalScreenDescriptor.flag_table_size&0x7) + 1 ));
		CopyMem(buffer+index, gif->GlobalColorTable, size * 3);
		index += size * 3;
	}

	// extension and image data
	GIF_APP_EXT_DATA 				*a = gif->AppExtHeader->next;
	GIF_COMMENT_EXT_DATA		*c = gif->CommentExtHeader->next;
	GIF_GRAPHICS_EXT_DATA   *g = gif->GraphicsExtHeader->next;
	GIF_IMAGE_DATA          *i = gif->ImageDataHeader->next;
	for (UINTN component_index = 0; component_index < gif->ComponentOrder.size; ++component_index) {
		switch (gif->ComponentOrder.component[component_index]) {
			case kAppExt:
			{
				if (a != NULL) {
					CopyMem(buffer+index, &a->app.header.introducer, 14);
					index += 14;

					GIF_DATA_SUB_BLOCK_NODE *p = a->app.data_sub_block_buffer.header->next;
					while (p != NULL) {
						CopyMem(buffer+index, &p->data_size, 1);
						index += 1;

						size = p->data_size;
						CopyMem(buffer+index, p->data, size);
						index += size;

						p = p->next;
					}

					CopyMem(buffer+index, &a->app.terminator, 1);
					index += 1;
					a = a->next;
				}
			}
			break;
			case kCommentExt:
			{
				if (c != NULL) {
					CopyMem(buffer+index, &c->comment.header.introducer, 2);
					index += 2;

					GIF_DATA_SUB_BLOCK_NODE *p = c->comment.data_sub_block_buffer.header->next;
					while (p != NULL) {
						CopyMem(buffer+index, &p->data_size, 1);
						index += 1;

						size = p->data_size;
						CopyMem(buffer+index, p->data, size);
						index += size;

						p = p->next;
					}

					CopyMem(buffer+index, &c->comment.terminator, 1);
					index += 1;
					c = c->next;
				}
			} 
			break;
			case kGraphicsExt:
			{
				if (g != NULL) {
					CopyMem(buffer+index, &g->graphics.header.introducer, 8);
					index += 8;
					g = g->next;
				}
			}
			break;
			case kImageData:
			{
				if (i != NULL) {
					CopyMem(buffer+index, &i->image_descriptor, 10);
					index += 10;

					if (i->local_color_table) {
						size = (UINTN)(1 << (i->image_descriptor.flag_table_size&0x7 + 1));
						CopyMem(buffer+index, i->local_color_table, size * 3);
						index += size * 3;
					}

					CopyMem(buffer+index, &i->one_frame_data.LZW_Minimum_Code, 1);
					index += 1;

					GIF_DATA_SUB_BLOCK_NODE *p = i->one_frame_data.data_sub_block_buffer.header->next;
					while (p != NULL) {
						CopyMem(buffer+index, &p->data_size, 1);
						index += 1;

						size = p->data_size;
						CopyMem(buffer+index, p->data, size);
						index += size;
						p = p->next;
					}
					CopyMem(buffer+index, &i->one_frame_data.terminator, 1);
					index += 1;

					i = i->next;
				}
			}
			break;
			default:
				break;
		}
	}

	// 0x3B
	CopyMem(buffer+index, &gif->trailer, 1);
	index += 1;

	//printf("GIFParserGetDataBufferFromGif: index = %llu\n", index);
	return buffer;
}

BOOLEAN
GIFParserClear(IN GIF *gif) {
	//printf("Now function: GIFParserClear\n");
	if (gif->ImageDataHeader != NULL) {
		GIF_IMAGE_DATA *b = gif->ImageDataHeader;
		GIF_IMAGE_DATA *d = b->next;
		
		while (d != NULL) {
			GIF_DATA_SUB_BLOCK_NODE *p = d->one_frame_data.data_sub_block_buffer.header;
			GIF_DATA_SUB_BLOCK_NODE *q = p->next;

			while (q != NULL) {
				FreePool(p);
				p = q;
				q = q->next;
			}
			FreePool(p);

			if (b->local_color_table) {
				FreePool(b->local_color_table);
			}

			FreePool(b);
			b = d;
			d = d->next;
		}
		FreePool(b);
	}

	if (gif->CommentExtHeader) {
		GIF_COMMENT_EXT_DATA *c = gif->CommentExtHeader;
		GIF_COMMENT_EXT_DATA *v = c->next;

		while (v != NULL) {
			GIF_DATA_SUB_BLOCK_NODE *p = v->comment.data_sub_block_buffer.header;
			GIF_DATA_SUB_BLOCK_NODE *q = p->next;

			while (q != NULL) {
				FreePool(p);
				p = q;
				q = q->next;
			}
			FreePool(p);

			FreePool(c);
			c = v;
			v = v->next;
		} 
		FreePool(c);
	}

	if (gif->AppExtHeader) {
		GIF_APP_EXT_DATA *a = gif->AppExtHeader;
		GIF_APP_EXT_DATA *s = a->next;

		while (s != NULL) {
			GIF_DATA_SUB_BLOCK_NODE *p = s->app.data_sub_block_buffer.header;
			GIF_DATA_SUB_BLOCK_NODE *q = p->next;

			while (q != NULL) {
				FreePool(p);
				p = q;
				q = q->next;
			}
			FreePool(p);

			FreePool(a);
			a = s;
			s = s->next;
		} 
		FreePool(a);
	}

	if (gif->GraphicsExtHeader) {
		GIF_GRAPHICS_EXT_DATA *g = gif->GraphicsExtHeader;
		GIF_GRAPHICS_EXT_DATA *h = g->next;
		while (h != NULL) {
			FreePool(g);
			g = h;
			h = h->next;
		}
		FreePool(g);
	}

	if (gif->ComponentOrder.component != NULL) {
		FreePool(gif->ComponentOrder.component);
	}

	if (gif->GlobalColorTable) FreePool(gif->GlobalColorTable);
	if (gif) FreePool(gif);
	return TRUE;
}

BOOLEAN
_GIFParserMemRead(OUT VOID *dst, IN UINT8 **src, IN UINT32 size) {
	VOID *buffer = CopyMem(dst, (VOID *)*src, (UINTN)size);
	*src += size;
	if (buffer != NULL) return TRUE;
	return FALSE;
}

BOOLEAN
_HandleExtension(IN CHAR8 **src, IN UINT8 label, OUT GIF **gif) {
	UINT8 size = 0;

	switch (label&0xFF) {
	case 0x01:           // Plain Text Extension 
		break;

	case 0xFF:          // Application Extension  19bytes
	{
		if (gTailerPointer.app == NULL || gTailerPointer.app->next != NULL) {
			//printf("gTailerPointer.app error\n");
			return FALSE;
		}

		GIF_APP_EXT_DATA *new_app_node = (GIF_APP_EXT_DATA *)AllocatePool(sizeof(GIF_APP_EXT_DATA));
		new_app_node->next = NULL;
		new_app_node->app.data_sub_block_buffer.total_data_size = 0;
		new_app_node->app.data_sub_block_buffer.header = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
		GIF_DATA_SUB_BLOCK_NODE *p = new_app_node->app.data_sub_block_buffer.header;
		p->next = NULL;

		new_app_node->app.header.introducer = 0x21;
		new_app_node->app.header.label      = 0xFF;
		_GIFParserMemRead(&new_app_node->app.size, src, 12);
		_GIFParserMemRead(&size, src, 1);

		while (size > 0) {
			GIF_DATA_SUB_BLOCK_NODE *new_node = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
			new_node->data_size = size;
			new_node->next = NULL;
			new_app_node->app.data_sub_block_buffer.total_data_size += size;
			_GIFParserMemRead(new_node->data, src, size);
			_GIFParserMemRead(&size, src, 1);
			p->next = new_node;
			p = p->next;
		}
		
		if (size == 0) {
			new_app_node->app.terminator = 0;	
			gTailerPointer.app->next = new_app_node;
			gTailerPointer.app = new_app_node;
			_RecordComponentOrder(kAppExt, gif);
		}
		else {
			//printf("_HandleExtension[0xFF]: Load Error.\n");
			return FALSE;
		}
		return TRUE;
	}

	case 0xFE:          // Comment Extension 
	{
		if (gTailerPointer.comment == NULL || gTailerPointer.comment->next != NULL) {
			//printf("gTailerPointer.comment error\n");
			return FALSE;
		}
		GIF_COMMENT_EXT_DATA *new_com_node = (GIF_COMMENT_EXT_DATA *)AllocatePool(sizeof(GIF_COMMENT_EXT_DATA));
		new_com_node->next = NULL;
		new_com_node->comment.data_sub_block_buffer.total_data_size = 0;
		new_com_node->comment.data_sub_block_buffer.header = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
		GIF_DATA_SUB_BLOCK_NODE *p = new_com_node->comment.data_sub_block_buffer.header;
		p->next = NULL;

		new_com_node->comment.header.introducer = 0x21;
		new_com_node->comment.header.label      = 0xFE;
		_GIFParserMemRead(&size, src, 1);
		while (size > 0) {
			GIF_DATA_SUB_BLOCK_NODE *new_node = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
			new_node->data_size = size;
			new_node->next = NULL;
			new_com_node->comment.data_sub_block_buffer.total_data_size += size;
			_GIFParserMemRead(new_node->data, src, size);
			_GIFParserMemRead(&size, src, 1);
			p->next = new_node;
			p = p->next;
		}
		
		if (size == 0) {
			new_com_node->comment.terminator = 0;	
			gTailerPointer.comment->next = new_com_node;
			gTailerPointer.comment = new_com_node;
			_RecordComponentOrder(kCommentExt, gif);
		}
		else {
			//printf("_HandleExtension[0xFE]: Load Error.\n");
			return FALSE;
		}
		return TRUE;
	}

	case 0xF9:          // Graphic Control Extension 
	{
		if (gTailerPointer.graphics == NULL || gTailerPointer.graphics->next != NULL) {
			//printf("gTailerPointer.graphics error\n");
			return FALSE;
		}
		
		GIF_GRAPHICS_EXT_DATA *new_graphics_node = (GIF_GRAPHICS_EXT_DATA *)AllocatePool(sizeof(GIF_GRAPHICS_EXT_DATA));  // one frame
		new_graphics_node->next = NULL;
		new_graphics_node->graphics.header.introducer = 0x21;
		new_graphics_node->graphics.header.label      = 0xF9;
		_GIFParserMemRead(&new_graphics_node->graphics.size, src, 6);  	// Graphics control extension 
		gTailerPointer.graphics->next = new_graphics_node;
		gTailerPointer.graphics = new_graphics_node;
		_RecordComponentOrder(kGraphicsExt, gif);

		return TRUE;
	}

	default:
		break;
	}

	return TRUE;
}

BOOLEAN
_HandleImageData(IN CHAR8 **src, OUT GIF **gif) {
 	UINT32 size = 0;
	*src -= 1;  // already read ','  -> 0x2C

	if (gTailerPointer.image == NULL || gTailerPointer.image->next != NULL) {
		//printf("gTailerPointer.image error\n");
		return FALSE;
	}

	GIF_IMAGE_DATA *new_image_node = (GIF_IMAGE_DATA *)AllocatePool(sizeof(GIF_IMAGE_DATA));
	new_image_node->next = NULL;
	new_image_node->local_color_table = NULL;
	new_image_node->one_frame_data.data_sub_block_buffer.total_data_size = 0;
	new_image_node->one_frame_data.data_sub_block_buffer.header = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
	GIF_DATA_SUB_BLOCK_NODE *p = new_image_node->one_frame_data.data_sub_block_buffer.header;
	p->next = NULL;

	_GIFParserMemRead(&new_image_node->image_descriptor, src, 10);       // Image descriptor
	if ( (new_image_node->image_descriptor.flag_color_table&0x1) == 1 ) {    					// local color table
		UINT16 local_color_table_amount = 1 << ((new_image_node->image_descriptor.flag_table_size&0x7) + 1);
		new_image_node->local_color_table = (GIF_COLOR_TABLE *)AllocatePool(sizeof(GIF_COLOR_TABLE) * local_color_table_amount );
		_GIFParserMemRead(new_image_node->local_color_table, src, sizeof(GIF_COLOR_TABLE) * local_color_table_amount);
	}
	_GIFParserMemRead(&new_image_node->one_frame_data.LZW_Minimum_Code, src, 1);  // one frame data: LZW_Minimum_Code
	_GIFParserMemRead(&size, src, 1);																			     // size is data_size
	while (size > 0) {																												   	     // one frame data: data_sub_block_buffer
		GIF_DATA_SUB_BLOCK_NODE *new_node = (GIF_DATA_SUB_BLOCK_NODE *)AllocatePool(sizeof(GIF_DATA_SUB_BLOCK_NODE));
		new_node->next = NULL;
		new_node->data_size = (UINT8)size;
		new_image_node->one_frame_data.data_sub_block_buffer.total_data_size += size;
		_GIFParserMemRead(new_node->data, src, size);
		p->next = new_node;
		p = p->next;
		_GIFParserMemRead(&size, src, 1);
	}

	if (size == 0) {  // one frame data: terminator, the last size is terminator = 0
		new_image_node->one_frame_data.terminator = 0;		
		gTailerPointer.image->next = new_image_node;
		gTailerPointer.image = new_image_node;
		(*gif)->FramesCount = (*gif)->FramesCount + 1;
		_RecordComponentOrder(kImageData, gif);
	}
	else {
		//printf("_HandleImageData: Load Image Data Error.\n");
		return FALSE;
	}

	return TRUE;
}

VOID
_RecordComponentOrder(IN GIF_COMPONENT key, OUT GIF **gif) {
	GIF_COMPONENT *tmp = NULL;
	if (gAllocComponentCount*ALLOC_COMPONENT_AMOUNT - (*gif)->ComponentOrder.size < 10) {
		gAllocComponentCount = gAllocComponentCount + 1;
		tmp = (GIF_COMPONENT *)ReallocatePool((gAllocComponentCount-1) * ALLOC_COMPONENT_SIZE, gAllocComponentCount * ALLOC_COMPONENT_SIZE, (*gif)->ComponentOrder.component);
		if (!tmp) {
			DEBUG((EFI_D_INFO, "ReallocatePool failed\n"));
			return ;
		}
	}
	(*gif)->ComponentOrder.component[(*gif)->ComponentOrder.size] = key;
	++((*gif)->ComponentOrder.size);
}

VOID
_PrintBuffer(IN CHAR8 *buffer, IN UINTN len) {
	EFI_FILE_HANDLE                 WriteFileHandle;
	EFI_FILE_PROTOCOL               *FileRoot = NULL;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = NULL;

	gBS->LocateProtocol(
						      &gEfiSimpleFileSystemProtocolGuid, 
						      NULL,
					     	  (VOID **)&SimpleFileSystem
                  );
	SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileRoot);
	FileRoot->Open(FileRoot, 
								 &WriteFileHandle, 
								 (CHAR16*)L"log.gif",
								 EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
								 0
								);
	WriteFileHandle->Write(WriteFileHandle, &len, (VOID *)buffer);
	WriteFileHandle->Close(WriteFileHandle);
}

EFI_STATUS
EFIAPI
_LoadFile2Buffer (
  IN      CHAR16           *FileName,
  IN OUT  UINT8            **FileDataBuffer,
  IN OUT  UINT32           *DataLength
)
{
	EFI_STATUS                      Status;
  EFI_FILE_HANDLE                 ReadFileHandle;
  EFI_FILE_PROTOCOL               *FileRoot = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = NULL;
  EFI_FILE_INFO                   *ReadFileInfo = NULL;
	DEBUG((EFI_D_INFO, "_LoadFile2Buffer Start\n"));

  Status = gBS->LocateProtocol(
						      &gEfiSimpleFileSystemProtocolGuid, 
						      NULL,
					     	  (VOID **)&SimpleFileSystem
                  );
  if (EFI_ERROR(Status)) {
		  DEBUG((EFI_D_INFO, "Not Found gEfiSimpleFileSystemProtocolGuid!\n"));
      return Status;
	}
  
  Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileRoot);
  if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_INFO, "OpenVolume error\n"));
    return Status;	
	}

  Status = FileRoot->Open(
						           FileRoot, 
							         &ReadFileHandle, 
							         FileName,
							         EFI_FILE_MODE_READ, 
							         0 
                       );
  if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_INFO, "Open file error\n"));
    return Status;
  }
  
  ReadFileInfo = ShellGetFileInfo( (SHELL_FILE_HANDLE)ReadFileHandle);

  *DataLength = (UINT32)(ReadFileInfo->FileSize);
  *FileDataBuffer = (UINT8 *)AllocatePool(*DataLength);

  Status = ReadFileHandle->Read(ReadFileHandle,
                                (UINTN *)DataLength,
                                *FileDataBuffer
                               	);
  if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_INFO, "Read file error\n"));
    return Status;
  }	
  
  ReadFileHandle->Close(ReadFileHandle);
  FileRoot->Close(FileRoot);

  return Status;
}