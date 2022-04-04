#ifndef BOUNDS_H 
#define BOUNDS_H 
#include "gba.h"

typedef struct vec2 {
  int x; 
  int y; 
} Vect2D; 

// axis aligned bounding box 
typedef struct bbox {
  int x; 
  int y; 
  int w; 
  int h; 
} BBox; 

int bbox_intersects(BBox* a, BBox* b);
int bbox_bottom_crossed(BBox* box); 
int bbox_right_crossed(BBox* box);
int bbox_left_crossed(BBox* box);

void bbox_set(BBox* box, int x, int y, int w, int h);
void bbox_move(BBox* box, int dx, int dy);
void bbox_vmove(BBox* box, Vect2D diff);  
void bbox_rand_coin(BBox* box, int w, int h); 
void bbox_center(BBox* box, int width, int height);
void bbox_center_top(BBox* box, int width, int height);
void bbox_center_bottom(BBox* box, int width, int height);
void bbox_fill(BBox* box, u16 color);
void bbox_image(BBox* box, const u16* img);

#endif 