/*ushellell project*/
#ifndef USHELL_H
#define USHELL_H

#define USHELL_VER_MAJOR 0
#define USHELL_VER_MINOR 1

typedef void (*ushell_cmd_handler_t)( char* argv[], int argc );

typedef struct {
    const char* cmd_str;
    ushell_cmd_handler_t cmd;
} ushell_cmd_def_t;

void ushell_init(ushell_cmd_def_t *cmd_list);
void ushell_loop(void);

/*Developer must implement putc and getc. These functions are platform
 * dependent functions.
 */
void ushell_putc(char c);
char ushell_getc(void);
#endif // USHELL_H
