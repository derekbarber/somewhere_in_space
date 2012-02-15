/*
  Somewhere in Space (c) 1997 by Derek Barber
*/

//// I N C L U D E S //////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <allegro.h>

#include "space.h"

//// D E F I N E S ////////////////////////////////////////////////

#define height       400            // screen's visible height
#define width        640            // screen's visible width
#define v_height     800            // screen's virtual height
#define v_width      640            // screen's virtual width
#define num_stars    35             // the number of stars in the starfield
#define plane1       1              // the speed of star plane #1
#define plane2       2              // the speed of star plane #2
#define plane3       3              // the speed of star plane #3
#define max_missle   40             // the max number of missles
#define back_images  3              // the number of background images

//// P R O T O T Y P E S //////////////////////////////////////////

int initialize(void);
int key_checker(void);
void load_graphics(void);
void shutdown(void);
void init_starfield(void);
void move_background(void);
void move_player(void);
void add_player_missle(void);
void move_player_missle(void);
void move_enemy(void);
void add_enemy_missle(int num);
void move_enemy_missle(void);
void show_status(void);
void move_boss(void);
void add_boss_missle(void);
void move_boss_missle(void);
int collide(int x1, int y1, int x2, int y2, int w, int h);
void end_level(void);
int main_menu(void);
void credits(void);
void story(void);
void end_game(void);
void initialize_variables(void);

//// S T R U C T U R E S ///////////////////////////////////////////

typedef struct ship_typ
  {
    int x, y;
    int frame;
    int shields;
  } SHIP;

typedef struct missle_typ
  {
    int x, y;
    int frame;
    int status;
    int timer;
  } MISSLE;

typedef struct back_typ
  {
    int x, y;
    int frame;
    int image;
    int delay;
    int speed;
  } BACKIMG;

typedef struct star_typ
  {
    int x, y;        // position of star
    int plane;       // which plane the star is in
    int color;       // the color of the star
  } STAR;


//// G L O B A L S //////////////////////////////////////////////////

DATAFILE *data;            // a pointer to the datafile structure
BITMAP *ship;
BITMAP *Vscreen, *Hscreen;  // a bitmap representing the visible portion of
                            // the screen and a bitmap representing the
                            // hidden portion of the screen, respectively.
SAMPLE *intro;
MIDI *theme;

int pmissle_delay, emissle_delay[2], level, score, bdie, bdie_frame, bdie_delay;
int lives, pdie, edie[2], edie_frame[2], pdie_frame, edie_delay[2], pdie_delay;
int quit=0, enemy_num, bmissle_delay, boss_delay;
int e_movement, e_firerate, enemy_done[2], die_delay;

STAR stars[num_stars];      // an array representing the starfield

SHIP pship, eship[2], boss;
BACKIMG back[back_images];
MISSLE pmissles[max_missle], emissles[max_missle], bmissles[max_missle];

BITMAP *player[3], *enemy[24], *boss1[3], *splanet3, *splanet4;
BITMAP *boss2[3], *boss3[3], *boss4[3], *boss5[3], *splanet[3];
BITMAP *lplanet, *explosion[6], *pmissle[2], *emissle[2], *bmissle;
BITMAP *asteroid1, *asteroid2, *asteroid3, *splanet2[2], *status;
BITMAP *portal, *gexplosion[5];


//// M A I N /////////////////////////////////////////////////////////

int main()
{
  int error, option;

  srandom(time(NULL));

  error = initialize();

  if (error)
    {
      printf("Error initializing game - now terminating");
      exit(1);
    }

  play_sample(data[START].dat, 200, 125, 1000, 0);
  play_memory_fli(data[INTRO].dat, screen, 0, key_checker);

  readkey();
  fade_out(1);
  rest(500);

  load_graphics();

  while (option != 4)
    {
      option = main_menu();

 	    if (option==2)
    	  credits();

      if (option==3)
        story();

	  	if (option==1)
			  {
  			  clear_keybuf();
 			    initialize_variables();
  				if (random()%2)
  				  play_midi(data[THEME1].dat, 1);
  				else
  				  play_midi(data[THEME2].dat, 1);

  				init_starfield();

  				status = create_bitmap(320, 30);
				  clear(status);
				  blit(data[STATUS].dat, status, 0, 0, 0, 0, 320, 30);

				  clear(Hscreen);
				  move_background();
				  blit(status, Hscreen, 0, 0, 0, 210, 320, 30);
				  blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);

				  fade_in(data[PAL].dat,1);

				  textout_centre(Vscreen, data[FONT4].dat, "LEVEL ONE", 160, 60, 50);
				  textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 150);
				  readkey();

				  while (!key[KEY_ESC] && !quit)
			    {
			      clear(Hscreen);
			      move_background();
      			move_player_missle();
			      move_player();

			      if (enemy_num<=30)
      		  {
		          move_enemy();
		          move_enemy_missle();
              if (die_delay > 0)
                die_delay--;
              if (die_delay <= 0)
                {
	  		          if ((collide(pship.x, pship.y, eship[0].x, eship[0].y, 25, 25)) && !edie[0] && !pdie)
  	    	        {
  		  						play_sample(data[BANG].dat, 100, 125, 800, 0);
      	    	      edie[0]=1;
        		  	    pdie=1;
          		  	  pdie_frame = 0;
	          		    pdie_delay = 0;
  	          		  edie_frame[0] = 0;
    	          		edie_delay[0] = 0;
                    die_delay = 100;
	  	    	      }
				          if ((collide(pship.x, pship.y, eship[1].x, eship[1].y, 25, 25)) && !edie[1] && !pdie)
    	  		      {
  								  play_sample(data[BANG].dat, 100, 125, 800, 0);
		        	      edie[1]=1;
    		      	    pdie=1;
        		    	  pdie_frame = 0;
            		  	pdie_delay = 0;
			              edie_frame[1] = 0;
  			            edie_delay[1] = 0;
                    die_delay = 100;
		    	        }
                }
	        	}
 			      else
        		{
          	  move_boss_missle();
		          move_boss();
    		      if ((collide(pship.x, pship.y, boss.x, boss.y, 40, 40)) && !pdie && !bdie)
        	    {
  						  play_sample(data[BANG].dat, 40, 125, 800, 0);
            	  pdie=1;
	              pdie_frame = 0;
  	            pdie_delay = 0;
    	        }
      		  }

		        show_status();
			      blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
      			rest(2);
    			}
    			stop_midi();
          if (quit==1)
            {
              end_game();
            }
			    if (quit==2)
      			{
			        play_sample(data[DIE].dat, 255, 125, 1000, 0);
      			  textout_centre(Vscreen, data[FONT4].dat, "YOU ARE", 160, 50, 110);
			        textout_centre(Vscreen, data[FONT4].dat, "TERMINATED", 160, 110, 110);
			        clear_keybuf();
					    readkey();
			        stop_sample(data[DIE].dat);
			      }
			    fade_out(1);
				}
	  }

  shutdown();

  return (0);
}

END_OF_MAIN()


//// I N I T I A L I Z E ///////////////////////////////////////////////

int initialize(void)
{
    int error, i;
    
    if (allegro_init() != 0)
        return 1;
    
    
    install_keyboard();
    
    
    error = install_timer();
    if (error)
        return(1);
    
    error = install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    if (error)
        return(1);
    
    
    if (set_gfx_mode(GFX_AUTODETECT, width, height, 0, 0) != 0) {
        if (set_gfx_mode(GFX_SAFE, width, height, 0, 0) != 0) {
            set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
            allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
            return 1;
        }
    }
    
    //error = set_gfx_mode(GFX_MODEX, width, height, v_width, v_height);
    //error = set_gfx_mode(GFX_AUTODETECT, width, height, v_width, v_height);
    //if (error)
    //return(1);
    
    acquire_screen();
    
    Vscreen = create_bitmap(width, height);
    clear(Vscreen);
    Hscreen = create_bitmap(width, height);
    clear(Hscreen);
    
    Vscreen = create_sub_bitmap(screen, 0, 0, width, height);
    Hscreen = create_sub_bitmap(screen, 0, height, width, height);
    
    data = load_datafile("space.dat");
    if (!data)
    return(1);
    
    text_mode(-1);
    
    release_screen();
    
    
    return(0);
}
                                

//// M A I N   M E N U //////////////////////////////////////////////

int main_menu(void)
{
  int option = 1;
  char input;

  clear(Vscreen);

  blit(data[BACK].dat, Vscreen, 0, 0, 0, 0, 320, 240);

  textout_centre(Vscreen, data[FONT2].dat, "Somewhere In Space", 160, 3, 50);
  textout_centre(Vscreen, data[FONT3].dat, "Version 1.0", 160, 35, 70);

  rectfill(Vscreen, 120, 70, 200, 90, 150);
  textout_centre(Vscreen, data[FONT5].dat, "New Game", 160, 70, 140);
  textout_centre(Vscreen, data[FONT5].dat, "Credits", 160, 100, 140);
  textout_centre(Vscreen, data[FONT5].dat, "Story", 160, 130, 140);
  textout_centre(Vscreen, data[FONT5].dat, "Exit", 160, 160, 140);

  fade_in(data[PAL].dat, 10);

  while (option!=6)
    {
      if (keypressed())
      {
    	  if (key[KEY_DOWN])
          {
            if (++option==5)
              option=1;
          }
         if (key[KEY_UP])
          {
            if (--option==0)
              option=4;
          }
        if (key[KEY_ENTER])
          {
  		  		fade_out(10);
            return(option);
          }
    	  blit(data[BACK].dat, Hscreen, 0, 0, 0, 0, 320, 240);
  	  	textout_centre(Hscreen, data[FONT2].dat, "Somewhere In Space", 160, 3, 50);
    	  textout_centre(Hscreen, data[FONT3].dat, "Version 1.0", 160, 35, 70);

        if (option==1)
    		  rectfill(Hscreen, 120, 70, 200, 90, 150);
   			else if (option==2)
          rectfill(Hscreen, 125, 100, 195, 120, 150);
   			else if (option==3)
          rectfill(Hscreen, 135, 130, 185, 150, 150);
   			else if (option==4)
          rectfill(Hscreen, 140, 160, 180, 180, 150);

    		textout_centre(Hscreen, data[FONT5].dat, "New Game", 160, 70, 140);
    		textout_centre(Hscreen, data[FONT5].dat, "Credits", 160, 100, 140);
    		textout_centre(Hscreen, data[FONT5].dat, "Story", 160, 130, 140);
	     	textout_centre(Hscreen, data[FONT5].dat, "Exit", 160, 160, 140);
        blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
        clear_keybuf();
      }
    }
}


//// E N D   L E V E L //////////////////////////////////////////////

void end_level(void)
{
  int i;

  enemy_num=0;
  bdie = 0;
  level++;
  if (lives < 6)
  	lives++;
  clear(Hscreen);
  move_background();
  move_player_missle();
  move_player();
  show_status();
  blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
  rest(300);
  fade_out(1);
  clear(Hscreen);
  move_background();
  blit(status, Hscreen, 0, 0, 0, 210, 320, 30);
  blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
  fade_in(data[PAL].dat,3);
  pship.x = 145;
  pship.y = 180;
  pship.frame = 0;
  enemy_done[0] = 1;
  enemy_done[1] = 1;
  if (level <= 4)
    {
    	edie[0] = 0;
  	  eship[0].frame=(random()%15);
  		eship[0].x = (random()%250+30);
 		  eship[0].y = -50;
		  eship[0].shields = 100;
    }
  else if (level > 4)
    {
      eship[0].frame=(random()%24);
      eship[0].y = -50;
      eship[0].x = (random()%100+150);
      eship[0].shields = 100;
      eship[1].frame=(random()%24);
      eship[1].y = -70;
      eship[1].x = (random()%100+50);
      eship[1].shields = 100;
    }

  for (i=0; i<max_missle; i++)
    {
      pmissles[i].status=0;
      emissles[i].status=0;
      bmissles[i].status=0;
    }
}

//// M O V E   E N E M Y /////////////////////////////////////////////

void move_enemy(void)
{

//// IF ENEMY DIES THEN SET APPROPRIATE VARIABLES
  if (eship[0].shields <=0 && !edie[0])
    {
      play_sample(data[BANG].dat, 40, 125, 1000, 0);
      edie[0] = 1;
      score += 10;
      edie_frame[0] = 0;
      edie_delay[0] = 0;
    }
  if (eship[1].shields <=0 && !edie[1])
    {
      play_sample(data[BANG].dat, 40, 125, 1000, 0);
      edie[1] = 1;
      score += 10;
      edie_frame[1] = 0;
      edie_delay[1] = 0;
    }

  ////// LEVEL ONE THROUGH FOUR ENEMY //////////

  switch (level)
    {
      case 1: case 2: case 3: case 4:
        {
          if (!edie[0])
            {
              eship[0].y+=e_movement;
              if (eship[0].x > pship.x)
                eship[0].x--;
              else
                eship[0].x++;

              emissle_delay[0]++;

              if ((emissle_delay[0]>e_firerate) && (eship[0].y>0))
                add_enemy_missle(0);

              if (++eship[0].frame==15)
                eship[0].frame=12;
              else if (eship[0].frame==12)
                eship[0].frame=9;
              else if (eship[0].frame==9)
                eship[0].frame=6;
              else if (eship[0].frame==6)
                eship[0].frame=3;
              else if (eship[0].frame==3)
                eship[0].frame=0;
              draw_sprite(Hscreen, enemy[eship[0].frame], eship[0].x, eship[0].y);
              if (eship[0].y>210)
                {
                  eship[0].frame=(random()%15);
                  eship[0].y = -50;
                  eship[0].x = (random()%250+30);
                  eship[0].shields = 100;
                  enemy_num+=2;
                }
            }
          else
            {
              draw_sprite(Hscreen, explosion[edie_frame[0]], eship[0].x, eship[0].y);
              edie_delay[0]++;
              if (edie_delay[0]>=4)
                {
                  edie_delay[0]=0;
                  edie_frame[0]++;
                  if (edie_frame[0] == 6)
                   {
                     edie[0] = 0;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -50;
                     eship[0].shields = 100;
      							 enemy_num+=2;
                   }
                }
            }
        } break;


  ////// LEVEL FIVE THROUGH NINE ENEMY //////////

      case 5: case 6: case 7: case 8: case 9:
        {
          if (!edie[0] && enemy_done[0])
            {
              eship[0].y+=e_movement;
              if (eship[0].x > pship.x)
                eship[0].x--;
              else
                eship[0].x++;

              emissle_delay[0]++;

              if ((emissle_delay[0]>e_firerate) && (eship[0].y>0))
                add_enemy_missle(0);

              if (++eship[0].frame==24)
                eship[0].frame=21;
              else if (eship[0].frame==21)
                eship[0].frame=18;
              else if (eship[0].frame==18)
                eship[0].frame=15;
              else if (eship[0].frame==15)
                eship[0].frame=12;
              else if (eship[0].frame==12)
                eship[0].frame=9;
              else if (eship[0].frame==9)
                eship[0].frame=6;
              else if (eship[0].frame==6)
                eship[0].frame=3;
              else if (eship[0].frame==3)
                eship[0].frame=0;
              draw_sprite(Hscreen, enemy[eship[0].frame], eship[0].x, eship[0].y);
              if (eship[0].y>210)
                {
                  eship[0].frame=(random()%24);
                  eship[0].y = -50;
                  eship[0].x = (random()%250+30);
                  eship[0].shields = 100;
                  if (++enemy_num >= 30)
                    enemy_done[0] = 0;
                }
            }
          else if (enemy_done[0])
            {
              draw_sprite(Hscreen, explosion[edie_frame[0]], eship[0].x, eship[0].y);
              edie_delay[0]++;
              if (edie_delay[0]>=4)
                {
                  edie_delay[0]=0;
                  edie_frame[0]++;
                  if (edie_frame[0] == 6)
                   {
                     edie[0] = 0;
                     eship[0].frame=(random()%24);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -50;
                     eship[0].shields = 100;
                     if (++enemy_num >= 30)
                       enemy_done[0] = 0;
                   }
                }
            }
          if (!edie[1] && enemy_done[1])
            {
              eship[1].y+=e_movement;
              if (eship[1].x > pship.x)
                eship[1].x--;
              else
                eship[1].x++;

              emissle_delay[1]++;

              if ((emissle_delay[1]>e_firerate) && (eship[1].y>0))
                add_enemy_missle(1);

              if (++eship[1].frame==24)
                eship[1].frame=21;
              else if (eship[1].frame==21)
                eship[1].frame=18;
              else if (eship[1].frame==18)
                eship[1].frame=15;
              else if (eship[1].frame==15)
                eship[1].frame=12;
              else if (eship[1].frame==12)
                eship[1].frame=9;
              else if (eship[1].frame==9)
                eship[1].frame=6;
              else if (eship[1].frame==6)
                eship[1].frame=3;
              else if (eship[1].frame==3)
                eship[1].frame=0;
              draw_sprite(Hscreen, enemy[eship[1].frame], eship[1].x, eship[1].y);
              if (eship[1].y>210)
                {
                  eship[1].frame=(random()%18);
                  eship[1].y = -50;
                  eship[1].x = (random()%250+30);
                  eship[1].shields = 100;
                  if (++enemy_num >= 30)
                      enemy_done[1] = 0;
                }
            }
          else if (enemy_done[1])
            {
              draw_sprite(Hscreen, explosion[edie_frame[1]], eship[1].x, eship[1].y);
              edie_delay[1]++;
              if (edie_delay[1]>=4)
                {
                  edie_delay[1]=0;
                  edie_frame[1]++;
                  if (edie_frame[1] == 6)
                   {
                     edie[1] = 0;
                     eship[1].frame=(random()%18);
                     eship[1].x = (random()%250+30);
                     eship[1].y = -50;
                     eship[1].shields = 100;
                     if (++enemy_num >= 30)
                      enemy_done[1] = 0;
                   }
                }
            }
        } break;
    }
}

//// M O V E   B O S S ////////////////////////////////////////////////

void move_boss(void)
{
  if (boss.shields <=0 && !bdie)
    {
      play_sample(data[BANG].dat, 150, 125, 800, 0);
      bdie = 1;
      score += 100;
      bdie_frame = 0;
      bdie_delay = 0;
    }

  switch (level)
    {
      case 1:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<40) && (random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss1[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
                     e_firerate-=2;
                     e_movement++;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL TWO", 160, 60, 120);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 160);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;

      case 2:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<150) && (random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss2[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
                     e_firerate-=2;
                     e_movement++;
  									 boss.frame = 0;
                     boss.shields = 200;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL THREE", 160, 60, 120);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;

      case 3:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss3[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
                     e_firerate-=2;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL FOUR", 160, 60, 120);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 4:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss4[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
                     e_movement=2;
										 e_firerate=10;
  									 boss.frame = 0;
                     boss.shields = 200;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL FIVE", 160, 60, 130);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 5:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss1[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=4)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
                     e_movement++;
										 e_firerate-=2;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL SIX", 160, 60, 150);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 6:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss2[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
                     e_movement++;
										 e_firerate-=2;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL SEVEN", 160, 60, 160);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 7:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss3[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
                     e_movement++;
										 e_firerate-=2;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL EIGHT", 160, 60, 180);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 170);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 8:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss.frame>2)
                boss.frame=0;

              draw_sprite(Hscreen, boss4[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 39, 44);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
		  							 boss.x = random()%100+110;
  									 boss.y = -250;
  									 boss.frame = 0;
                     boss.shields = 200;
										 e_firerate-=2;
                     eship[0].frame=(random()%15);
                     eship[0].x = (random()%250+30);
                     eship[0].y = -100;
                     eship[0].shields = 100;
                     end_level();
  									 textout_centre(Vscreen, data[FONT4].dat, "LEVEL NINE!!", 160, 60, 200);
  									 textout_centre(Vscreen, data[FONT3].dat, "Press a key to begin", 160, 130, 100);
                     clear_keybuf();
  									 readkey();
                   }
                }
            }
        } break;
      case 9:
        {
          if (!bdie)
            {
              if (boss.y<10)
                boss.y+=2;
              else if ((boss.y<100) && (!random()%2))
                boss.y+=2;
              else if (boss.y>10)
                boss.y-=2;

              if (boss.x > pship.x)
                boss.x--;
              else
                boss.x++;

              bmissle_delay++;

              if (bmissle_delay>10 && boss.y>-20)
                add_boss_missle();

              if (++boss_delay>7)
                {
                  if (++boss.frame>1)
                    boss.frame=0;
                   boss_delay=0;
                }

              draw_sprite(Hscreen, boss5[boss.frame], boss.x, boss.y);
            }
          else
            {
              stretch_sprite(Hscreen, gexplosion[bdie_frame], boss.x, boss.y, 65, 40);
              if (++bdie_delay>=6)
                {
                  bdie_delay=0;
                  if (++bdie_frame == 5)
                   {
                     end_level();
                     quit=1;
                   }
                }
            }
        } break;
    }
}


//// A D D   B O S S   M I S S L E ///////////////////////////////////

void add_boss_missle(void)
{
  int i, count=0;

  bmissle_delay=0;

  for (i=0; i<max_missle; i++)
    {
      if (bmissles[i].status==0)
        {
          if (count==0)
            {
              bmissles[i].x = boss.x+10;
              bmissles[i].y = boss.y+15;
            }
          else if (count==1)
            {
              bmissles[i].x = boss.x+30;
              bmissles[i].y = boss.y+15;
            }

          bmissles[i].frame = 0;
          bmissles[i].status = 1;
          bmissles[i].timer = 0;
          if (++count==2)
            break;
        }
    }
}

//// M O V E   B O S S   M I S S L E ////////////////////////////////

void move_boss_missle(void)
{
  int i;

  for (i=0; i<max_missle; i++)
    {
      if (bmissles[i].status==1)
        {
          bmissles[i].y+=4;
          draw_sprite(Hscreen, bmissle, bmissles[i].x, bmissles[i].y);
          if (bmissles[i].y>210)
            bmissles[i].status=0;

          if (collide(pship.x, pship.y, bmissles[i].x, bmissles[i].y, 15, 20))
            {
              if (pship.shields>=10)
                pship.shields-=10;
              bmissles[i].status=0;
            }
        }
    }
}


//// C O L L I D E //////////////////////////////////////////////////////

int collide(int x1, int y1, int x2, int y2, int w, int h)
{
  int dx, dy;

  dx = abs(x1 - x2);
  dy = abs(y1 - y2);

  if (dx<(w-(w>>3)) && dy<(h-(h>>3)))
    return(1);
  else
    return(0);
}

//// A D D   E N E M Y   M I S S L E ////////////////////////////////

void add_enemy_missle(int num)
{
  int i;

  emissle_delay[num]=0;

  for (i=0; i<max_missle; i++)
    {
      if (emissles[i].status==0)
        {
          emissles[i].x = eship[num].x+10;
          emissles[i].y = eship[num].y+10;
          emissles[i].frame = 0;
          emissles[i].status = 1;
          emissles[i].timer = 0;
          break;
        }
    }
}

//// M O V E   E N E M Y   M I S S L E ////////////////////////////////

void move_enemy_missle(void)
{
  int i;

  for (i=0; i<max_missle; i++)
    {
      if (emissles[i].status==1)
        {
          emissles[i].y+=5;
          draw_sprite(Hscreen, emissle[emissles[i].frame], emissles[i].x, emissles[i].y);
          if (emissles[i].frame++>0)
            emissles[i].frame=0;
          if (emissles[i].y>210)
            emissles[i].status=0;
          if (collide(pship.x, pship.y, emissles[i].x, emissles[i].y, 28, 20))
            {
              if (pship.shields>=10)
                pship.shields-=10;
              emissles[i].status=0;
            }
        }
    }
}


//// M O V E   P L A Y E R /////////////////////////////////////////////

void move_player(void)
{
  if (pship.shields <=0 && !pdie)
    {
      play_sample(data[BANG].dat, 40, 125, 1000, 0);
      pdie = 1;
      pdie_frame = 0;
      pdie_delay = 0;
      die_delay = 100;
    }

  if ((key[KEY_LEFT]) && (pship.x>1) && (!pdie))
    pship.x-=3;
  if ((key[KEY_RIGHT]) && (pship.x<285) && (!pdie))
    pship.x+=3;
  if ((key[KEY_UP]) && (pship.y>1) && (!pdie))
    pship.y-=3;
  if ((key[KEY_DOWN]) && (pship.y<184) && (!pdie))
    pship.y+=3;
  if ((key[KEY_SPACE]) && (pmissle_delay>7) && (!pdie))
    add_player_missle();

  if (!pdie)
    {
      draw_sprite(Hscreen, player[pship.frame], pship.x, pship.y);
      pmissle_delay++;
      if (pship.frame++>1)
        pship.frame = 0;
    }
  else
    {
      draw_sprite(Hscreen, explosion[pdie_frame], pship.x, pship.y);
      pdie_delay++;
      if (pdie_delay>=4)
        {
          pdie_delay=0;
          pdie_frame++;
          if (pdie_frame == 6)
            {
              pdie = 0;
              pship.shields = 100;
              pship.x = 160;
              pship.y = 170;
              lives--;
              if (lives<0)
                quit = 2;
            }
        }
    }

}

//// M O V E   P L A Y E R   M I S S L E //////////////////////////////

void move_player_missle(void)
{
  int i;

  for (i=0; i<max_missle; i++)
    {
      if (pmissles[i].status==1)
        {
          if (pmissles[i].timer++ < 5)
            pmissles[i].y--;
          else if (pmissles[i].timer < 10)
            pmissles[i].y-=2;
          else
            pmissles[i].y-=5;
          draw_sprite(Hscreen, pmissle[pmissles[i].frame], pmissles[i].x, pmissles[i].y);
          if (pmissles[i].frame++>0)
            pmissles[i].frame=0;
          if (pmissles[i].y<-8)
            pmissles[i].status=0;

          if (enemy_num<=30)
            {
              if (collide(eship[0].x, eship[0].y, pmissles[i].x, pmissles[i].y, 20, 20))
                {
                  eship[0].shields-=20;
                  pmissles[i].status=0;
                }
              if (collide(eship[1].x, eship[1].y, pmissles[i].x, pmissles[i].y, 20, 20))
                {
                  eship[1].shields-=20;
                  pmissles[i].status=0;
                }
            }
          else
            {
              if (collide(boss.x, boss.y, pmissles[i].x, pmissles[i].y, 40, 30))
                {
                  boss.shields-=10;
                  pmissles[i].status=0;
                }
            }
        }
    }
}

//// A D D  P L A Y E R   M I S S L E ////////////////////////////////

void add_player_missle(void)
{
  int i;

  pmissle_delay=0;

  for (i=0; i<max_missle; i++)
    {
      if (pmissles[i].status==0)
        {
          pmissles[i].x = pship.x+14;
          pmissles[i].y = pship.y+2;
          pmissles[i].frame = 0;
          pmissles[i].status = 1;
          pmissles[i].timer = 0;
          break;
        }
    }
}

//// C R E D I T S ////////////////////////////////////////////
void credits(void)
{
  blit(data[BACK].dat, Vscreen, 0, 0, 0, 0, 320, 240);
	textout_centre(Vscreen, data[FONT2].dat, "Credits", 160, 10, 50);
  textout_centre(Vscreen, data[FONT3].dat, "Programming by Derek Barber", 160, 50, 180);
  textout_centre(Vscreen, data[FONT3].dat, "Music by Derek Barber", 160, 70, 180);
  textout_centre(Vscreen, data[FONT3].dat, "Special Thanks to Shawn Hargreaves for his", 160, 110, 210);
  textout_centre(Vscreen, data[FONT3].dat, "game programming library, ALLEGRO, and Ari", 160, 125, 210);
  textout_centre(Vscreen, data[FONT3].dat, "Feldman for his graphics library, SPRITELIB.", 160, 140, 210);
  textout_centre(Vscreen, data[FONT5].dat, "Press a key...", 160, 200, 230);
  fade_in(data[PAL].dat, 10);
  clear_keybuf();
  readkey();
  fade_out(10);
}


//// M O V E   B A C K G R O U N D //////////////////////////////////////

void move_background(void)
{
  int index, i;

  for (index=0; index<=num_stars; index++)
    {
      switch(stars[index].plane)
        {
          case 1:
            {
              stars[index].y+=plane1;
            } break;
          case 2:
            {
              stars[index].y+=plane2;
            } break;
          case 3:
            {
              stars[index].y+=plane3;
            } break;
        }

      if (stars[index].y>=210)
        stars[index].y = 0;

      putpixel(Hscreen,stars[index].x,stars[index].y,stars[index].color);
    }

  for (i=0; i<back_images; i++)
    {
      switch (back[i].image)
        {
          case 0:
            {
              draw_sprite(Hscreen, splanet[back[0].frame], back[i].x, back[i].y);
            } break;
          case 1:
            {
              draw_sprite(Hscreen, lplanet, back[i].x, back[i].y);
            } break;
          case 2:
            {
              draw_sprite(Hscreen, asteroid1, back[i].x, back[i].y);
            } break;
          case 3:
            {
              draw_sprite(Hscreen, asteroid2, back[i].x, back[i].y);
            } break;
          case 4:
            {
              draw_sprite(Hscreen, asteroid3, back[i].x, back[i].y);
            } break;
          case 5:
            {
              draw_sprite(Hscreen, splanet2[0], back[i].x, back[i].y);
            } break;
          case 6:
            {
              draw_sprite(Hscreen, splanet2[1], back[i].x, back[i].y);
            } break;
          case 7:
            {
              draw_sprite(Hscreen, splanet3, back[i].x, back[i].y);
            } break;
          case 8:
            {
              draw_sprite(Hscreen, splanet4, back[i].x, back[i].y);
            } break;
        }
    }

    if (back[0].delay++>10)
      {
        if (back[0].frame++>1)
          back[0].frame=0;
        back[0].delay=0;
      }

    back[0].y+=back[0].speed;
    back[1].y+=back[1].speed;
    back[2].y+=back[2].speed;

    if (back[0].y>210)
      {
        back[0].image=random()%9;
        back[0].x = random()%90;
        back[0].y = random()%100-150;
        back[0].speed = random()%2+1;
      }
    if (back[1].y>210)
      {
        back[1].image=random()%9;
        back[1].x = random()%90+100;
        back[1].y = random()%100-150;
        back[1].speed = random()%2+1;
      }
    if (back[2].y>210)
      {
        back[2].image=random()%9;
        back[2].x = random()%90+210;
        back[2].y = random()%100-150;
        back[2].speed = random()%2+1;
      }
}

//// L O A D   G R A P H I C S /////////////////////////////////////////

void load_graphics(void)
{
  int i;

  asteroid1 = create_bitmap(22, 26);
  asteroid2 = create_bitmap(13, 14);
  asteroid3 = create_bitmap(16, 17);
  splanet2[0] = create_bitmap(12, 19);
  splanet2[1] = create_bitmap(12, 19);
  splanet3 = create_bitmap(14, 13);
  splanet4 = create_bitmap(22, 18);
  pmissle[0] = create_bitmap(5, 8);
  pmissle[1] = create_bitmap(5, 8);
  emissle[0] = create_bitmap(5, 8);
  emissle[1] = create_bitmap(5, 8);
  bmissle = create_bitmap(12, 16);
  lplanet = create_bitmap(49, 46);
  for (i=0; i<6; i++)
    explosion[i] = create_bitmap(22, 22);
  for (i=0; i<3; i++)
    splanet[i] = create_bitmap(18,18);
  for (i=0; i<3; i++)
    player[i] = create_bitmap(33,25);
  for (i=0; i<3; i++)
    enemy[i] = create_bitmap(21,15);
  for (i=3; i<6; i++)
    enemy[i] = create_bitmap(18,13);
  for (i=6; i<9; i++)
    enemy[i] = create_bitmap(33,22);
  for (i=9; i<12; i++)
    enemy[i] = create_bitmap(22,20);
  for (i=12; i<15; i++)
    enemy[i] = create_bitmap(26,21);
  for (i=15; i<18; i++)
    enemy[i] = create_bitmap(16,17);
  for (i=18; i<21; i++)
    enemy[i] = create_bitmap(23,22);
  for (i=21; i<24; i++)
    enemy[i] = create_bitmap(27,27);
  for (i=0; i<3; i++)
    boss1[i] = create_bitmap(39,44);
  for (i=0; i<3; i++)
    boss2[i] = create_bitmap(44,26);
  for (i=0; i<3; i++)
    boss3[i] = create_bitmap(46,34);
  for (i=0; i<3; i++)
    boss4[i] = create_bitmap(33,35);
  for (i=0; i<2; i++)
    boss5[i] = create_bitmap(65,40);
  for (i=0; i<5; i++)
    gexplosion[i] = create_bitmap(20, 19);
  portal = create_bitmap(40, 46);

  blit(data[GRAPHICS].dat, portal, 138, 233, 0, 0, 40, 46);
  blit(data[GRAPHICS].dat, gexplosion[0], 138, 210, 0, 0, 20, 19);
  blit(data[GRAPHICS].dat, gexplosion[1], 164, 210, 0, 0, 20, 19);
  blit(data[GRAPHICS].dat, gexplosion[2], 191, 210, 0, 0, 20, 19);
  blit(data[GRAPHICS].dat, gexplosion[3], 218, 210, 0, 0, 20, 19);
  blit(data[GRAPHICS].dat, gexplosion[4], 246, 210, 0, 0, 20, 19);
  blit(data[GRAPHICS].dat, explosion[0], 0, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, explosion[1], 23, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, explosion[2], 46, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, explosion[3], 71, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, explosion[4], 97, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, explosion[5], 123, 0, 0, 0, 22, 22);
  blit(data[GRAPHICS].dat, asteroid1, 60, 186, 0, 0, 22, 26);
  blit(data[GRAPHICS].dat, asteroid2, 92, 188, 0, 0, 13, 14);
  blit(data[GRAPHICS].dat, asteroid3, 114, 189, 0, 0, 16, 17);
  blit(data[GRAPHICS].dat, splanet2[0], 59, 225, 0, 0, 12, 19);
  blit(data[GRAPHICS].dat, splanet2[1], 85, 225, 0, 0, 12, 19);
  blit(data[GRAPHICS].dat, splanet3, 5, 222, 0, 0, 14, 13);
  blit(data[GRAPHICS].dat, splanet4, 107, 216, 0, 0, 22, 18);
  blit(data[GRAPHICS].dat, pmissle[0], 153, 0, 0, 0, 5, 8);
  blit(data[GRAPHICS].dat, pmissle[1], 161, 0, 0, 0, 5, 8);
  blit(data[GRAPHICS].dat, emissle[0], 170, 0, 0, 0, 5, 8);
  blit(data[GRAPHICS].dat, emissle[1], 178, 0, 0, 0, 5, 8);
  blit(data[GRAPHICS].dat, bmissle, 188, 0, 0, 0, 12, 16);
  blit(data[GRAPHICS].dat, lplanet, 0, 160, 0, 0, 49, 46);
  blit(data[GRAPHICS].dat, splanet[0], 62, 160, 0, 0, 18, 18);
  blit(data[GRAPHICS].dat, splanet[1], 87, 160, 0, 0, 18, 18);
  blit(data[GRAPHICS].dat, splanet[2], 112, 160, 0, 0, 18, 18);
  blit(data[GRAPHICS].dat, player[0], 0, 23, 0, 0, 33, 25);
  blit(data[GRAPHICS].dat, player[1], 39, 23, 0, 0, 33, 25);
  blit(data[GRAPHICS].dat, player[2], 78, 23, 0, 0, 33, 25);
  blit(data[GRAPHICS].dat, enemy[0], 115, 23, 0, 0, 21, 15);
  blit(data[GRAPHICS].dat, enemy[1], 142, 23, 0, 0, 21, 15);
  blit(data[GRAPHICS].dat, enemy[2], 169, 23, 0, 0, 21, 15);
  blit(data[GRAPHICS].dat, enemy[3], 195, 23, 0, 0, 18, 13);
  blit(data[GRAPHICS].dat, enemy[4], 219, 23, 0, 0, 18, 13);
  blit(data[GRAPHICS].dat, enemy[5], 243, 23, 0, 0, 18, 13);
  blit(data[GRAPHICS].dat, enemy[6], 0, 51, 0, 0, 33, 22);
  blit(data[GRAPHICS].dat, enemy[7], 37, 51, 0, 0, 33, 22);
  blit(data[GRAPHICS].dat, enemy[8], 74, 51, 0, 0, 33, 22);
  blit(data[GRAPHICS].dat, enemy[9], 115, 42, 0, 0, 22, 20);
  blit(data[GRAPHICS].dat, enemy[10], 140, 42, 0, 0, 22, 20);
  blit(data[GRAPHICS].dat, enemy[11], 165, 42, 0, 0, 22, 20);
  blit(data[GRAPHICS].dat, enemy[12], 193, 40, 0, 0, 26, 21);
  blit(data[GRAPHICS].dat, enemy[13], 228, 40, 0, 0, 26, 21);
  blit(data[GRAPHICS].dat, enemy[14], 263, 40, 0, 0, 26, 21);
  blit(data[GRAPHICS].dat, enemy[15], 132, 97, 0, 0, 16, 17);
  blit(data[GRAPHICS].dat, enemy[16], 156, 97, 0, 0, 16, 17);
  blit(data[GRAPHICS].dat, enemy[17], 180, 97, 0, 0, 16, 17);
  blit(data[GRAPHICS].dat, enemy[18], 202, 96, 0, 0, 23, 22);
  blit(data[GRAPHICS].dat, enemy[19], 231, 96, 0, 0, 23, 22);
  blit(data[GRAPHICS].dat, enemy[20], 260, 96, 0, 0, 23, 22);
  blit(data[GRAPHICS].dat, enemy[21], 0, 123, 0, 0, 27, 27);
  blit(data[GRAPHICS].dat, enemy[22], 40, 123, 0, 0, 27, 27);
  blit(data[GRAPHICS].dat, enemy[23], 79, 123, 0, 0, 27, 27);
  blit(data[GRAPHICS].dat, boss1[0], 0, 76, 0, 0, 39, 44);
  blit(data[GRAPHICS].dat, boss1[1], 44, 76, 0, 0, 39, 44);
  blit(data[GRAPHICS].dat, boss1[2], 88, 76, 0, 0, 39, 44);
  blit(data[GRAPHICS].dat, boss2[0], 132, 67, 0, 0, 44, 26);
  blit(data[GRAPHICS].dat, boss2[1], 180, 67, 0, 0, 44, 26);
  blit(data[GRAPHICS].dat, boss2[2], 228, 67, 0, 0, 44, 26);
  blit(data[GRAPHICS].dat, boss3[0], 113, 124, 0, 0, 46, 34);
  blit(data[GRAPHICS].dat, boss3[1], 165, 124, 0, 0, 46, 34);
  blit(data[GRAPHICS].dat, boss3[2], 218, 124, 0, 0, 46, 34);
  blit(data[GRAPHICS].dat, boss4[0], 0, 245, 0, 0, 33, 35);
  blit(data[GRAPHICS].dat, boss4[1], 40, 245, 0, 0, 33, 35);
  blit(data[GRAPHICS].dat, boss4[2], 79, 245, 0, 0, 33, 35);
  blit(data[GRAPHICS].dat, boss5[0], 140, 164, 0, 0, 65, 40);
  blit(data[GRAPHICS].dat, boss5[1], 209, 164, 0, 0, 65, 40);

  back[0].image=random()%9;
  back[0].x = random()%90;
  back[0].y = random()%100-150;
  back[0].speed = random()%2+1;
  back[1].image=random()%9;
  back[1].x = random()%90+100;
  back[1].y = random()%100-210;
  back[1].speed = random()%2+1;
  back[2].image=random()%9;
  back[2].x = random()%90+210;
  back[2].y = random()%100-180;
  back[2].speed = random()%2+1;
  back[0].delay = 0;
  back[0].frame=0;

}

//// S T O R Y ///////////////////////////////////////////////

void story(void)
{
  blit(data[BACK].dat, Vscreen, 0, 0, 0, 0, 320, 240);
	textout_centre(Vscreen, data[FONT2].dat, "Story", 160, 10, 50);
  textout_centre(Vscreen, font, "Many years ago, you left Earth,", 160, 70, 180);
  textout_centre(Vscreen, font, "supposedly on a routine mission.", 160, 80, 180);
  textout_centre(Vscreen, font, "However, due to a critical malfunction", 160, 90, 180);
  textout_centre(Vscreen, font, "in your ship's hyperdrive engine, you", 160, 100, 180);
  textout_centre(Vscreen, font, "were sent into another dimension.", 160, 110, 180);
  textout_centre(Vscreen, font, "You found this new dimension to be", 160, 120, 180);
  textout_centre(Vscreen, font, "populated with hostile creatures and", 160, 130, 180);
  textout_centre(Vscreen, font, "you have been forced to fight them or", 160, 140, 180);
  textout_centre(Vscreen, font, "perish.  You only hope is that you will", 160, 150, 180);
  textout_centre(Vscreen, font, "someday return to earth...", 160, 160, 180);
  textout_centre(Vscreen, data[FONT5].dat, "Press a key...", 160, 200, 230);
  fade_in(data[PAL].dat, 10);
  clear_keybuf();
  readkey();
  fade_out(10);

}


void initialize_variables(void)
{
  int i;

  quit = 0;
  enemy_num = 0;

  pdie = 0;
  pship.x = 160;
  pship.y = 160;
  pship.frame = 0;
  pship.shields = 100;

  eship[0].x = random()%100+150;
  eship[0].y = -40;
  eship[0].frame = (random()%15);
  eship[0].shields = 100;
  edie[0] = 0;

  eship[1].x = random()%100+50;
  eship[1].y = -80;
  eship[1].frame = (random()%24);
  eship[1].shields = 100;
  edie[1] = 0;

  boss.x = random()%100+110;
  boss.y = -250;
  boss.frame = 0;
  boss.shields = 200;
  bdie = 0;

  level = 1;
  score = 0;
  lives = 3;
  enemy_done[0] = 1;
  enemy_done[1] = 1;

  e_movement = 2;
  e_firerate = 10;

  for (i=0; i<max_missle; i++)
    {
      pmissles[i].status=0;
      emissles[i].status=0;
      bmissles[i].status=0;
    }
}


//// S H O W   S T A T U S /////////////////////////////////////////////

void show_status(void)
{
  char Score[8], Pshields[4];
  int i;

  sprintf(Score, "%d", score);
  sprintf(Pshields, "%d", pship.shields);

  blit(status, Hscreen, 0, 0, 0, 210, 320, 30);
  textout_centre(Hscreen, font, Score, 49, 227, 200);
  textout_centre(Hscreen, font, Pshields, 155, 227, 200);
  for (i=0; i<lives; i++)
    stretch_sprite(Hscreen, player[0], 197+i*20, 225, 15, 13);
}


//// S H U T D O W N //////////////////////////////////////////////////

void shutdown(void)
{
  int i;

  destroy_bitmap(asteroid1);
  destroy_bitmap(asteroid2);
  destroy_bitmap(asteroid3);
  destroy_bitmap(splanet2[0]);
  destroy_bitmap(splanet2[1]);
  destroy_bitmap(splanet3);
  destroy_bitmap(splanet4);
  destroy_bitmap(pmissle[0]);
  destroy_bitmap(pmissle[1]);
  destroy_bitmap(emissle[0]);
  destroy_bitmap(emissle[1]);
  destroy_bitmap(bmissle);
  destroy_bitmap(lplanet);
  for (i=0; i<6; i++)
    destroy_bitmap(explosion[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(splanet[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(player[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(enemy[i]);
  for (i=3; i<6; i++)
    destroy_bitmap(enemy[i]);
  for (i=6; i<9; i++)
    destroy_bitmap(enemy[i]);
  for (i=9; i<12; i++)
    destroy_bitmap(enemy[i]);
  for (i=12; i<15; i++)
    destroy_bitmap(enemy[i]);
  for (i=15; i<18; i++)
    destroy_bitmap(enemy[i]);
  for (i=18; i<21; i++)
    destroy_bitmap(enemy[i]);
  for (i=21; i<24; i++)
    destroy_bitmap(enemy[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(boss1[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(boss2[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(boss3[i]);
  for (i=0; i<3; i++)
    destroy_bitmap(boss4[i]);
  for (i=0; i<2; i++)
    destroy_bitmap(boss5[i]);

  allegro_exit();
}

//// K E Y   C H E C K E R //////////////////////////////////////////////

int key_checker(void)
{
  if (keypressed())
	 return 1;
      else
	 return 0;
}

//// I N I T   S T A R F I E L D /////////////////////////////////////////

void init_starfield(void)
{
  int index;

  for (index=0; index<num_stars; index++)
    {
      stars[index].x = random()%320;
      stars[index].y = random()%210;

      switch(random()%3)
        {
          case 0:
            {
              stars[index].plane = 1;
              stars[index].color = 8;
            } break;
          case 1:
            {
              stars[index].plane = 2;
              stars[index].color = 7;
            } break;
          case 2:
            {
              stars[index].plane = 3;
              stars[index].color = 15;
            } break;

        }  // end switch

    } // end for index

} // end init_starfield


//// E N D   G A M E /////////////////////////////////////////

void end_game(void)
{
  int quit = 0, delay = 0;
  char Score[20];

  play_sample(data[END].dat, 255, 125, 1000, 0);

  draw_sprite(Vscreen, player[0], pship.x, pship.y);

	textout_centre(Vscreen, data[FONT3].dat, "You are victorious...", 160, 80, 150);
  rest(4000);
  textout_centre(Vscreen, data[FONT3].dat, "You have destroyed all of the enemies!", 160, 100, 150);
  rest(2000);
  textout_centre(Vscreen, data[FONT3].dat, "Now you must find your way home again.", 160, 120, 150);
  rest(4000);

  while (quit==0)
    {
	    clear(Hscreen);
		  move_background();
		  blit(status, Hscreen, 0, 0, 0, 210, 320, 30);
      draw_sprite(Hscreen, player[pship.frame], pship.x, pship.y);

      delay++;

      if (delay >= 80 && delay <= 250)
        textout_centre(Hscreen, data[FONT3].dat, "Suddenly, something incredible happens...", 160, 120, 150);
			if (delay >= 150)
			  stretch_sprite(Hscreen, portal, 158, 76, 4, 4);
			if (delay >= 155)
			  stretch_sprite(Hscreen, portal, 156, 74, 8, 8);
			if (delay >= 160)
			  stretch_sprite(Hscreen, portal, 154, 72, 12, 12);
			if (delay >= 165)
			  stretch_sprite(Hscreen, portal, 152, 70, 16, 16);
			if (delay >= 170)
			  stretch_sprite(Hscreen, portal, 150, 68, 20, 20);
			if (delay >= 175)
			  stretch_sprite(Hscreen, portal, 148, 66, 24, 24);
			if (delay >= 180)
			  stretch_sprite(Hscreen, portal, 146, 64, 28, 28);
			if (delay >= 185)
			  stretch_sprite(Hscreen, portal, 144, 62, 32, 32);
			if (delay >= 190)
			  stretch_sprite(Hscreen, portal, 142, 60, 36, 36);
			if (delay >= 195)
			  stretch_sprite(Hscreen, portal, 140, 58, 40, 40);
			if (delay >= 200)
			  stretch_sprite(Hscreen, portal, 140, 56, 40, 44);
			if (delay >= 205)
				draw_sprite(Hscreen, portal, 140, 55);
      if (delay >= 300 && delay <= 450)
        textout_centre(Hscreen, data[FONT3].dat, "A dimensional portal opens!", 160, 120, 150);
      if (delay >= 340 && delay <= 450)
        textout_centre(Hscreen, data[FONT3].dat, "You have no choice but to enter it...", 160, 140, 150);
      if (delay >= 450)
        pship.y--;

      if (pship.y <= 55)
			  quit = 1;

      if (pship.frame++>1)
        pship.frame = 0;

  	  blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
      rest(10);
    }

    quit = 0;
    delay=0;

  while (quit==0)
    {
	    clear(Hscreen);
		  move_background();
		  blit(status, Hscreen, 0, 0, 0, 210, 320, 30);

      delay++;

      if (delay <= 80)
				draw_sprite(Hscreen, portal, 140, 55);
			if (delay >= 90 && delay < 95)
			  stretch_sprite(Hscreen, portal, 140, 56, 40, 44);
			if (delay >= 95 && delay < 100)
			  stretch_sprite(Hscreen, portal, 140, 58, 40, 40);
			if (delay >= 100 && delay < 105)
			  stretch_sprite(Hscreen, portal, 142, 60, 36, 36);
			if (delay >= 105 && delay < 110)
			  stretch_sprite(Hscreen, portal, 144, 62, 32, 32);
			if (delay >= 110 && delay < 115)
			  stretch_sprite(Hscreen, portal, 146, 64, 28, 28);
			if (delay >= 115 && delay < 120)
			  stretch_sprite(Hscreen, portal, 148, 66, 24, 24);
			if (delay >= 120 && delay < 125)
			  stretch_sprite(Hscreen, portal, 150, 68, 20, 20);
			if (delay >= 125 && delay < 130)
			  stretch_sprite(Hscreen, portal, 152, 70, 16, 16);
			if (delay >= 130 && delay < 135)
			  stretch_sprite(Hscreen, portal, 154, 72, 12, 12);
			if (delay >= 135 && delay < 140)
			  stretch_sprite(Hscreen, portal, 156, 74, 8, 8);
			if (delay >= 140 && delay <= 145)
			  stretch_sprite(Hscreen, portal, 158, 76, 4, 4);
      if (delay >= 200)
        textout_centre(Hscreen, data[FONT3].dat, "Where has this portal taken you?", 160, 100, 170);
      if (delay >= 240)
        textout_centre(Hscreen, data[FONT3].dat, "Who knows, and who cares!", 160, 120, 170);
      if (delay >= 280)
        textout_centre(Hscreen, data[FONT3].dat, "You finished the game and now it's over.", 160, 140, 170);
      if (delay >= 300)
        textout_centre(Hscreen, data[FONT3].dat, "Thanks for playing, I hope you enjoyed it!", 160, 160, 180);
      if (delay >= 320)
        {
      	  sprintf(Score, "%s%d", "Your final score was : ", score);
          textout_centre(Hscreen, data[FONT3].dat, Score, 160, 180, 170);
        }
      if (delay >= 400)
			  quit = 1;

  	  blit(Hscreen, Vscreen, 0, 0, 0, 0, 320, 240);
      rest(10);
    }

  clear_keybuf();
  readkey();
  fade_out(1);

}
