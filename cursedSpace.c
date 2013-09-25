#include "cursedSpace.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define NUMBER_OBJS 20

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
      handle_user_input();
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
   curs_set(0);
   start_color();

   init_pair(BG_WHITE_TXT_BLACK,COLOR_BLACK,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_BLUE,COLOR_BLUE,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_GREEN,COLOR_GREEN,COLOR_WHITE);
   init_pair(BG_WHITE_TXT_RED,COLOR_RED,COLOR_WHITE);
   init_pair(BG_BLACK_TXT_BLACK,COLOR_BLACK,COLOR_BLACK);

   bkgd(use_default_colors());


   int i;
   for(i=0;i<NUMBER_OBJS;i++)
	 gameObjs[i].status = STATUS_DESTROYED;

   spaceObj hero_spaceship = init_hero_spaceship();
   gameObjs[0] = hero_spaceship;
}


void render()
{
   clear();

   switch(game_status)
   {
      case GAME_ON_MAIN_MENU:
            print_main_menu();
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

/*****************************
** User Input Handlers
*****************************/

void handle_user_input() 
{
   char ch;

   ch = getch();

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
      case FIRE_PHOTON_TORPEDOS:
           move_obj(RIGHT,&gameObjs[0]);
           break;
      case ION_CANNON:
           move_obj(RIGHT,&gameObjs[0]);
           break;
      case WARP:
           teleport();
           break;
      case MASSIVE_BLACK_HOLE:
           summon_black_hole();
           break;
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
   int x0 = obj->x0;
   int x1 = obj->x1;
   int y0 = obj->y0;
   int y1 = obj->y1;
               	
   switch(direction)
   {
      case DOWN:
               y0++; y1++;
               if(!position_valid(x0,y0,x1,y1)) break;
               obj->y0++;
               obj->y1++;
               break;
      case UP:
               y0--; y1--;
               if(!position_valid(x0,y0,x1,y1)) break;
               obj->y0--;
               obj->y1--;
               break;
      case LEFT:
               x0--; x1--;
               if(!position_valid(x0,y0,x1,y1)) break;
               obj->x0--;
               obj->x1--;
               break;
      case RIGHT:
               x0++; x1++;
               if(!position_valid(x0,y0,x1,y1)) break;
               obj->x0++;
               obj->x1++;
               break;
      default:
               break;
   }
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



spaceObj init_hero_spaceship() 
{
   spaceObj obj;
   int i,len,center_x,center_y;
   int spaceship_width = 38;
   int spaceship_height = 8;

   getmaxyx(stdscr,center_y,center_x);       /* get screen width */
   center_x /= 2;
   center_y /= 2;

   /* hardcode values because they are totally dependent on the ship's image */
   obj.x0 = center_x - spaceship_width/2;
   obj.x1 = center_x + spaceship_width/2;
   obj.y0 = center_y - spaceship_height/2;
   obj.y1 = center_y + spaceship_height/2;

   obj.type = HERO_SPACESHIP;
   obj.status = STATUS_ALIVE;

   char **image = malloc(spaceship_height*sizeof(char*));
   for(i = 0; i < spaceship_height; i++)
        image[i] = malloc( spaceship_width*sizeof(char));

   strcpy(image[0],"                                   ");
   strcpy(image[1],"_____________         _______________");
   strcpy(image[2],"\\____________)  _____/ == killer == / ");
   strcpy(image[3],"      | |      /_:::_______________/ ");
   strcpy(image[4],"      | |       /  /    \\_______/    ");
   strcpy(image[5],"   ___|_|______/__/                  ");
   strcpy(image[6],"   ==  ... ...   ||||                ");
   strcpy(image[7],"   `--___________||||                ");

    obj.image = image;

    return obj;
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
   }
}


void print_obj(spaceObj *obj)
{
   char **image = obj->image;
   int x,y,max_lines = obj->y1-obj->y0;

   for(y=0;y<max_lines;y++,image++) 
   {
      print_text(obj->x0,obj->y0+y,*image,BG_WHITE_TXT_BLUE,NONE,NONE,0);
   }
}


void summon_black_hole() 
{
   int max_x,max_y;
   int x_center,y_center;
   int x,y;
   start_color();

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


/*

void print_asteroid(int size)
{

   switch(size)
   {
      case SMALL: 
            center(22," ___ ");
            center(23,"/   \\");
            center(24,"\\___/");
            break;

      case MEDIUM:
            center(22," .-;;-. ");
            center(23,"/      \\");
            center(24,"|        |");
            center(25,"\\      /");
            center(26,"  '-..-'  ");
            break;

      case HIGH:
            center(22,"      ______      ");
            center(23,"    .'      '.    ");
            center(24,"  .'          '.  ");
            center(25," /              \\ ");
            center(26,";                ;");
            center(27,"|                |");
            center(28,";                ;");
            center(29," \\              / ");
            center(30,"  '.          .'  ");
            center(31,"    '-.____.-'    ");
   }
}


void print_photon_torpedo()
{
   center(22,"***");
}
               
*/ 
