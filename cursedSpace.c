#include "cursedSpace.h"
#include "linked_list.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int num_photon_torpedos;
int num_ion_cannons;
int num_black_holes;
int score = 0;
int kills = 0;
int current_level = 0;
int direction = RIGHT;
time_t level_timer = 0;
time_t respawn_interval = RESPAWN_INTERVAL;
int game_status = GAME_ON_MAIN_MENU;
struct linked_list *gameObjs;

/*****************************
** Where the universe begins
******************************/

int main(void)
{

   initialize();

   while(game_status != GAME_FINISHED) 
   {
      check_level_status();
      handle_user_input();
      process_world_events();
      render();
   }

   dispose();

   return 0;
}


/*****************************
** Life cycle of the game
*****************************/

void initialize() 
{
   initscr();
   keypad(stdscr,TRUE);
   noecho();
   curs_set(0);
   start_color();

   init_pair(BG_WHITE_TXT_BLACK,COLOR_BLACK,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_BLUE,COLOR_BLUE,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_GREEN,COLOR_GREEN,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_YELLOW,COLOR_YELLOW,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_RED,COLOR_RED,COLOR_WHITE);
   init_pair(BG_BLACK_TXT_BLACK,COLOR_BLACK,COLOR_BLACK);
   init_pair(BG_BLUE_TXT_WHITE,COLOR_WHITE,COLOR_BLUE);
   init_pair(BG_RED_TXT_WHITE,COLOR_WHITE,COLOR_RED);

   bkgd(use_default_colors());

   init_space_obj_list();

   init_level(LEVEL_1);
}


void render()
{
   clear();

   switch(game_status)
   {
      case GAME_ON_MAIN_MENU:
            print_main_menu();
            getchar();
 	    clear();
            print_howto_menu();
            getchar();
            game_status = GAME_RUNNING;
            break;
      case GAME_RUNNING:
	    print_world();
            print_status_bar();
            break;
      case GAME_PAUSED:
            break;
   }

   refresh();
}

void resize()
{
}

void pause()
{
}

void resume()
{
}

void dispose()
{
   endwin();
}

void handle_user_input() 
{
   char ch;
   struct timeval begin,end;
   double elapsed_time = 0,wait_time=0;
   int x1,y1;
   spaceObj *obj;

   gettimeofday(&begin, NULL);
   timeout(50);
   ch = getch();
   gettimeofday(&end, NULL);

   elapsed_time = (end.tv_sec - begin.tv_sec) * 1000.0;      // sec to ms
   elapsed_time += (end.tv_usec - begin.tv_usec) / 1000.0;   // us to ms
   wait_time = 50 - elapsed_time; 

   napms(wait_time);

   switch(ch) 
   {
      case PRESS_DOWN:
           obj = get_space_obj(0);
           move_obj(DOWN,obj);
           direction = DOWN;
           break;
      case PRESS_UP:
           obj = get_space_obj(0);
           move_obj(UP,obj);
           direction = UP;
           break;
      case PRESS_RIGHT:
           obj = get_space_obj(0);
           move_obj(RIGHT,obj);
           direction = RIGHT; 
           break;
      case PRESS_LEFT:
           obj = get_space_obj(0);
           move_obj(LEFT,obj);
           direction = LEFT;
           break;
      case PRESS_FIRE_PHOTON_TORPEDOS:
           if(num_photon_torpedos > 0) {
                obj = get_space_obj(0);
                x1=obj->x1;
                y1=obj->y1;
                obj = init_photon_torpedo(x1,y1);
                add_space_obj(obj);
                num_photon_torpedos--;
           }
           break;
      case PRESS_ION_CANNON:
           if(num_ion_cannons > 0) {
               obj = get_space_obj(0);
               x1=obj->x1; 
               y1=obj->y1;
               obj = init_ion_cannon(x1,y1);
               add_space_obj(obj);
               num_ion_cannons--; 
           } 
           break;
      case PRESS_WARP:
           teleport();
           break;
      case PRESS_MASSIVE_BLACK_HOLE:
           if(num_black_holes > 0) {
               summon_black_hole();
               num_black_holes--;
           }
           break;
   }
}


void check_level_status()
{
    time_t now = time(NULL);	
    int elapsed_time = now - level_timer;

    if(elapsed_time < LEVEL_TIMER) 
          return;

    switch(current_level)
    {
       case LEVEL_1: 
           init_level(LEVEL_2);
           break;
       case LEVEL_2: 
           init_level(LEVEL_3);
           break;
       case LEVEL_3: 
           init_level(LEVEL_4);
           break;
       case LEVEL_4: 
           init_level(FINAL_BOSS);
           break;
       case FINAL_BOSS:
           break;
           // do something
    }
}

void process_world_events()
{
   spaceObj *obj;
   int i,total_num_objs,total_num_enemies;

   total_num_objs = get_number_space_objs();

   // Move objects
   for(i=0;i<total_num_objs;i++)
   {
	obj = get_space_obj(i);

	switch(obj->type)
	{
            case HERO_SPACESHIP:
                // multiplayer? :) 
		break;
            case ALIEN_SPACESHIP:
		move_obj(LEFT,obj); // blind alien spaceship
                break;
            case SMALL_ASTEROID:
                move_obj(LEFT,obj);
                break;
            case MEDIUM_ASTEROID:
                move_obj(LEFT,obj);
                break;
            case BIG_ASTEROID:
                move_obj(LEFT,obj);
                break;
            case HUGE_ASTEROID:
                move_obj(LEFT,obj);
                break;
            case PHOTON_TORPEDO:
                move_obj(RIGHT,obj);
                break;
            case ION_CANNON:
                move_obj(RIGHT,obj);
                break;
            default:
		// do something
                break;
	}
   }

  // Respawn enemies
  time_t now = time(NULL);
  int elapsed_time = now - respawn_interval;
  total_num_enemies = get_number_enemies();

  if(total_num_enemies < MAX_NUM_ENEMIES && elapsed_time > RESPAWN_INTERVAL)
  {
     respawn_enemy();
     respawn_interval = time(NULL);
  }

  // Check for collisions
  analyse_collisions();

  // Clean all dead objects with some sort of space garbage collector
  run_space_garbage_collector();
}


/*****************************
** Vectorial functions
*****************************/

void teleport() 
{
   int max_x,max_y;
   int x0,x1,y0,y1,width,height;
   spaceObj *obj = get_space_obj(0);

   getmaxyx(stdscr,max_y,max_x);

   x0 = obj->x0;
   x1 = obj->x1;
   y0 = obj->y0;
   y1 = obj->y1; 
   width = x1 - x0;
   height = y1 - y0;

   switch(direction) {
      case UP:
           if(y0-TELEPORT_RANGE_Y <= 0) {
                y0 = 1;
                y1 = height+1;
           }
           else {
                y0 -= TELEPORT_RANGE_Y;
                y1 -= TELEPORT_RANGE_Y;
           }
           break;
      case DOWN:
           if(y1+TELEPORT_RANGE_Y >= max_y) {
                y0 = max_y-height-1;
                y1 = max_y-1;
           }
           else {
                y0 += TELEPORT_RANGE_Y;
                y1 += TELEPORT_RANGE_Y;
           }
           break;
      case LEFT:
           if(x0-TELEPORT_RANGE_X <= 0) {
                x0 = 1;
                x1 = width+1; 
           }
           else {
                x0 -= TELEPORT_RANGE_X;
                x1 -= TELEPORT_RANGE_X;
           }
           break;
      case RIGHT:
           if(x1+TELEPORT_RANGE_X >= max_x) {
                x0 = max_x-width-1;
                x1 = max_x-1;
           }
           else {
                x0 += TELEPORT_RANGE_X;
                x1 += TELEPORT_RANGE_X;
           }
           break;
   }

   obj->x0 = x0;
   obj->x1 = x1;
   obj->y0 = y0;
   obj->y1 = y1;
}

void move_obj(int direction,spaceObj *obj)
{
   int obj_invalid = 0;
   int obj_type = obj->type;
   int x0 = obj->x0;
   int x1 = obj->x1;
   int y0 = obj->y0;
   int y1 = obj->y1;
               	
   switch(direction)
   {
      case DOWN:
               y0++; y1++;
               if(!position_valid(x0,y0,x1,y1)) { obj_invalid = 1; break; }
               obj->y0++;
               obj->y1++;
               break;
      case UP:
               y0--; y1--;
               if(!position_valid(x0,y0,x1,y1)) { obj_invalid = 1; break; }
               obj->y0--;
               obj->y1--;
               break;
      case LEFT:
               x0--; x1--;
               if(!position_valid(x0,y0,x1,y1)) { obj_invalid = 1; break; }
               obj->x0--;
               obj->x1--;
               break;
      case RIGHT:
               x0++; x1++;
               if(!position_valid(x0,y0,x1,y1)) { obj_invalid = 1; break; }
               obj->x0++;
               obj->x1++;
               break;
      default:
               break;
   }

   if(obj_invalid && obj_type != HERO_SPACESHIP)
        obj->status = STATUS_DESTROYED;
}


int position_valid(int x0,int y0,int x1,int y1)
{
   int max_x,max_y;
   
   getmaxyx(stdscr,max_y,max_x);       /* get screen width */

   return x0>=0 && y0>=0 && x1<=max_x && y1<=max_y;
}


/*****************************
** All printable content
*****************************/

void print_text(int x,int y, char *text,int color,int style,int align,int display_speed)
{
	if(align == ALIGN_CENTER)
	   x = (x-strlen(text))/2;

	move(y,x);

	while(*text)			   /* loop through the whole string */
	{
	        attrset(COLOR_PAIR(color));
                if(style != NONE) 
		    attron(style);

		addch(*text);		   /* put one char to curscr */
		text++;		   /* increment the pointer */

		if(display_speed != 0) 
                {
	            refresh();		   /* update the screen */
		    napms(display_speed);  /* delay a bit to see the display */
		}
	}

        attrset(COLOR_PAIR(color));
        if(style != NONE) 
              attroff(style);
}


void print_main_menu() 
{
   int logo_display_speed = 1;
   int history_display_speed = 50;
   int start_display_speed = 1;
   int x,y;

   getmaxyx(stdscr,y,x);

   print_text(x,0,"                                                      ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,1," _______           _______  _______  _______  ______  ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,2,"(  ____ \\|\\     /|(  ____ )(  ____ \\(  ____ \\(  __  \\ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,3,"| (    \\/| )   ( || (    )|| (    \\/| (    \\/| (  \\  )",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,4,"| |      | |   | || (____)|| (_____ | (__    | |   ) |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,5,"| |      | |   | ||     __)(_____  )|  __)   | |   | |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,6,"| |      | |   | || (\\ (         ) || (      | |   ) |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,7,"| (____/\\| (___) || ) \\ \\__/\\____) || (____/\\| (__/  )",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,8,"(_______/(_______)|/   \\__/\\_______)(_______/(______/ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,10," _______  _______  _______  _______  _______  ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,11,"(  ____ \\(  ____ )(  ___  )(  ____ \\(  ____ \\",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,12,"| (    \\/| (    )|| (   ) || (    \\/| (    \\/ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,13,"| (_____ | (____)|| (___) || |      | (__     ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,14,"(_____  )|  _____)|  ___  || |      |  __)    ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,15,"      ) || (      | (   ) || |      | (       ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,16,"/\\____) || )      | )   ( || (____/\\| (____/\\ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,17,"\\_______)|/       |/     \\|(_______/(_______/ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);


   print_text(x,22,"On a remote terminal very far far far away",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);
   print_text(x,23,"there is a universe on the edge of its very existence,",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);
   print_text(x,24,"infested with ASCII asteroids and aliens.",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);
   print_text(x,25,"But there is still hope!",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);
   print_text(x,26,"You can make the difference aboard the killer ship!",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);
   print_text(x,27,"Your terminal is counting on you! Good hunt!",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,history_display_speed);

   print_text(x,35,"Press any key to start",BG_WHITE_TXT_BLACK,A_BOLD,ALIGN_CENTER,start_display_speed);
}

void print_howto_menu()
{
   int logo_display_speed = 1;
   int history_display_speed = 50;
   int start_display_speed = 1;
   int x,y;

   getmaxyx(stdscr,y,x);
   y /= 4;

   print_text(x,y," _   _                 _____      ______ _             ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+1,"| | | |               |_   _|     | ___ \\ |            ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+2,"| |_| | _____      __   | | ___   | |_/ / | __ _ _   _ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+3,"|  _  |/ _ \\ \\ /\\ / /   | |/ _ \\  |  __/| |/ _` | | | |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+4,"| | | | (_) \\ V  V /    | | (_) | | |   | | (_| | |_| |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+5,"\\_| |_/\\___/ \\_/\\_/     \\_/\\___/  \\_|   |_|\\__,_|\\__, |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+6,"                                                  __/ |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+7,"                                                 |___/ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);

   print_text(x,y+10,"w - move ship up",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+11,"s - move ship down",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+12,"a - move ship left",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+13,"d - move ship right",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);

   print_text(x,y+15,"t - teleport",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+16,"f - fire photon torpedos",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+17,"i - fire ion cannon",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);
   print_text(x,y+18,"b - summon black hole",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,start_display_speed);

   print_text(x,y+22,"Press any key to start",BG_WHITE_TXT_BLACK,A_BOLD,ALIGN_CENTER,start_display_speed);
}


void print_youdied_menu()
{
   int logo_display_speed = 1;
   int history_display_speed = 50;
   int start_display_speed = 1;
   int x,y;

   getmaxyx(stdscr,y,x);
   y /= 4;

   print_text(x,y,"          _______             ______  _________ _______  ______   _ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+1,"|\\     /|(  ___  )|\\     /|  (  __  \\ \\__   __/(  ____ \\(  __  \\ ( )",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+2,"( \\   / )| (   ) || )   ( |  | (  \\  )   ) (   | (    \\/| (  \\  )| |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+3," \\ (_) / | |   | || |   | |  | |   ) |   | |   | (__    | |   ) || |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+4,"  \\   /  | |   | || |   | |  | |   | |   | |   |  __)   | |   | || |",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+5,"   ) (   | |   | || |   | |  | |   ) |   | |   | (      | |   ) |(_)",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+6,"   | |   | (___) || (___) |  | (__/  )___) (___| (____/\\| (__/  ) _ ",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
   print_text(x,y+7,"   \\_/   (_______)(_______)  (______/ \\_______/(_______/(______/ (_)",BG_WHITE_TXT_BLACK,NONE,ALIGN_CENTER,logo_display_speed);
}


void init_level(int level)
{
   int i, num_objs;
   spaceObj *obj;

   // reset weapons' ammo
   num_photon_torpedos = MAX_PHOTON_TORPEDOS;
   num_ion_cannons = MAX_ION_CANNON;
   num_black_holes = MAX_BLACK_HOLE;

   // update level number
   current_level = level;

   // special actions for this particular level
   switch(current_level)
   {
      case LEVEL_1:
   	    // add hero spaceship to the world 
   	    obj = init_hero_spaceship();
            add_space_obj(obj);
            break;
      case LEVEL_2:
      case LEVEL_3:
      case LEVEL_4:
      case FINAL_BOSS:
         break;
   }

   level_timer = time(NULL);
}

spaceObj* init_hero_spaceship() 
{
   spaceObj *obj = (spaceObj *) malloc(sizeof(spaceObj));
   int i,len,center_x,center_y;
   int spaceship_width = 38;
   int spaceship_height = 8;

   getmaxyx(stdscr,center_y,center_x);       /* get screen width */
   center_x /= 2;
   center_y /= 2;

   /* hardcode values because they are totally dependent on the ship's image */
   obj->x0 = center_x - spaceship_width/2;
   obj->x1 = center_x + spaceship_width/2;
   obj->y0 = center_y - spaceship_height/2;
   obj->y1 = center_y + spaceship_height/2;

   obj->type = HERO_SPACESHIP;
   obj->status = STATUS_ALIVE;
   obj->color = BG_WHITE_TXT_BLACK;
   obj->shield = SHIELD_HERO_SPACESHIP;

   char **image = malloc(spaceship_height*sizeof(char*));
   for(i = 0; i < spaceship_height; i++)
        image[i] = malloc( spaceship_width*sizeof(char));

   strcpy(image[0],"                                     ");
   strcpy(image[1],"_____________         _______________");
   strcpy(image[2],"\\____________)  _____/ == killer == /");
   strcpy(image[3],"      | |      /_:::_______________/ ");
   strcpy(image[4],"      | |       /  /    \\_______/    ");
   strcpy(image[5],"   ___|_|______/__/                  ");
   strcpy(image[6],"   ==  ... ...   ||||                ");
   strcpy(image[7],"   `--___________||||                ");

   obj->image = image;

   return obj;
}

spaceObj* init_photon_torpedo(int rel_x1,int rel_y1)
{
   spaceObj *obj = (spaceObj *) malloc(sizeof(spaceObj));
   int i;
   int photon_torpedo_width = 4;
   int photon_torpedo_height = 2;

   /* hardcode values because they are totally dependent on the image */
   obj->x0 = rel_x1;
   obj->x1 = rel_x1 + photon_torpedo_width;
   obj->y0 = rel_y1 - photon_torpedo_height;
   obj->y1 = rel_y1;

   obj->type = PHOTON_TORPEDO;
   obj->status = STATUS_ALIVE;
   obj->color = BG_RED_TXT_WHITE;

   char **image = malloc(photon_torpedo_height*sizeof(char*));
   for(i = 0; i < photon_torpedo_height; i++)
        image[i] = malloc( photon_torpedo_width*sizeof(char));

   strcpy(image[0],"****");
   strcpy(image[1],"****");

   obj->image = image;

   return obj;
}

spaceObj* init_ion_cannon(int rel_x1,int rel_y1)
{
   spaceObj *obj = (spaceObj *) malloc(sizeof(spaceObj));
   int i;
   int ion_cannon_width = 20;
   int ion_cannon_height = 8;

   /* hardcode values because they are totally dependent on the image */
   obj->x0 = rel_x1;
   obj->x1 = rel_x1 + ion_cannon_width;
   obj->y0 = rel_y1 - ion_cannon_height;
   obj->y1 = rel_y1;

   obj->type = ION_CANNON;
   obj->status = STATUS_ALIVE;
   obj->color = BG_BLUE_TXT_WHITE;

   char **image = malloc(ion_cannon_height*sizeof(char*));
   for(i = 0; i < ion_cannon_height; i++)
        image[i] = malloc( ion_cannon_width*sizeof(char));

   strcpy(image[0],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[1],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[2],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[3],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[4],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[5],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[6],"~-~-~-~-~-~-~-~-~-~");
   strcpy(image[7],"~-~-~-~-~-~-~-~-~-~");

   obj->image = image;

   return obj;
}

spaceObj* init_asteroid(int size)
{
   spaceObj *obj = (spaceObj *) malloc(sizeof(spaceObj));
   int i,random_y,max_x,max_y;
   int asteroid_width = 0;
   int asteroid_height = 0;

   getmaxyx(stdscr,max_y,max_x);       /* get screen width */
   max_x--;

   switch(size)
   {
	case SMALL:
	     asteroid_height = 4; 
	     asteroid_width = 6;
             obj->shield = SHIELD_SMALL_ASTEROID;
   	     obj->type = SMALL_ASTEROID;
             break;
        case MEDIUM:
             asteroid_height = 6;
             asteroid_width = 8;
             obj->shield = SHIELD_MEDIUM_ASTEROID;
             obj->type = MEDIUM_ASTEROID;
             break;
        case BIG:
             asteroid_height = 10;
             asteroid_width = 18;
             obj->shield = SHIELD_BIG_ASTEROID;
             obj->type = BIG_ASTEROID;
             break;
	case HUGE:
             asteroid_height = 16;
             asteroid_width = 30;
             obj->shield = SHIELD_HUGE_ASTEROID;
             obj->type = HUGE_ASTEROID;
             break;
   }

   /* put asteroid at random y position */
   random_y = random_number(asteroid_height,max_y-asteroid_height); 

   /* hardcode values because they are totally dependent on the asteroid's image */
   obj->x0 = max_x - asteroid_width;
   obj->x1 = max_x;
   obj->y0 = max_y-random_y - asteroid_height/2;
   obj->y1 = max_y-random_y + asteroid_height/2;

   obj->color = BG_WHITE_TXT_BLACK;

   char **image = malloc(asteroid_height*sizeof(char*));
   for(i = 0; i < asteroid_height; i++)
        image[i] = malloc(asteroid_width*sizeof(char));

   switch(size)
   {
        case SMALL:
            strcpy(image[0]," ____ ");
            strcpy(image[1],"/    \\");
	    strcpy(image[2],"|    |");
	    strcpy(image[3],"\\____/");
            break;
        case MEDIUM:
            strcpy(image[0]," .-;;-. ");
            strcpy(image[1],"/      \\");
            strcpy(image[2],"|      |");
            strcpy(image[3],"\\      /");
            strcpy(image[4]," '-..-' ");
	    strcpy(image[5],"        ");
	    break;
        case BIG:
            strcpy(image[0],"      ______      ");
            strcpy(image[1],"    .'      '.    ");
            strcpy(image[2],"  .'          '.  ");
            strcpy(image[3]," /              \\ ");
            strcpy(image[4],";                ;");
            strcpy(image[5],"|                |");
            strcpy(image[6],";                ;");
            strcpy(image[7]," \\              / ");
            strcpy(image[8],"  '.          .'  ");
            strcpy(image[9],"    '-.____.-'    ");
            break;
	case HUGE:
	    strcpy(image[0],"         ___----___         ");
            strcpy(image[1],"      .--          --.      ");
	    strcpy(image[2],"    ./                \\.    ");
	    strcpy(image[3],"   /                    \\   ");
	    strcpy(image[4],"  /                      \\  ");
	    strcpy(image[5]," |                        | ");
	    strcpy(image[6],"|                          |");
	    strcpy(image[7],"|                          |");
	    strcpy(image[8],"|                          |");
	    strcpy(image[9],"|                          |");
	    strcpy(image[10]," |                        | ");
	    strcpy(image[11],"  \\                      /  ");
	    strcpy(image[12],"   \\                    /   ");
	    strcpy(image[13],"    `\\                /'    ");
	    strcpy(image[14],"      '--___    ___--'      ");
	    strcpy(image[15],"            ----            ");
            break;
   }

   obj->image = image;

   return obj;
}

spaceObj* init_alien_spaceship()
{
   spaceObj *obj = (spaceObj *) malloc(sizeof(spaceObj));
   int i,random_y,max_x,max_y;
   int spaceship_width = 16;
   int spaceship_height = 6;

   getmaxyx(stdscr,max_y,max_x);       /* get screen width */
   max_x--;

   /* put asteroid at random y position */
   random_y = random_number(spaceship_height,max_y-spaceship_height);

   /* hardcode values because they are totally dependent on the asteroid's image */
   obj->x0 = max_x - spaceship_width;
   obj->x1 = max_x;
   obj->y0 = max_y-random_y - spaceship_height/2;
   obj->y1 = max_y-random_y + spaceship_height/2;

   obj->type = ALIEN_SPACESHIP;
   obj->status = STATUS_ALIVE;
   obj->color = BG_WHITE_TXT_BLACK;
   obj->shield = SHIELD_ALIEN_SPACESHIP;

   char **image = malloc(spaceship_height*sizeof(char*));
   for(i = 0; i < spaceship_height; i++)
        image[i] = malloc( spaceship_width*sizeof(char));

   strcpy(image[0],"    \\    /     ");
   strcpy(image[1],"   __\\__/__    ");
   strcpy(image[2],"  /         \\  ");
   strcpy(image[3]," /  UFO FTW  \\ ");
   strcpy(image[4],"/-------------\\");
   strcpy(image[5],"===============");

   obj->image = image;

   return obj;
}


void print_world()
{
   int i,total_num_objs;
   total_num_objs = get_number_space_objs();

   for(i=0;i<total_num_objs;i++)
   {
      spaceObj *obj = get_space_obj(i);

      if(obj->status != STATUS_DESTROYED) 
      {
      	print_obj(obj);
      }
   }
}

void print_status_bar()
{
   int elapsed_time,time_left;
   int len=0,margin=5;
   char str[20];

   spaceObj *obj = get_space_obj(0);

   time_t now = time(NULL);
   elapsed_time = now - level_timer;
   time_left = LEVEL_TIMER - elapsed_time;

   sprintf(str, "Shield: %d",obj->shield);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Photon Torpedo: %d",num_photon_torpedos);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Ion Cannon: %d",num_ion_cannons);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Black Hole: %d",num_black_holes);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Time: %d",time_left);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Enemies Killed: %d",kills);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   len += strlen(str) + margin;

   sprintf(str, "Score: %d",score);
   print_text(len,0,str,BG_WHITE_TXT_BLACK,NONE,NONE,0);
}

void respawn_enemy()
{
   spaceObj *obj;
   int r_num,obj_type;

   switch(current_level)
   {
      case LEVEL_1:
            r_num = random_number(1,2);
            if(r_num == 1) obj_type = SMALL_ASTEROID;
            else obj_type = MEDIUM_ASTEROID;
            break;
      case LEVEL_2:
            r_num = random_number(1,3);
            if(r_num == 1) obj_type = ALIEN_SPACESHIP;
            else if(r_num == 2) obj_type = MEDIUM_ASTEROID;
            else obj_type = BIG_ASTEROID;
            break;
      case LEVEL_3:
            r_num = random_number(1,4);
            if(r_num == 1) obj_type = ALIEN_SPACESHIP;
            else if(r_num == 2) obj_type = MEDIUM_ASTEROID;
            else if(r_num == 3) obj_type = BIG_ASTEROID;
            else obj_type = HUGE_ASTEROID;
            break;
      case LEVEL_4:
	    obj_type = ALIEN_SPACESHIP;
            break;
      case FINAL_BOSS:
            break;
   }

   if(obj_type == SMALL_ASTEROID) obj = init_asteroid(SMALL);
   else if(obj_type == MEDIUM_ASTEROID) obj = init_asteroid(MEDIUM);
   else if(obj_type == BIG_ASTEROID) obj = init_asteroid(BIG);
   else if(obj_type == HUGE_ASTEROID) obj = init_asteroid(HUGE);
   else if(obj_type == ALIEN_SPACESHIP) obj = init_alien_spaceship();
   else obj = init_asteroid(SMALL);

   // add object to the world
   add_space_obj(obj);
}

void print_obj(spaceObj *obj)
{
   char **image = obj->image;
   int x,y,max_lines = obj->y1-obj->y0;
   int type = obj->type;
   int color = obj->color;

   for(y=0;y<max_lines;y++,image++)
   {
           print_text(obj->x0,obj->y0+y,*image,color,NONE,NONE,0);
   }
}


void summon_black_hole() 
{
   int max_x,max_y;
   int x_center,y_center;
   int x,y;

   int i,total_num_objs;
   spaceObj *obj;

   getmaxyx(stdscr,max_y,max_x);

   x_center = max_x/2;
   y_center = max_y/2;

   for(x=0;x<x_center;x++) {
      for(y=0;y<y_center;y++) 
      {
   	  print_text(x_center+x,y_center+y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,0);
	  print_text(x_center+x,y_center-y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,0);
          print_text(x_center-x,y_center+y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,0);
          print_text(x_center-x,y_center-y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,1);
      }
    }

    total_num_objs = get_number_space_objs();
    for(i=1;i<total_num_objs;i++)
    {
        obj = get_space_obj(i);
        if(is_enemy(obj->type) == 0) {
             kills++;
	     update_score(obj->type);
	}

        obj->status = STATUS_DESTROYED;
    }
}


/***************************
***  Random
***************************/

int random_number(int min_num, int max_num)
{
   int result=0,low_num=0,hi_num=0;
   if(min_num<max_num)
   {
       low_num=min_num;
       hi_num=max_num+1; // this is done to include max_num in output.
   }else{
       low_num=max_num+1;// this is done to include max_num in output.
        hi_num=min_num;
   }
   srand(time(NULL));
   result = (rand()%(hi_num-low_num))+low_num;
   return result;
}


/***************************
***  List utils                
***************************/

void init_space_obj_list() {
    gameObjs = init_linked_list();
}

spaceObj* get_space_obj(int position) {
    return get_node(gameObjs,position)->data;
}

void add_space_obj(spaceObj *obj) {

    struct node *node = (struct node*) malloc(sizeof(struct node));
    node->data = obj;
    node->next = NULL;
    node->previous = NULL;

    add_node_end(node,gameObjs);
}

void remove_space_obj(int position) {

    remove_node(gameObjs,position);
}

int get_number_space_objs() {
    return get_linked_list_size(gameObjs);
}

int get_number_enemies() {
    int i,total_num_objs,total_num_enemies;
    spaceObj *obj;

    total_num_enemies = 0;
    total_num_objs = get_number_space_objs();
    for(i=0;i<total_num_objs;i++) 
    {
        obj = get_space_obj(i);
	if(is_enemy(obj->type) == 0)
             total_num_enemies++;
    }
 
     return total_num_enemies;
}

/* Using O(n^2) algorithm, although better aproaches like using sorted trees/arrays with distance as index might be interesting.*/
void analyse_collisions() {
    int i,j,total_num_objs,res;
    int x0,x1,y0,y1;
    int _x0,_x1,_y0,_y1;
    spaceObj *obj,*tmp;

    total_num_objs = get_number_space_objs();

    for(i=0;i<total_num_objs;i++)
    {
        obj = get_space_obj(i);
        x0 = obj->x0;
        x1 = obj->x1;
        y0 = obj->y0;
        y1 = obj->y1;

        for(j=0;j<total_num_objs;j++)
        {
                if(i == j) continue;

        	tmp = get_space_obj(j);
                _x0 = tmp->x0;
                _x1 = tmp->x1;
		_y0 = tmp->y0;
		_y1 = tmp->y1;

		if(!(_y1<y0 || _x0>x1 || _x1<x0 || _y0>y1)) { // collision?

                       res = is_enemy(obj->type);

                       if(res == 0 && tmp->type == PHOTON_TORPEDO) 
      		       {
				take_damage(obj,DAMAGE_PHOTON_TORPEDO);
				tmp->status = STATUS_DESTROYED;
		       }
		       else if(res == 0 && tmp->type == ION_CANNON) 
		       {	
				take_damage(obj,DAMAGE_ION_CANNON);
		       }
                       else if(obj->type == HERO_SPACESHIP && tmp->type == SMALL_ASTEROID) 
		       {	
				take_damage(obj,DAMAGE_SMALL_ASTEROID);
                                take_damage(tmp,DAMAGE_COLLISION_HERO_SPACESHIP);
		       }
                       else if(obj->type == HERO_SPACESHIP && tmp->type == MEDIUM_ASTEROID) 
		       {
				take_damage(obj,DAMAGE_MEDIUM_ASTEROID);
                                take_damage(tmp,DAMAGE_COLLISION_HERO_SPACESHIP);
		       }
                       else if(obj->type == HERO_SPACESHIP && tmp->type == BIG_ASTEROID) 
		       {
				take_damage(obj,DAMAGE_BIG_ASTEROID);
                                take_damage(tmp,DAMAGE_COLLISION_HERO_SPACESHIP);
	               }
                       else if(obj->type == HERO_SPACESHIP && tmp->type == HUGE_ASTEROID)
                       {
                                take_damage(obj,DAMAGE_HUGE_ASTEROID);
                                take_damage(tmp,DAMAGE_COLLISION_HERO_SPACESHIP);
                       }
                       else if(obj->type == HERO_SPACESHIP && tmp->type == ALIEN_SPACESHIP)
                       {
                                take_damage(obj,DAMAGE_ALIEN_SPACESHIP);
                                take_damage(tmp,DAMAGE_COLLISION_HERO_SPACESHIP);
                       }
	    }
        }
    }
}

void run_space_garbage_collector() {
    int i,total_num_objs;
    spaceObj *obj;

    total_num_objs = get_number_space_objs();
    for(i=0;i<total_num_objs;i++)
    {
        obj = get_space_obj(i);

        if(obj->status == STATUS_DESTROYED) {
             remove_space_obj(i);
             total_num_objs--;
             i--;
        }
    }
}

int is_enemy(int obj_type) {
    int res = -1;

    switch(obj_type)
    {
         case ALIEN_SPACESHIP: res=0; break;
         case SMALL_ASTEROID: res=0; break;
	 case MEDIUM_ASTEROID: res=0; break;
	 case BIG_ASTEROID: res=0; break;
         case HUGE_ASTEROID: res=0; break;
    }
   
    return res;
}

void take_damage(spaceObj *obj,int damage)
{
    int max_shield;
    double shield_percentage;

    obj->shield -= damage;

    if(obj->shield < 0) {
          obj->status = STATUS_DESTROYED;
          if(is_enemy(obj->type) == 0) {
		kills++;
		update_score(obj->type);
	  }

          if(obj->type == HERO_SPACESHIP) {
	      print_youdied_menu();
              refresh();
              exit(1);
          }
	  return;
    }

    max_shield = get_max_shield(obj->type);
    shield_percentage = ((double)obj->shield/max_shield)*100;
    
    if(shield_percentage > 60)  obj->color = BG_WHITE_TXT_BLACK;
    else if(shield_percentage > 40) obj->color = BG_WHITE_TXT_YELLOW;
    else obj->color = BG_WHITE_TXT_RED;
}

void update_score(int obj_type)
{
    switch(obj_type)
    {
        case ALIEN_SPACESHIP: score += SCORE_ALIEN_SPACESHIP; break;
        case SMALL_ASTEROID: score += SCORE_SMALL_ASTEROID; break;
        case MEDIUM_ASTEROID: score += SCORE_MEDIUM_ASTEROID; break;
        case BIG_ASTEROID: score += SCORE_BIG_ASTEROID; break;
        case FINAL_BOSS: score += SCORE_BOSS; break;
    }
}

int get_max_shield(int obj_type)
{
    int value;

    switch(obj_type)
    {
        case HERO_SPACESHIP: value = SHIELD_HERO_SPACESHIP; break;
	case ALIEN_SPACESHIP: value = SHIELD_ALIEN_SPACESHIP; break;
	case SMALL_ASTEROID: value = SHIELD_SMALL_ASTEROID; break;
	case MEDIUM_ASTEROID: value = SHIELD_MEDIUM_ASTEROID; break;
	case BIG_ASTEROID: value = SHIELD_BIG_ASTEROID; break;
    }

    return value;
}
