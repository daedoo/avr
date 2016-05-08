#ifndef _CMD_H_
#define _CMD_H_

#define BUFSIZE     32
#define MAXCMDLEN   12

char cbuf[BUFSIZE];
char saved_cmd[BUFSIZE];

typedef struct command 
{
    char *cmd ;              /* command string (+1 for NUL terminator) */
    void (*cmdfunc)() ;       /* pointer to command function */
}CMD_TBL;

extern const CMD_TBL cmd_tbl[];

char *get_token(char *line, char *cmd);
extern void cmd_loop(void);
int show_help(char *c_ln);

#endif

