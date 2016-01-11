#ifndef USHELL_CONFIG_H
#define USHELL_CONFIG_H

#define USHELL_CMD_BUF        64
#define USHELL_MAX_ARGS       3
#define USHELL_NEWLINE        '\n'
#define USHELL_NEWLINE_PRINT  "\r\n"
#define USHELL_PREDICT        '\t'
#define USHELL_DELETE         127

#ifdef USHELL_MAX_MEM_OPTS
#define USHELL_DISABLE_HELP             /* Disable help command and help text*/
#define USHELL_SMALL_INFO               /* Disable small info about author */
#define USHELL_DISABLE_MAX_ARGS_INFO    /* Disable max number arguments info */
#define USHELL_DISABLE_HINT             /* Disable double tap hint (bash-like option) */
#define USHELL_DISABLE_SPEC_KEYS_CHECK  /* Disable input keys check (enable whole ASCI on input) */
#define
#endif

#ifdef USHELL_MED_MEM_OPTS
#define USHELL_DISABLE_HELP
#define USHELL_SMALL_INFO
#define USHELL_DISABLE_MAX_ARGS_INFO
#define
#endif

#endif // USHELL_CONFIG_H
