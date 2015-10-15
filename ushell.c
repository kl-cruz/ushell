#include "stdlib.h"
#include "ushell.h"
#include "ushell_config.h"

typedef struct {
	char                buf[USHELL_CMD_BUF];
	unsigned            buf_pos;
	ushell_cmd_def_t    *user_cmds;
	unsigned            state;
} ushell_ctx_t;

static ushell_ctx_t ctx;

/*ushell atomic functions*/

int ushell_strlen(char *str) {
	unsigned j = 0;
	while(str[j] != '\0') {
		++j;
	}
	return j;
}

int ushell_strncmp(const char *s1, const char *s2, size_t n) {
	unsigned i;
	for( i = 0; i < n; ++i) {
		if(s1[i] != s2[i]) {
			return 1;
		}
	}
	return 0;
}

int putchar(int c)
{
	ushell_putc((char) c);
	return 1;
}

/*Basic ushell functions*/

void help_cmd( char* argv[], int argc );
void info_cmd( char* argv[], int argc );
void lscmd_cmd( char* argv[], int argc );
void exit_cmd( char* argv[], int argc );

static ushell_cmd_def_t basic_cmds[] = {
	#ifndef USHELL_DISABLE_HELP
	{"help", help_cmd},
	#endif
	{"info", info_cmd},
	{"lscmd", lscmd_cmd},
	{"exit", exit_cmd},
	{NULL, NULL}
};

#ifndef USHELL_DISABLE_HELP
void help_cmd( char* argv[], int argc )
{
	(void)(argv);
	(void)(argc);
	ushell_printf("ushell is small and simple shell for your constrainted\n");
	ushell_printf("devices. Be careful. In this application your command is\n");
	ushell_printf("limited to %d chars and arguments to %d.\n", USHELL_CMD_BUF, USHELL_MAX_ARGS);
	ushell_printf("It is simple help:\n");
	ushell_printf(" - Type command and args with space key: cmd arg1 arg2\n");
	ushell_printf(" - Double pressed tab key can show you commands hint\n");
	ushell_printf(" - Type lscmd and press enter to list all command\n");

}
#endif

void info_cmd( char* argv[], int argc )
{
	(void)(argv);
	(void)(argc);
	ushell_printf("ushellell version %d.%d\n", USHELL_VER_MAJOR, USHELL_VER_MINOR);
#ifndef USHELL_SMALL_INFO
	ushell_printf("ushell is small and simple shell for constrainted\n");
	ushell_printf("microcontrollers and processors. ushell can be used\n");
	ushell_printf("with 8, 16, 32 and 64bit architectures. It requieres\n");
	ushell_printf("to implement only two functions: putc and getc. \n");
	ushell_printf("\n");
	ushell_printf("For license constrainted see LICENSE file.\n");
	ushell_printf("\n");
#endif
	ushell_printf("Repository is available at: http://github.com/kl-cruz/ushell\n");
	ushell_printf("\n");
	ushell_printf("Karol {cruz} Lasonczyk 2015");
}

void lscmd_cmd( char* argv[], int argc )
{
	(void)(argv);
	(void)(argc);
	unsigned i = 0;
	ushell_printf("System commands:\n");

	while(basic_cmds[i].cmd != NULL) {
		ushell_printf("    %s\n", basic_cmds[i++].cmd_str);
	}

	i = 0;
	ushell_printf("User commands:\n");

	if(ctx.user_cmds != NULL) {
		while(ctx.user_cmds[i].cmd != NULL) {
			ushell_printf("    %s\n", ctx.user_cmds[i++].cmd_str);
		}
	}
}

void exit_cmd( char* argv[], int argc )
{
	ctx.state = 0;
	(void)(argv);
	(void)(argc);
}

/*ushell main functions*/

unsigned ushell_exec_cmd(char *line, ushell_cmd_def_t *cmds)
{
	size_t cmd_str_pos = 0;
	unsigned i = 0, cmd_i = 0, args_i = 0, result = 0;
	char *args[USHELL_MAX_ARGS];
	char c = line[i++];

	while(c!=' ' && c!='\0') {
		c = line[i++];
		cmd_str_pos++;
	}

	/*ushell is not depend of any library so strcmp is implemented below*/
	while( cmds[cmd_i].cmd != NULL) {
		char *cmd_s = cmds[cmd_i].cmd_str;
		unsigned j = 0;

		j = ushell_strlen(cmd_s);

		if(cmd_str_pos != j) {
			cmd_i++;
			continue;
		}

		i = ushell_strncmp(cmds[cmd_i].cmd_str, line, j);

		/*Probably we found desired command*/
		if(i != 0) {
			cmd_i++;
			continue;
		}

		cmd_str_pos = 0;
		if((line[j+1] == ' ') || (line[j+1] == '\0') || (cmds[cmd_i].cmd_str[j] == '\0') ) {
			/*Ok that is our function! Let's parse args*/
			do {
				c = line[j++];
				if((c == '\0')) {
					break;
				}
				if((c == ' ')) {
					args[args_i++] = line+j;
				}
			} while(c != '\0' && (args_i <= USHELL_MAX_ARGS));

#ifndef USHELL_DISABLE_MAX_ARGS_INFO
			if(args_i > USHELL_MAX_ARGS) {
				ushell_printf("[ushell WARN] Function has too much arguments! Max %d.\n", USHELL_MAX_ARGS);
			}
#endif

			/*arguments must be separate by zero to be parsable like strings*/
			for(i = 0; i < USHELL_CMD_BUF; ++i) {
				if(line[i] == ' ') {
					line[i] = '\0';
				}
			}
			cmds[cmd_i].cmd(args, args_i);
			result = 1;
		}
		cmd_i++;
	}
	if (cmd_str_pos != 0) {
		line[cmd_str_pos] = 0;

	}

	return result;
}

#ifndef USHELL_DISABLE_HINT
char* ushell_hint(char* line, ushell_cmd_def_t *basic_cmds, ushell_cmd_def_t *user_cmds)
{
	size_t cmd_str_pos = 0;
	unsigned i = 0, cmd_i = 0, founds = 0;
	char *cmd;
	char c;

	if(line[0] == '\0') {
		unsigned i = 0;
		while(basic_cmds[i].cmd != NULL) {
			ushell_printf("%s\n", basic_cmds[i++].cmd_str);
		}

		i = 0;
		if(ctx.user_cmds != NULL) {
			while(ctx.user_cmds[i].cmd != NULL) {
				ushell_printf("%s\n", ctx.user_cmds[i++].cmd_str);
			}
		}

	} else {
		do {
			c = line[i++];
			cmd_str_pos++;
		} while(c!=' ' && c!='\0');
		cmd_str_pos--;

		/*ushell is not depend of any library so strncmp is implemented below*/
		while( basic_cmds[cmd_i].cmd != NULL) {
			for( i = 0; i < cmd_str_pos; ++i) {
				if(basic_cmds[cmd_i].cmd_str[i] != line[i]) {
					i = 0;
					break;
				}
			}
			/*Probably we found desired command*/
			if(i != 0) {
				founds++;
				cmd = basic_cmds[cmd_i].cmd_str;
				ushell_printf("%s\n", basic_cmds[cmd_i].cmd_str);
			}
			cmd_i++;
		}
	}
	if(founds == 1) {
		return cmd;
	}
	return line;
}
#endif

void ushell_loop(void)
{
	char c;
	unsigned i, prediction = 0;
	ushell_printf("ushell version %d.%d\n", USHELL_VER_MAJOR, USHELL_VER_MINOR);
	ushell_putc('\n');
	ushell_putc('>');
	ushell_putc(' ');
	while(ctx.state > 0) {
		c = ushell_getc();

		if(c == USHELL_NEWLINE) {
			ushell_putc('\n');
			if(ushell_exec_cmd(ctx.buf, basic_cmds) == 1) {
				ushell_putc('\n');
			} else if(ushell_exec_cmd(ctx.buf, ctx.user_cmds) == 1) {
				ushell_putc('\n');
			} else {
				ushell_printf("[ushell ERROR] Command %s not recognized!\n", ctx.buf);
			}

			for(i = 0; i < ctx.buf_pos; ++i) {
				ctx.buf[i] = 0;
			}
			ctx.buf_pos = 0;

			if(ctx.state > 0) {
				ushell_putc('>');
				ushell_putc(' ');
			}

			continue;
		}

#ifndef USHELL_DISABLE_HINT
		if(c == USHELL_PREDICT) {
			prediction++;
			if(prediction == 2) {
				ushell_printf("\n");
				char* cmd = ushell_hint(ctx.buf, basic_cmds, ctx.user_cmds);
				prediction = 0;
				for(i = 0; i < ushell_strlen(cmd); ++i) {
					ctx.buf[i] = cmd[i];
				}
				ushell_printf("> %s", cmd);
			}
			continue;
		}
#endif

		if(c == USHELL_DELETE) {
			ctx.buf_pos--;
			ctx.buf[ctx.buf_pos] = '\0';
			ushell_putc('\b');
			ushell_putc(' ');
			ushell_putc('\b');
			continue;
		}

		if(c > 0 && ctx.buf_pos < USHELL_CMD_BUF) {
			prediction = 0;

#ifndef USHELL_DISABLE_ECHO
			ushell_putc(c);
#endif

			ctx.buf[ctx.buf_pos++] = c;
		}
	}
}

void ushell_init(ushell_cmd_def_t *cmd_list)
{
	ctx.user_cmds = cmd_list;
	ctx.state = 1;
	ctx.buf_pos = 0;
}
