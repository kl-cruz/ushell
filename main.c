#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include "ushell.h"

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


void ushell_putc( char c )
{
	putchar(c);
}

char ushell_getc( void )
{
	return (char) getchar();
}

int ushell_printf(const char * format, ...)
{
	int ret;
    va_list ap;
    va_start(ap, format);
    ret = vprintf(format, ap);
    va_end(ap);

    return ret;
}


int main( void )
{
	/*
	 * Configure terminal to get character without
	 * waiting for a new line.
	 */
	struct termios term_old, term_new;

	/* get old terminal settings */
	tcgetattr(STDIN_FILENO, &term_old);

	/* initialize new settings with old settings */
	memcpy( &term_new, &term_old, sizeof(struct termios));

	/* enable raw mode */
	cfmakeraw(&term_new);

	/* translate cartridge return to new line */
	term_new.c_iflag |= ICRNL;

	/* set new attributes */
	tcsetattr(STDIN_FILENO, TCSANOW, &term_new);

    ushell_init(user_cmds);
    ushell_loop();

	/* restore terminal attributes */
	tcsetattr(STDIN_FILENO, TCSANOW, &term_old);

	return 0;
}
