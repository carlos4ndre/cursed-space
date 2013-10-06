#define SMALL 0
#define MEDIUM 1
#define BIG 2

#define GAME_ON_MAIN_MENU 0
#define GAME_RUNNING 1
#define GAME_PAUSED 2
#define GAME_FINISHED 3

#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 3
#define LEVEL_4 4
#define FINAL_BOSS 666

#define HERO_SPACESHIP 0
#define ALIEN_SPACESHIP 1
#define ASTEROID 2
#define PHOTON_TORPEDO 3
#define ION_CANNON 4

#define PRESS_DOWN 's'
#define PRESS_UP 'w'
#define PRESS_LEFT 'a'
#define PRESS_RIGHT 'd'
#define PRESS_FIRE_PHOTON_TORPEDOS 'f'
#define PRESS_ION_CANNON 'i'
#define PRESS_WARP 't'
#define PRESS_MASSIVE_BLACK_HOLE 'b'

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
#define BG_BLUE_TXT_WHITE 6

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
void process_world_events();
void move_obj(int direction,spaceObj *obj);
void teleport();
void summon_black_hole();
void blast_them_with_ion_cannon();
int random_number(int min_num, int max_num);

void print_text(int x,int y, char *title,int color,int style,int align,int display_speed);
void print_main_menu();
void print_howto_menu();
void print_world();
void print_obj(spaceObj *obj);
void init_space_obj_list();
spaceObj* get_space_obj(int position);
void add_space_obj(spaceObj *obj);
int get_number_space_objs();

void init_level(int level);
spaceObj* init_hero_spaceship();
spaceObj* init_asteroid(int size);
spaceObj* init_photon_torpedo(int rel_x1,int rel_y1);
void respawn_obj(spaceObj *obj);
void check_level_status();
