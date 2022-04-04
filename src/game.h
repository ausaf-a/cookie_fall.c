#ifndef GAME_H
#define GAME_H
#include "gba.h"
#include "bounds.h"

#define MAX_OBJECTS 5

typedef struct player {
  BBox bounds; 
  Vect2D vel; 
  u16 color; 
} Player; 


typedef enum 
{
  COIN, 
  HEART, 
} EntityType; 

typedef struct entity {
  BBox bounds;
  Vect2D vel;
  int active;
  EntityType type;  
  u16 color; 
} Entity; 

typedef enum
{
  START,
  PLAY,
  WIN,
  LOSE,
} GBAState;

typedef struct gamestate {
  GBAState status;
  int score;
  int health;  
  Player player; 
  Entity target; 
  Entity objects[MAX_OBJECTS]; 
  u32 currentButtons; 
  u32 previousButtons;  
} GameState; 


 
void game_init(GameState* game); 
void game_play(GameState* game);

#endif
