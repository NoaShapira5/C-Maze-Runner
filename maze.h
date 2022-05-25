#include <stdbool.h>

//We chose using vertical (|), horizontal (-), Trap (!), secret wall (?), multi-targets, keys (&) and doors (+)//

#define MAX_LEN 256
#define size(array) (sizeof(array) / sizeof(array[0]))
#define MSG_NORMAL  msg_normal[rand() % size(msg_normal)]
#define MSG_TRAP    msg_trap[rand() % size(msg_trap)]
#define MSG_SECRET  msg_secret[rand() % size(msg_secret)]
#define MSG_START   msg_start[rand() % size(msg_start)]
#define MSG_WALL    msg_wall[rand() % size(msg_wall)]
#define MSG_UNKNOWN msg_unknown[rand() % size(msg_unknown)]
#define MSG_WIN     msg_win[rand() % size(msg_win)]
#define MSG_QUIT    msg_quit[rand() % size(msg_quit)]
#define MSG_DOOR    msg_door[rand() % size(msg_door)]
#define MSG_KEY     msg_key[rand() % size(msg_key)]

typedef struct {
    unsigned i;
    unsigned j;
}location;

typedef struct {
    location player;
    unsigned maze_cols;
    unsigned maze_rows;
    const char *message;
    char **tiles;
    unsigned num_targets;
    int now_num_targets;
    location *old_targets;
    int keys;
    int curr_keys;

} maze;

char* concat(const char *s1 , const char *s2);
int status_check(maze *maze_x);
int handle_input(maze maze_x);
void print_tile(char *tile, char *color);
void print_row(maze maze_x);
void set_terminal(bool start);
void print_maze(maze maze_x);
void delete_maze(maze *maze_x);
char** create_maze(int rows, int cols);
maze* read_maze(char *filename);

/************************************
 * TERMINAL CODES ("MAGIC STRINGS") *
 ************************************/

#define COLOR_OFF       "\e[0m"
#define COLOR_BLACK     "\e[0;30m"
#define COLOR_GRAY      "\e[1;30m"
#define COLOR_RED       "\e[0;31m"
#define COLOR_LT_RED    "\e[1;31m"
#define COLOR_GREEN     "\e[0;32m"
#define COLOR_LT_GREEN  "\e[1;32m"
#define COLOR_BROWN     "\e[0;33m"
#define COLOR_YELLOW    "\e[1;33m"
#define COLOR_BLUE      "\e[0;34m"
#define COLOR_LT_BLUE   "\e[1;34m"
#define COLOR_PURPLE    "\e[0;35m"
#define COLOR_LT_PURPLE "\e[1;35m"
#define COLOR_CYAN      "\e[0;36m"
#define COLOR_LT_CYAN   "\e[1;36m"
#define COLOR_LT_GRAY   "\e[0;37m"
#define COLOR_WHITE     "\e[1;37m"

#define CLEAR_SCREEN    "\e[2J\e[1;1H"

/*****************************
 * SYMBOLS FOR GAME ELEMENTS *
 *****************************/

#define S_FLOOR            " "
#define S_WALL             "#" COLOR_OFF
#define S_WALL_COLOR       COLOR_GRAY
#define S_PLAYER           "@" COLOR_OFF
#define S_PLAYER_COLOR     COLOR_BLUE
#define S_TARGET           "$" COLOR_OFF
#define T_VISITED_COL      COLOR_GREEN
#define S_TARGET_COLOR     COLOR_LT_RED
#define T_SECRET           S_WALL
#define T_TRAP             S_FLOOR
#define T_VERTICAL         "|" COLOR_OFF
#define T_VERTICAL_COLOR   S_WALL_COLOR
#define T_HORIZONTAL       "-" COLOR_OFF
#define T_HORIZONTAL_COLOR S_WALL_COLOR
#define T_KEY              COLOR_LT_PURPLE "&" COLOR_OFF
#define T_DOOR             COLOR_LT_CYAN  "+" COLOR_OFF
