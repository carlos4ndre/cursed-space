#define SMALL 0
#define MEDIUM 1
#define HIGH 2

#define GAME_ON_MAIN_MENU 0
#define GAME_RUNNING 1
#define GAME_PAUSED 2
#define GAME_FINISHED 3

#define HERO_SPACESHIP 0
#define ALIEN_SPACESHIP 1
#define ASTEROID 2
#define BLACK_HOLE 3

#define PRESS_DOWN 's'
#define PRESS_UP 'w'
#define PRESS_LEFT 'a'
#define PRESS_RIGHT 'd'
#define FIRE_PHOTON_TORPEDOS 'f'
#define ION_CANNON 'i'
#define WARP 't'
#define MASSIVE_BLACK_HOLE 'b'

#define MAX_SHIELD 10
#define MAX_PHOTON_TORPEDOS 5
#define MAX_ION_CANNON 1
#define MAX_BLACK_HOLE 1
#define MAX_WARP 1

#define STATUS_ALIVE 0
#define STATUS_DESTROYED 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define NONE 0
#define ALIGN_CENTER 1
#define BG_WHITE_TXT_BLACK 1
#define BG_WHITE_TXT_BLUE 2
#define BG_WHITE_TXT_GREEN 3
#define BG_WHITE_TXT_RED 4
#define BG_BLACK_TXT_BLACK 5

typedef struct spaceObj
{
   int x0,x1,y0,y1;
   int type;
   int status;
   char **image;
} spaceObj;

void initialize();
void render();
void resize();
void pause();
void resume();
void dispose();

void handle_user_input();
void move_obj(int direction,spaceObj *obj);
void teleport();
void summon_black_hole();
int random_number(int min_num, int max_num);

void print_text(int x,int y, char *title,int color,int style,int align,int display_speed);
void print_main_menu();
void print_world();
void print_obj(spaceObj *obj);

spaceObj init_hero_spaceship();
