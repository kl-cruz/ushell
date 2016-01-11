#include <stdio.h>
#include <stdlib.h>
#include "ushell.h"
#include <curses.h>
#include <string.h>

void args_cmd( char* argv[], int argc )
{
    unsigned i;
    printf("arguments count: %d\r\n", argc);
    for(i = 0; i< argc; ++i) {
        printf("argument %d: %s\r\n", i, argv[i]);
    }
}

static ushell_cmd_def_t user_cmds[] = {
    {"args", args_cmd},
    {NULL, NULL}
};

void ushell_putc(char c)
{
    //putchar(c);
    addch(c);
}

char ushell_getc()
{
    return (char)getch();
    //return (char)getchar();
}

int ushell_printf(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}

int main(void)
{
    initscr();
    noecho();
    refresh();

    ushell_init(user_cmds);
    ushell_loop();

    endwin();
    return 0;
}

