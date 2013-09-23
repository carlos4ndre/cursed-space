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
            //print_main_menu();
            //getchar();
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
   int ch;

   ch = getch();
   switch(ch)
   {
      case KEY_DOWN:
               move_obj(DOWN,&gameObjs[0]);
               break;
      case KEY_UP:
               move_obj(UP,&gameObjs[0]);
               break;
      case KEY_LEFT:
               move_obj(LEFT,&gameObjs[0]);
               break;
      case KEY_RIGHT:
               move_obj(RIGHT,&gameObjs[0]);
               break;
      default:
               break;
   }
}

/*****************************
** Vectorial functions
*****************************/

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

void center_text(int row, char *title,int display_speed)
{
        int len,indent,y,width;

        getmaxyx(stdscr,y,width);       /* get screen width */

        len = strlen(title);            /* get title's length */
        indent = width - len;           /* subtract it from screen width */
        indent /= 2;                    /* divide result into two */


	move(row,indent);

	while(*title)			   /* loop through the whole string */
	{
		addch(*title);		   /* put one char to curscr */
		title++;		   /* increment the pointer */
		refresh();		   /* update the screen */
		napms(display_speed);  /* delay a bit to see the display */
	}
}


/*****************************
** All printable content
*****************************/

void print_main_menu() 
{
   int logo_display_speed = 1;
   int history_display_speed = 50;
   int start_display_speed = 1;

   center_text(1," _______           _______  _______  _______  ______  ",logo_display_speed);
   center_text(2,"(  ____ \\|\\     /|(  ____ )(  ____ \\(  ____ \\(  __  \\ ",logo_display_speed);
   center_text(3,"| (    \\/| )   ( || (    )|| (    \\/| (    \\/| (  \\  )",logo_display_speed);
   center_text(4,"| |      | |   | || (____)|| (_____ | (__    | |   ) |",logo_display_speed);
   center_text(5,"| |      | |   | ||     __)(_____  )|  __)   | |   | |",logo_display_speed);
   center_text(6,"| |      | |   | || (\\ (         ) || (      | |   ) |",logo_display_speed);
   center_text(7,"| (____/\\| (___) || ) \\ \\__/\\____) || (____/\\| (__/  )",logo_display_speed);
   center_text(8,"(_______/(_______)|/   \\__/\\_______)(_______/(______/ ",logo_display_speed);
   center_text(10," _______  _______  _______  _______  _______   ",logo_display_speed);
   center_text(11,"(  ____ \\(  ____ )(  ___  )(  ____ \\(  ____ \\ ",logo_display_speed);
   center_text(12,"| (    \\/| (    )|| (   ) || (    \\/| (    \\/ ",logo_display_speed);
   center_text(13,"| (_____ | (____)|| (___) || |      | (__     ",logo_display_speed);
   center_text(14,"(_____  )|  _____)|  ___  || |      |  __)    ",logo_display_speed);
   center_text(15,"      ) || (      | (   ) || |      | (       ",logo_display_speed);
   center_text(16,"/\\____) || )      | )   ( || (____/\\| (____/\\ ",logo_display_speed);
   center_text(17,"\\_______)|/       |/     \\|(_______/(_______/ ",logo_display_speed);


   center_text(22,"On a remote terminal very far far far away",history_display_speed);
   center_text(23,"there is a universe on the edge of its very existence,",history_display_speed);
   center_text(24,"infested with ASCII asteroids and aliens.",history_display_speed);
   center_text(25,"But there is still hope!",history_display_speed);
   center_text(26,"You can make the difference aboard the killer ship!",history_display_speed);
   center_text(27,"Your terminal is counting on you! Good hunt!",history_display_speed);


   center_text(35,"Press any key to start",start_display_speed);
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
      char *line = *image;

      while(*line)                 /* loop through the whole string */
      {
         move(obj->y0+y,obj->x0+x);
         addch(*line);             /* put one char to curscr */
         line++;
         x++;
      }
      x = 0;
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
