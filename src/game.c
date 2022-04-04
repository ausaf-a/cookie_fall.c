#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
#include "images/garbage.h"
#include "images/cookie.h"
#include "images/milk.h"
#include "images/coin.h"
#include "images/heart.h"
#include "images/kek.h"
#include "images/nice.h"
#include "images/start.h"

#define BG BLACK
#define FG WHITE

// player dimensions
#define PW 15
#define PH 15

// target dimensions
#define TW 15
#define TH 25

// coin dimensions
#define CW 10
#define CH 10

void game_init(GameState *game)
{
  drawFullScreenImageDMA(start); 
  // reset game
  game->status = START;
  game->score = 0;
  game->health = 3;

  // reset player
  bbox_center_top(&game->player.bounds, PW, PH);
  game->player.color = GREEN;
  game->player.vel.y = 1; 
  // create powerups
  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    bbox_rand_coin(&game->objects[i].bounds, CW, CH);
    game->objects[i].type = (randint(0, 100) >= 50) ? COIN : HEART;
    game->objects[i].active = 1;
  }

  //create target
  bbox_center_bottom(&game->target.bounds, TW, TH);
  game->target.color = WHITE;
  game->target.vel.y = 0;
  game->target.vel.x = 2;
  game->currentButtons = BUTTONS;
  game->previousButtons = BUTTONS;
}

void game_play(GameState *game)
{
  // undraw old objects
  bbox_fill(&game->player.bounds, BG);
  bbox_fill(&game->target.bounds, BG);

  // move the player
  bbox_vmove(&game->player.bounds, game->player.vel);

  // move the target
  bbox_vmove(&game->target.bounds, game->target.vel); 
  

  if (vBlankCounter == 300) {
    if(game->player.vel.y > 0) {
      game->player.vel.y++; 
    } else if (game) {
      game->player.vel.y--;  
    } 
  }

  /* Check collision with target */
  if (bbox_intersects(&game->player.bounds, &game->target.bounds))
  {
    game->score++; 
    bbox_center_top(&game->player.bounds, game->player.bounds.w, game->player.bounds.h);
    for (int i = 0; i < MAX_OBJECTS; i++) {
      if (!game->objects[i].active) {
        game->objects[i].active = 1; 
        game->objects[i].type = (randint(0, 5) < game->health) ? COIN : HEART;
      }
    }
  }

  /* check if player went off screen */
  if (bbox_bottom_crossed(&game->player.bounds))
  {
    game->health--;
    bbox_center_top(&game->player.bounds, game->player.bounds.w, game->player.bounds.h);
    for (int i = 0; i < MAX_OBJECTS; i++) {
      if (!game->objects[i].active) {
          game->objects[i].active = 1; 
         game->objects[i].type = (randint(0, 5) < game->health) ? COIN : HEART;
      }
    }
  }

  if(game->health <= 0) {
    if (game->score < 3) {
      game->status = LOSE;
      drawFullScreenImageDMA(kek);
    } else {
      game->status = WIN; 
      drawFullScreenImageDMA(nice); 
    }
    return; 
  }

  if (bbox_right_crossed(&game->target.bounds))
  {
    game->target.vel.x *= -1;
    // game->target.bounds.x = WIDTH - TW - 1;
  }
  if (bbox_left_crossed(&game->target.bounds))
  {
    game->target.vel.x *= -1;
    // game->target.bounds.x = 1;
  }

  if (KEY_DOWN(BUTTON_UP, game->currentButtons))
  {
    // game->player.bounds.y--;
  }
  if (KEY_DOWN(BUTTON_DOWN, game->currentButtons))
  {
    game->player.bounds.y++;
  }
  if (KEY_DOWN(BUTTON_LEFT, game->currentButtons))
  {
    game->player.bounds.x--;
  }
  if (KEY_DOWN(BUTTON_RIGHT, game->currentButtons))
  {
    game->player.bounds.x++;
  }

  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    if (game->objects[i].active)
    {
      bbox_fill(&game->objects[i].bounds, BG);

      if (game->objects[i].type == COIN)
      {
        bbox_image(&game->objects[i].bounds, coin);
      }
      else if (game->objects[i].type == HEART)
      {
        bbox_image(&game->objects[i].bounds, heart);
      }

      // collided with a power up 
      if (bbox_intersects(&game->objects[i].bounds, &game->player.bounds))
      {
        if (game->objects[i].type == COIN)
        {
          game->score+=2; 
        }
        else if (game->objects[i].type == HEART)
        {
          game->health++; 
        } 
        bbox_fill(&game->objects[i].bounds, BG);
        game->objects[i].active = 0;

        // give it a new position for next time
        bbox_rand_coin(&game->objects[i].bounds, CW, CH);
      }
    }
  }

  bbox_image(&game->player.bounds, cookie);
  bbox_image(&game->target.bounds, milk);
}

int main(void)
{

  REG_DISPCNT = MODE3 | BG2_ENABLE;

  GameState game;
  fillScreenDMA(BG);
  game_init(&game);

  char scoreMsg[16];
  char healthMsg[16];

  while (1)
  {
    game.currentButtons = BUTTONS; // Load the current state of the buttons

    waitForVBlank();

    // reset game
    if (KEY_JUST_PRESSED(BUTTON_SELECT, game.currentButtons, game.previousButtons))
    {
      game_init(&game); // restart
    }

    switch (game.status)
    {
    case START:
      if (KEY_JUST_PRESSED(BUTTON_START, game.currentButtons, game.previousButtons))
      {
        game.status = PLAY;
        fillScreenDMA(BLACK);  
      }

      break;
    case PLAY:
      snprintf(scoreMsg, 16, "Score: %d", game.score);
      snprintf(healthMsg, 16, "Health: %d", game.health);
      drawRectDMA(0, 0, 80, 20, BG);
      drawRectDMA(HEIGHT - 10, 0, 30, 30, BG);
      drawString(10, 0, scoreMsg, FG);
      drawRectDMA(0,0,game.health*10,10, RED); 
      drawString(0, 0, healthMsg, FG);

      game_play(&game);
      
      break;
      
    case WIN:
      drawString(100, 70, scoreMsg, BLACK);
      drawString(110, 60, "BACKSPACE to try again", BLACK);
      // state = ?
      break;
    case LOSE:
      drawString(100, 120, scoreMsg, BLACK);
      drawString(110, 100, "BACKSPACE to try again", BLACK);
      // state = ?
      break;
    }

    game.previousButtons = game.currentButtons; // Store the current state of the buttons
  }

  return 0;
}
