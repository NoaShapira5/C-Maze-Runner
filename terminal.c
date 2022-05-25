#include <stdbool.h>
#include <termios.h>

/******************************
 * NO NEED TO UNDERSTAND THIS *
 ******************************/

void set_terminal(bool start)
{
    static struct termios old_settings;
    
    if (start) { 
        // save old settings and apply new settings
        tcgetattr(0, &old_settings);
        struct termios new_settings = old_settings;
        new_settings.c_lflag &= ~(ECHO|ICANON|ISIG);
        new_settings.c_cc[VMIN] = 0;
        new_settings.c_cc[VTIME] = 0;
        tcsetattr(0, TCSANOW, &new_settings);
    } else {
        // restore old settings
        tcsetattr(0, TCSANOW, &old_settings);
    }
}
