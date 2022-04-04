#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
  while(SCANLINECOUNTER > HEIGHT); // wait until vblank is over
  while(SCANLINECOUNTER < HEIGHT); // wait until vdraw is over 
  ++vBlankCounter; // inc vblank counter
}

static int __qran_seed = 42;
static int qran(void) {
  __qran_seed = 1664525 * __qran_seed + 1013904223;
  return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

void setPixel(int row, int col, u16 color) {
  videoBuffer[row*WIDTH + col] = color; 
}

void drawRectDMA(int row, int col, int width, int height, volatile u16 color) {
    unsigned short n = width; 
    for (int i = 0; i < height; i++){
      volatile void* start = (void*) (videoBuffer + (row + i)*WIDTH + col); 
      DMA[3].src = &color; 
      DMA[3].dst = start; 
      DMA[3].cnt = DMA_ON | DMA_16 | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | n;
    }
}

void drawFullScreenImageDMA(const u16 *image) {
  unsigned short n = WIDTH*HEIGHT;
  DMA[3].src = image; 
  DMA[3].dst = videoBuffer; 
  DMA[3].cnt = DMA_ON | DMA_16 | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | n;
}

void drawImageDMA(int row, int col, int width, int height, const u16 *image) {
  for (int i = 0; i < height; i++) {    
    volatile void* dst = (void*) (videoBuffer + (row + i)*WIDTH + col);
    volatile void* src = (void*) (image + i*width); 
    DMA[3].src = src; 
    DMA[3].dst = dst; 
    DMA[3].cnt = DMA_ON | DMA_16 | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT | width; 
  }
}

void fillScreenDMA(volatile u16 color) {
  unsigned short n = WIDTH*HEIGHT;
  DMA[3].src = &color; 
  DMA[3].dst = videoBuffer; 
  DMA[3].cnt = DMA_ON | DMA_16 | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT | n;
}

void drawChar(int row, int col, char ch, u16 color) {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
        setPixel(row + j, col + i, color);
      }
    }
  }
}

void drawString(int row, int col, char *str, u16 color) {
  while (*str) {
    drawChar(row, col, *str++, color);
    col += 6;
  }
}

void drawCenteredString(int row, int col, int width, int height, char *str,
                        u16 color) {
  u32 len = 0;
  char *strCpy = str;
  while (*strCpy) {
    len++;
    strCpy++;
  }

  u32 strWidth = 6 * len;
  u32 strHeight = 8;

  int new_row = row + ((height - strHeight) >> 1);
  int new_col = col + ((width - strWidth) >> 1);
  drawString(new_row, new_col, str, color);
}
