#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "maze.h"

const char *msg_normal[] = {"Press A/S/D/W to move, Q to quit"};
const char *msg_door[] = {"Door is locked!", "Pick up a key first!"};
const char *msg_key[] = {"Door unlocked, used 1 key"};
const char *msg_trap[] = {"Boom! You lose..."};
const char *msg_secret[] = {"This wall sounds hollow..."};
const char *msg_start[] = {"Welcome! ", "Hello! ", "Hi! ", "Let's play! "};
const char *msg_wall[] = {"Ouch!","You've hit the wall!", "Oops!", "Can't go there!", "Wrong way!"};
const char *msg_unknown[] = {"Unknown key pressed","Incorrect key!", "Wrong key", "Invalid key!"};
const char *msg_win[] = {"You win!","Epic!", "Good job!", "Amazing!", "Sweet!","Wow!","Extreme!","Fantastic!","Splendid!","No way!", "Super!", "Great!"};
const char *msg_quit[] = {"Goodbye!","Adios!","Auf Wiedersehen!", "Bye!", "see you soon!", "Until next time!", "Don't leave me :("};

int main(int argc, char *argv[])
{
    srand(time(NULL)); 
    if (2 == argc) {
        maze *new_maze = read_maze(argv[1]);
        print_maze(*new_maze);
        handle_input(*new_maze);
    } 
    return 0;
}

#define puts_no_newline(s) fputs(s, stdout)

maze* read_maze(char *filename)
{
    int x = 0;
    int count_player = 0;
    int count_target = 0;
    int count_keys = 0;
    int count_doors = 0;
    maze* maze_p;
    maze_p = malloc(sizeof(maze));
    if (NULL == maze_p) {
        return NULL;
    }
    char message[MAX_LEN];
    FILE *fp;
    fp = fopen(filename, "r");
    if (NULL == fp) {
        free(maze_p);
        fprintf(stderr, "Error opening maze\n");
        exit(1);
    }
    fgets(message, MAX_LEN, fp);
    for (int i = 0; i <MAX_LEN; i++) {
        if (message[i]=='\r') {
            message[i] = ' ';
        }
        if (message[i]=='\n') {
          message[i] = '\0';
          break;
        }
    }
    maze_p->message = message;
    maze_p->message = concat(maze_p->message,MSG_NORMAL);
    fscanf(fp, "%d %d", &maze_p->maze_rows, &maze_p->maze_cols);
    maze_p->tiles = create_maze(maze_p->maze_rows, maze_p->maze_cols);
    char array_maze[maze_p->maze_rows][maze_p->maze_cols];
    fseek(fp, 2, SEEK_CUR);
    while (fgets(array_maze[x], MAX_LEN, fp)) {
        array_maze[x][strlen(array_maze[x])-1] = '\0';
        x++;
    }
    for (int i = 0; i < maze_p->maze_rows; i++) {
        for (int j = 0; j < maze_p->maze_cols; j++) {
           maze_p->tiles[i][j] = array_maze[i][j];
            switch (array_maze[i][j]) {
                case '@':
                    maze_p->player.i = i;
                    maze_p->player.j = j;
                    count_player++;
                    break;
            
                case '$':
                    count_target++;
                    break;  
                case '&':
                    count_keys++;
                    break;
                case '+':
                    count_doors++;
                    break;
                case '\r':
                case '\n':
                case '-':
                case '|':
                case ' ':
                case '?':    
                case '!':
                case '#':
                case '\000':
                    break;
                default:
                    fprintf(stderr, "Error creating maze\n");
                    delete_maze(maze_p);
                    free(maze_p);
                    exit(1);
           } 
        }
    }
    maze_p->num_targets = count_target;
    maze_p->now_num_targets = 0;
    maze_p->old_targets = malloc(count_target * sizeof(location));
    if (NULL == maze_p->old_targets) {
        return NULL;
    }
    memset(maze_p->old_targets,0,maze_p->num_targets);
    if (1 != count_player || count_keys != count_doors) {
        fprintf(stderr, "Error creating maze\n");
        delete_maze(maze_p);
        exit(1);
    }
    maze_p->keys = count_keys;
    maze_p->curr_keys = 0;  
fclose(fp);  
return maze_p;
}

char** create_maze(int rows, int cols)
{
    char* data;
    char** tiles;
    data = malloc(cols * rows * sizeof(char));
    tiles = malloc(rows * sizeof(char*)+1);
    for (int i = 0; i<rows; i++) {
        tiles[i] = data + cols * i;
    }
    if (NULL == tiles || NULL == data) {
        return NULL;
    }
    free(data);
    return tiles;
}

void delete_maze(maze* maze_p)
{
    //free(maze_x.message);
    free(maze_p->tiles);
    free(maze_p->old_targets);
    //free(maze_p);
}

void print_tile(char *tile, char *color)
{
    char* result = concat(color, tile);
    puts_no_newline(result);
    free(result);
}

char* concat(const char *s1 , const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
   if (!result) {
        return NULL;
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int status_check(maze *maze_p)
{
    bool target_bool = false;
    for (int i = 0; i < maze_p->now_num_targets; i++) {
        if (maze_p->old_targets[i].i == maze_p->player.i && maze_p->old_targets[i].j == maze_p->player.j ) {
            target_bool = true;
            break;
        }
    }
    if (false == target_bool) {
        maze_p->old_targets[maze_p->now_num_targets].i = maze_p->player.i;
        maze_p->old_targets[maze_p->now_num_targets].j = maze_p->player.j;
        maze_p->now_num_targets++;
    }
    if (maze_p->now_num_targets == maze_p->num_targets) {
        return 1;
    } else {
        return 0;
    }
}

void print_row(maze maze_x)
{
    for (int j = 0; j < maze_x.maze_cols + 2; j++) {
        puts_no_newline(S_WALL_COLOR S_WALL);
    }
}

void print_maze(maze maze_x) 
{
    static unsigned frame_num; // this variable retains its value between calls
    // clear terminal
    puts_no_newline(CLEAR_SCREEN);
    // print frame number and message
    //printf("%-*.*s # of keys: %d%4d\n", maze_x.maze_rows-2, maze_x.maze_cols-2, maze_x.message, maze_x.curr_keys, frame_num++);
    printf("%-*.*s%d keys left%4d\n", maze_x.maze_cols-2, maze_x.maze_cols-2, maze_x.message, maze_x.curr_keys, frame_num++);
    // print header row
    print_row(maze_x);
    putchar('\n');
    // print maze rows (including player)
    for (int i = 0; i < maze_x.maze_rows; i++) {
        puts_no_newline(S_WALL_COLOR S_WALL);
        for (int j = 0; j < maze_x.maze_cols; j++) {
            if (i == maze_x.player.i && j == maze_x.player.j) {
                if ('$' == maze_x.tiles[i][j]) {
                    print_tile(S_TARGET, S_PLAYER_COLOR);
                }
                else if ('*' == maze_x.tiles[i][j]) {
                    print_tile(S_TARGET, S_PLAYER_COLOR);
                }
                else if ('?' == maze_x.tiles[i][j]) {
                    print_tile(T_SECRET, S_PLAYER_COLOR);
                }
                else if ('!' == maze_x.tiles[i][j]) {
                    puts_no_newline(S_FLOOR);
                }
                else if ('|' == maze_x.tiles[i][j]) {
                    puts_no_newline(S_PLAYER_COLOR T_VERTICAL);
                }
                else if ('-' == maze_x.tiles[i][j]) {
                    puts_no_newline(S_PLAYER_COLOR T_HORIZONTAL);
                }
                else if ('&' == maze_x.tiles[i][j]) {
                    puts_no_newline(S_PLAYER_COLOR S_PLAYER);
                }
                else if ('+' == maze_x.tiles[i][j] && 0 < maze_x.curr_keys) {
                    puts_no_newline(S_PLAYER_COLOR S_PLAYER);
                } else {
                    puts_no_newline(S_PLAYER_COLOR S_PLAYER);
                } 
            }
            else if ('$' == maze_x.tiles[i][j]) {
                puts_no_newline(S_TARGET_COLOR S_TARGET);
            } 
            else if ('*' == maze_x.tiles[i][j]) {
                puts_no_newline(T_VISITED_COL S_TARGET);
            }
            else if ('#' == maze_x.tiles[i][j]) {
                puts_no_newline(S_WALL_COLOR S_WALL);
            }
            else if ('?' == maze_x.tiles[i][j]) {
                puts_no_newline(S_WALL_COLOR T_SECRET);
            }
            else if ('!' == maze_x.tiles[i][j]) {
                puts_no_newline(T_TRAP);
            }
            else if ('-' == maze_x.tiles[i][j]) {
                puts_no_newline(T_HORIZONTAL_COLOR T_HORIZONTAL);
            }
            else if ('|' == maze_x.tiles[i][j]) {
                puts_no_newline(T_VERTICAL_COLOR T_VERTICAL);
            }
            else if ('&' == maze_x.tiles[i][j]) {
                puts_no_newline(T_KEY);
            }
            else if ('+' == maze_x.tiles[i][j]) {
                puts_no_newline(T_DOOR);
            } else {
                puts_no_newline(S_FLOOR);
            }
        }
        puts_no_newline(S_WALL_COLOR S_WALL);
        putchar('\n');
    }
    // print footer row
    print_row(maze_x);
    putchar('\n');
}

int handle_input(maze maze_x)
{
    maze* maze_p = &maze_x;
    set_terminal(true);
    bool game_over = false;
    do {
        maze_x.message = MSG_NORMAL;

        // get user input
        int tmp;
        int key = getchar();
        switch(key) {
            case EOF:
            case 'q':
            case 'Q':   // exit game
                maze_x.message = MSG_QUIT;
                game_over = true;
                break;
            case 'a':
            case 'A':   // go left
                if (-1 == maze_x.player.j - 1) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('#' == maze_x.tiles[maze_x.player.i][maze_x.player.j - 1] || '|' == maze_x.tiles[maze_x.player.i][maze_x.player.j - 1]) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j - 1] && 0 == maze_x.curr_keys) {
                    maze_x.message = MSG_DOOR;
                    break;
                }
                maze_x.player.j--;
                if ('?' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_SECRET;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j] && 0 < maze_x.curr_keys) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.message = MSG_KEY;
                    maze_x.curr_keys--;
                }
                else if ('&' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.curr_keys++;
                }
                else if ('!' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_TRAP;
                    game_over = true;
                }
                else if ('$' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    tmp = status_check(maze_p);
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = '*';
                    if (1 == tmp) {
                        maze_x.message = MSG_WIN;
                        game_over = true;
                    }
                }
                break;
            case 'd':
            case 'D':   // go right
                if (maze_x.maze_cols == maze_x.player.j + 1) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('#' == maze_x.tiles[maze_x.player.i][maze_x.player.j + 1] || '|' == maze_x.tiles[maze_x.player.i][maze_x.player.j + 1]) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j + 1] && 0 == maze_x.curr_keys) {
                    maze_x.message = MSG_DOOR;
                    break;
                }
                maze_x.player.j++;
                if ('?' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_SECRET;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j] && 0 < maze_x.curr_keys) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.message = MSG_KEY;
                    maze_x.curr_keys--;
                }
                else if ('&' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.curr_keys++;
                }
                else if ('!' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_TRAP;
                    game_over = true;
                }
                else if ('$' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    tmp = status_check(maze_p);
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = '*';
                    if (1 == tmp) {
                        maze_x.message = MSG_WIN;
                        game_over = true;
                    }
                }
                break;
            case 'w':
            case 'W':   // go up
                if (-1 == maze_x.player.i - 1) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('#' == maze_x.tiles[maze_x.player.i - 1][maze_x.player.j] || '-' == maze_x.tiles[maze_x.player.i - 1][maze_x.player.j]) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i - 1][maze_x.player.j] && 0 == maze_x.curr_keys) {
                    maze_x.message = MSG_DOOR;
                    break;
                }
                maze_x.player.i--;
                if ('?' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_SECRET;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j] && 0 < maze_x.curr_keys) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.message = MSG_KEY;
                    maze_x.curr_keys--;
                }
                else if ('&' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.curr_keys++;
                }
                else if ('!' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_TRAP;
                    game_over = true;
                }
                else if ('$' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = '*';
                    tmp = status_check(maze_p);
                    if (1 == tmp) {
                        maze_x.message = MSG_WIN;
                        game_over = true;
                    }
                }
                break;
            case 's':
            case 'S':   // go down
                if (maze_x.maze_rows == maze_x.player.i + 1) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('#' == maze_x.tiles[maze_x.player.i + 1][maze_x.player.j] || '-' == maze_x.tiles[maze_x.player.i + 1][maze_x.player.j]) {
                    maze_x.message = MSG_WALL;
                    break;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i + 1][maze_x.player.j] && 0 == maze_x.curr_keys) {
                    maze_x.message = MSG_DOOR;
                    break;
                }
                maze_x.player.i++;
                if ('?' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_SECRET;
                }
                else if ('+' == maze_x.tiles[maze_x.player.i][maze_x.player.j] && 0 < maze_x.curr_keys) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.message = MSG_KEY;
                    maze_x.curr_keys--;
                }
                else if ('&' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = ' ';
                    maze_x.curr_keys++;
                }
                else if ('!' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.message = MSG_TRAP;
                    game_over = true;
                }
                else if ('$' == maze_x.tiles[maze_x.player.i][maze_x.player.j]) {
                    maze_x.tiles[maze_x.player.i][maze_x.player.j] = '*';
                    tmp = status_check(maze_p);
                    if (1 == tmp) {
                        maze_x.message = MSG_WIN;
                        game_over = true;
                    }
                }
                break;
            default:
                maze_x.message = MSG_UNKNOWN;
        }
        // print maze
        print_maze(maze_x);
    } while (!game_over);
    set_terminal(false);
    delete_maze(maze_p);
    return game_over;
}