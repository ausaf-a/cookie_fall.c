#include "bounds.h"

// Returns true if the two bounding boxes intersect
int bbox_intersects(BBox* a, BBox* b) {
  return ((a->x > b->x + b->w || b->x > a->x + a->w) || (a->y > b->y + b->h || b->y > a->y + a->w)) ? 0 : 1; 
}

int bbox_bottom_crossed(BBox* box) {
  return (box->y > HEIGHT); 
}

int bbox_left_crossed(BBox* box) {
  return (box->x < 0); 
}

int bbox_right_crossed(BBox* box) {
  return (box->x + box->w > WIDTH); 
}

void bbox_set(BBox* box, int x, int y, int w, int h) {
  box->x = x; 
  box->y = y; 
  box->w = w; 
  box->h = h;   
}

void bbox_move(BBox* box, int dx, int dy) {
  box->x += dx; 
  box->y += dy;  
}

void bbox_vmove(BBox* box, Vect2D diff) {
  box->x += diff.x; 
  box->y += diff.y; 
}

// place box randomly on screen
void bbox_rand_coin(BBox* box, int w, int h) {
  bbox_set( box, 
            randint(10, WIDTH - w - 10), 
            randint(30, HEIGHT - h - 30),
            w, 
            h 
  ); 
}

void bbox_center(BBox* box, int width, int height) {
  bbox_set(box, (WIDTH-width)/2, (HEIGHT-height)/2, width, height); 
}

void bbox_center_top(BBox* box, int width, int height) {
  bbox_set(box, (WIDTH-width)/2, 0, width, height); 
}

void bbox_center_bottom(BBox* box, int width, int height) {
  bbox_set(box, (WIDTH-width)/2, HEIGHT - height, width, height); 
}

void bbox_fill(BBox* box, u16 color) {
  drawRectDMA(box->y, box->x, box->w, box->h, color); 
}

void bbox_image(BBox* box, const u16* img) {
  drawImageDMA(box->y, box->x, box->w, box->h, img); 
}
