#include "ushell.h"
#include "stdlib.h"
#include "ushell_config.h"

typedef struct {
    char                buf[USHELL_CMD_BUF];
    unsigned            buf_pos;
    ushell_cmd_def_t    *user_cmds;
    unsigned            state;
} ushell_ctx_t;

static ushell_ctx_t ctx;

/*ushell atomic functions*/

int ushell_strlen(const char *str) {
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
    ushell_printf("ushell is small and simple shell for your constrainted"USHELL_NEWLINE_PRINT);
    ushell_printf("devices. Be careful. In this application your command is"USHELL_NEWLINE_PRINT);
    ushell_printf("limited to %d chars and arguments to %d."USHELL_NEWLINE_PRINT, USHELL_CMD_BUF, USHELL_MAX_ARGS);
    ushell_printf("It is simple help:"USHELL_NEWLINE_PRINT);
    ushell_printf(" - Type command and args with space key: cmd arg1 arg2"USHELL_NEWLINE_PRINT);
    ushell_printf(" - Double pressed tab key can show you commands hint"USHELL_NEWLINE_PRINT);
    ushell_printf(" - Type lscmd and press enter to list all command"USHELL_NEWLINE_PRINT);

}
#endif

void info_cmd( char* argv[], int argc )
{
    (void)(argv);
    (void)(argc);
    ushell_printf("ushellell version %d.%d"USHELL_NEWLINE_PRINT, USHELL_VER_MAJOR, USHELL_VER_MINOR);
#ifndef USHELL_SMALL_INFO
    ushell_printf("ushell is small and simple shell for constrainted"USHELL_NEWLINE_PRINT);
    ushell_printf("microcontrollers and processors. ushell can be used"USHELL_NEWLINE_PRINT);
    ushell_printf("with 8, 16, 32 and 64bit architectures. It requieres"USHELL_NEWLINE_PRINT);
    ushell_printf("to implement only three functions: putc, getc and printf."USHELL_NEWLINE_PRINT);
    ushell_printf(USHELL_NEWLINE_PRINT);
    ushell_printf("For license constrainted see LICENSE file."USHELL_NEWLINE_PRINT);
    ushell_printf(USHELL_NEWLINE_PRINT);
#endif
    ushell_printf("Repository is available at: http://github.com/kl-cruz/ushell"USHELL_NEWLINE_PRINT);
    ushell_printf(USHELL_NEWLINE_PRINT);
    ushell_printf("Karol {cruz} Lasonczyk 2015"USHELL_NEWLINE_PRINT);
}

void lscmd_cmd( char* argv[], int argc )
{
    (void)(argv);
    (void)(argc);
    unsigned i = 0;
    ushell_printf("System commands:"USHELL_NEWLINE_PRINT);

    while(basic_cmds[i].cmd != NULL) {
        ushell_printf("    %s"USHELL_NEWLINE_PRINT, basic_cmds[i++].cmd_str);
    }

    i = 0;
    ushell_printf("User commands:"USHELL_NEWLINE_PRINT);

    if(ctx.user_cmds != NULL) {
        while(ctx.user_cmds[i].cmd != NULL) {
            ushell_printf("    %s"USHELL_NEWLINE_PRINT, ctx.user_cmds[i++].cmd_str);
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
        const char *cmd_s = cmds[cmd_i].cmd_str;
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
            } while(c != '\0' && (args_i < USHELL_MAX_ARGS));

#ifndef USHELL_DISABLE_MAX_ARGS_INFO
            if(args_i > USHELL_MAX_ARGS) {
                ushell_printf("[ushell WARN] Function has too much arguments! Max %d."USHELL_NEWLINE_PRINT, USHELL_MAX_ARGS);
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

    return result;
}

#ifndef USHELL_DISABLE_HINT
const char* ushell_hint(char* line, ushell_cmd_def_t *basic_cmds, ushell_cmd_def_t *user_cmds)
{
    size_t cmd_str_pos = 0;
    unsigned i = 0, cmd_i = 0, founds = 0;
    const char *cmd;
    char c;

    if(line[0] == '\0') {
        unsigned i = 0;
        while(basic_cmds[i].cmd != NULL) {
            ushell_printf("%s"USHELL_NEWLINE_PRINT, basic_cmds[i++].cmd_str);
        }

        i = 0;
        if(ctx.user_cmds != NULL) {
            while(ctx.user_cmds[i].cmd != NULL) {
                ushell_printf("%s"USHELL_NEWLINE_PRINT, ctx.user_cmds[i++].cmd_str);
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
                ushell_printf("%s"USHELL_NEWLINE_PRINT, basic_cmds[cmd_i].cmd_str);
            }
            cmd_i++;
        }
        if(user_cmds != NULL){
            cmd_i = 0;
            while( user_cmds[cmd_i].cmd != NULL) {
                for( i = 0; i < cmd_str_pos; ++i) {
                    if(user_cmds[cmd_i].cmd_str[i] != line[i]) {
                        i = 0;
                        break;
                    }
                }
                /*Probably we found desired command*/
                if(i != 0) {
                    founds++;
                    cmd = user_cmds[cmd_i].cmd_str;
                    ushell_printf("%s"USHELL_NEWLINE_PRINT, user_cmds[cmd_i].cmd_str);
                }
                cmd_i++;
            }
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
    ushell_printf(USHELL_NEWLINE_PRINT"ushell version %d.%d"USHELL_NEWLINE_PRINT, USHELL_VER_MAJOR, USHELL_VER_MINOR);
    ushell_putc('>');
    ushell_putc(' ');
    while(ctx.state > 0) {
        c = ushell_getc();

#ifndef USHELL_DISABLE_SPEC_KEYS_CHECK
        if((c < 0x20) && (c > 127) && (c != USHELL_NEWLINE)
                && (c != USHELL_PREDICT) && (c != USHELL_DELETE)) {
            continue;
        }
#endif

#ifndef USHELL_DISABLE_HINT
        if(c == USHELL_PREDICT) {
            prediction++;
            if(prediction == 2) {
                ushell_printf(USHELL_NEWLINE_PRINT);
                const char* cmd_hint = ushell_hint(ctx.buf, basic_cmds, ctx.user_cmds);
                prediction = 0;
                ushell_putc('>');
                ushell_putc(' ');
                for(i = 0; i < ushell_strlen(cmd_hint); ++i) {
                    ctx.buf[i] = cmd_hint[i];
                    ushell_putc(cmd_hint[i]);
                }
                ctx.buf_pos = i;
            }
            continue;
        } else {
            prediction = 0;
        }
#endif

        if(c == USHELL_NEWLINE) {
            ushell_printf(USHELL_NEWLINE_PRINT);
            if(ushell_exec_cmd(ctx.buf, basic_cmds) == 1) {
                ushell_printf(USHELL_NEWLINE_PRINT);
            } else if(ushell_exec_cmd(ctx.buf, ctx.user_cmds) == 1) {
                ushell_printf(USHELL_NEWLINE_PRINT);
            } else if(ushell_strlen(ctx.buf) == 0) {
                ;
            } else {
                ushell_printf("[ushell ERROR] Command %s not recognized!"USHELL_NEWLINE_PRINT, ctx.buf);
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

        if((c == USHELL_DELETE) && (ctx.buf_pos != 0)) {
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
