#include "cursedSpace.h"
#include "linked_list.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define NUMBER_OBJS 20

int level = 0;
time_t level_timer = 0;
int game_status = GAME_ON_MAIN_MENU;
spaceObj gameObjs[NUMBER_OBJS];

/*****************************
** Where the universe starts
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
   init_pair(BG_WHITE_TXT_RED,COLOR_RED,COLOR_WHITE);
   init_pair(BG_BLACK_TXT_BLACK,COLOR_BLACK,COLOR_BLACK);
   init_pair(BG_BLUE_TXT_WHITE,COLOR_WHITE,COLOR_BLUE);

   bkgd(use_default_colors());


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
           move_obj(DOWN,&gameObjs[0]);
           break;
      case PRESS_UP:
           move_obj(UP,&gameObjs[0]);
           break;
      case PRESS_RIGHT:
           move_obj(RIGHT,&gameObjs[0]);
           break;
      case PRESS_LEFT:
           move_obj(LEFT,&gameObjs[0]);
           break;
      case PRESS_FIRE_PHOTON_TORPEDOS:
           move_obj(RIGHT,&gameObjs[0]);
           break;
      case PRESS_ION_CANNON:
           blast_them_with_ion_cannon();
           break;
      case PRESS_WARP:
           teleport();
           break;
      case PRESS_MASSIVE_BLACK_HOLE:
           summon_black_hole();
           break;
   }
}


void check_level_status()
{
    time_t now = time(NULL);	
    int elapsed_time = now - level_timer;

    if(elapsed_time < 60) 
          return;

    switch(level)
    {
       case LEVEL_1: 
           level = LEVEL_2;
           init_level(LEVEL_2);
           break;
       case LEVEL_2: 
           level = LEVEL_3;
           init_level(LEVEL_3);
           break;
       case LEVEL_3: 
           level = LEVEL_4;
           init_level(LEVEL_4);
           break;
       case LEVEL_4: 
           level = FINAL_BOSS;
           init_level(FINAL_BOSS);
           break;
       case FINAL_BOSS:
           break;
           // do something
    }
}

void process_world_events()
{
   int obj_number, obj_type;

   for(obj_number=1;obj_number<NUMBER_OBJS;obj_number++)
   {
	obj_type = gameObjs[obj_number].type;

	switch(obj_type)
	{
            case HERO_SPACESHIP:
                // multiplayer? :) 
		break;
            case ALIEN_SPACESHIP:
                // do something
                break;
            case ASTEROID:
                move_obj(LEFT,&gameObjs[obj_number]);
                break;
            default:
		// do something
                break;
	}
   }
}


/*****************************
** Vectorial functions
*****************************/

void teleport() 
{
   int max_x,max_y;
   int middle_screen;
   int x0,x1,len;

   getmaxyx(stdscr,max_y,max_x);
   middle_screen = max_x/2;

   x0 = gameObjs[0].x0;
   x1 = gameObjs[0].x1;
   len = x1 - x0;

   if(x0 > middle_screen && x0-middle_screen > 0)
   {
          gameObjs[0].x0 -= middle_screen;
          gameObjs[0].x1 -= middle_screen;
   }
   else if(x0 > middle_screen && x0-middle_screen/2 > 0)
   {
          gameObjs[0].x0 -= middle_screen/2;
          gameObjs[0].x1 -= middle_screen/2;
   }
   else if(x1 < middle_screen && x1+middle_screen < max_x) 
   {
          gameObjs[0].x0 += middle_screen;
	  gameObjs[0].x1 += middle_screen;
   }
   else //if(x1 < middle_screen && x1+middle_screen/2 < max_x)
   {
          gameObjs[0].x0 += middle_screen/2;
          gameObjs[0].x1 += middle_screen/2;
   }
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

void init_level(int level)
{
   int i;
   for(i=0;i<NUMBER_OBJS;i++)
         gameObjs[i].status = STATUS_DESTROYED;

   spaceObj hero_spaceship;
   init_hero_spaceship(&hero_spaceship);
   gameObjs[0] = hero_spaceship;

   switch(level)
   {
      case LEVEL_1:
         init_asteroid(&gameObjs[1],BIG);
         // a lot to do!
      case LEVEL_2:
      case LEVEL_3:
      case LEVEL_4:
      case FINAL_BOSS:
         break;
   }

   level_timer = time(NULL);
}

void init_hero_spaceship(spaceObj *obj) 
{
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
}


void init_asteroid(spaceObj *obj,int size)
{
   int i,max_x,max_y;
   int asteroid_width = 0;
   int asteroid_height = 0;

   getmaxyx(stdscr,max_y,max_x);       /* get screen width */
   max_x--;

   switch(size)
   {
	case SMALL:
	     asteroid_height = 4;
	     asteroid_width = 4;
             break;
        case MEDIUM:
             asteroid_height = 6;
             asteroid_width = 8;
             break;
        case BIG:
             asteroid_height = 10;
             asteroid_width = 18;
             break;
   }

   /* hardcode values because they are totally dependent on the asteroid's image */
   obj->x0 = max_x - asteroid_width;
   obj->x1 = max_x;
   obj->y0 = max_y-30 - asteroid_height/2;
   obj->y1 = max_y-30 + asteroid_height/2;

   obj->type = ASTEROID;

   char **image = malloc(asteroid_height*sizeof(char*));
   for(i = 0; i < asteroid_height; i++)
        image[i] = malloc(asteroid_width*sizeof(char));

   switch(size)
   {
        case SMALL:
            strcpy(image[0]," __ ");
            strcpy(image[1],"/  \\");
            strcpy(image[2],"\\__/");
	    strcpy(image[3],"    ");
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
   }

   obj->image = image;
}


void print_world()
{
   int i;
   for(i=0;i<NUMBER_OBJS;i++)
   {
      if(gameObjs[i].status != STATUS_DESTROYED) 
      {
      	print_obj(&gameObjs[i]);
      }
      else if(gameObjs[i].status == STATUS_DESTROYED)
      {
        respawn_obj(&gameObjs[i]); 
      }
   }
}

void respawn_obj(spaceObj *obj)
{
   int obj_status = obj->status;
   obj->status = STATUS_ALIVE;
}

void print_obj(spaceObj *obj)
{
   char **image = obj->image;
   int x,y,max_lines = obj->y1-obj->y0;

   for(y=0;y<max_lines;y++,image++) 
   {
      print_text(obj->x0,obj->y0+y,*image,BG_WHITE_TXT_BLACK,NONE,NONE,0);
   }
}


void summon_black_hole() 
{
   int max_x,max_y;
   int x_center,y_center;
   int x,y;

   getmaxyx(stdscr,max_y,max_x);

   x_center = max_x/2;
   y_center = max_y/2;

   for(x=0;x<x_center;x++) {
      for(y=0;y<y_center;y++) 
      {
   	  print_text(x_center+x,y_center+y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,1);
	  print_text(x_center+x,y_center-y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,1);
          print_text(x_center-x,y_center+y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,1);
          print_text(x_center-x,y_center-y,"*",BG_BLACK_TXT_BLACK,NONE,NONE,1);
      }
    }
}


void blast_them_with_ion_cannon()
{
   int max_x,max_y;
   int x1,y0,y1;
   int column,line;
   getmaxyx(stdscr,max_y,max_x);

   x1 = gameObjs[0].x1;
   y0 = gameObjs[0].y0;
   y1 = gameObjs[0].y1;

   for(column=x1;column<max_x;column++) 
   {
       for(line=y0;line<y1;line++) 
       {
            if(column % 2)
            	print_text(column,line,"-",BG_BLUE_TXT_WHITE,NONE,NONE,1);
            else
                print_text(column,line,"~",BG_BLUE_TXT_WHITE,NONE,NONE,1);
       }
   }
}

