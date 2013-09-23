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

#define FIRE_PHOTON_TORPEDOS 0
#define ION_CANNON 1
#define WARP 2
#define MASSIVE_BLACK_HOLE 3

#define STATUS_ALIVE 0
#define STATUS_DESTROYED 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

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

void center_text(int row, char *title,int display_speed);
void print_main_menu();
void print_world();
void print_obj(spaceObj *obj);

spaceObj init_hero_spaceship();
