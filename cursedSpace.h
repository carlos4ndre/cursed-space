#define SMALL 0
#define MEDIUM 1
#define BIG 2

#define GAME_ON_MAIN_MENU 0
#define GAME_RUNNING 1
#define GAME_PAUSED 2
#define GAME_FINISHED 3

#define LEVEL_TIMER 120
#define LEVEL_1 1
#define LEVEL_2 2
#define LEVEL_3 3
#define LEVEL_4 4
#define FINAL_BOSS 666

#define HERO_SPACESHIP 0
#define ALIEN_SPACESHIP 1
#define SMALL_ASTEROID 2
#define MEDIUM_ASTEROID 3
#define BIG_ASTEROID 4
#define PHOTON_TORPEDO 5
#define ION_CANNON 6

#define PRESS_DOWN 's'
#define PRESS_UP 'w'
#define PRESS_LEFT 'a'
#define PRESS_RIGHT 'd'
#define PRESS_FIRE_PHOTON_TORPEDOS 'f'
#define PRESS_ION_CANNON 'i'
#define PRESS_WARP 't'
#define PRESS_MASSIVE_BLACK_HOLE 'b'

#define SHIELD_HERO_SPACESHIP 100
#define SHIELD_SMALL_ASTEROID 10
#define SHIELD_MEDIUM_ASTEROID 20
#define SHIELD_BIG_ASTEROID 30
#define SHIELD_ALIEN_SPACESHIP 40
#define SHIELD_BOSS 500

#define SCORE_SMALL_ASTEROID 10
#define SCORE_MEDIUM_ASTEROID 20
#define SCORE_BIG_ASTEROID 30
#define SCORE_ALIEN_SPACESHIP 40
#define SCORE_BOSS 100

#define DAMAGE_PHOTON_TORPEDO 5
#define DAMAGE_ION_CANNON 10
#define DAMAGE_SMALL_ASTEROID 10
#define DAMAGE_MEDIUM_ASTEROID 20
#define DAMAGE_BIG_ASTEROID 30
#define DAMAGE_COLLISION_HERO_SPACESHIP 50

#define MAX_PHOTON_TORPEDOS 200
#define MAX_ION_CANNON 50
#define MAX_BLACK_HOLE 1
#define MAX_NUM_ENEMIES 20
#define RESPAWN_INTERVAL 2

#define STATUS_ALIVE 0
#define STATUS_DESTROYED 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define TELEPORT_RANGE_X 50
#define TELEPORT_RANGE_Y 20

#define NONE 0
#define ALIGN_CENTER 1
#define BG_WHITE_TXT_BLACK 1
#define BG_WHITE_TXT_BLUE 2
#define BG_WHITE_TXT_GREEN 3
#define BG_WHITE_TXT_YELLOW 4
#define BG_WHITE_TXT_RED 5
#define BG_BLACK_TXT_BLACK 6
#define BG_BLUE_TXT_WHITE 7
#define BG_RED_TXT_WHITE 8

typedef struct spaceObj
{
   int x0,x1,y0,y1;
   int type;
   int status;
   int shield;
   int color;
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
int random_number(int min_num, int max_num);

void print_text(int x,int y, char *title,int color,int style,int align,int display_speed);
void print_main_menu();
void print_howto_menu();
void print_youdied_menu();
void print_world();
void print_status_bar();
void print_obj(spaceObj *obj);
void init_space_obj_list();
spaceObj* get_space_obj(int position);
void add_space_obj(spaceObj *obj);
void remove_space_obj(int position);
int get_number_space_objs();
int get_number_enemies();
int is_enemy(int obj_type);
void respawn_enemy();
void run_space_garbage_collector();
void analyse_collisions();
int get_max_shield(int obj_type);
void take_damage(spaceObj *obj,int damage);
void update_score(int obj_type);

void init_level(int level);
spaceObj* init_hero_spaceship();
spaceObj* init_asteroid(int size);
spaceObj* init_photon_torpedo(int rel_x1,int rel_y1);
spaceObj* init_ion_cannon(int rel_x1,int rel_y1);
void respawn_obj(spaceObj *obj);
void check_level_status();
