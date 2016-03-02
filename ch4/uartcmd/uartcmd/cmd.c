#include <string.h>
#include <stdio.h>
#include <avr/io.h>

#define BUFSIZE     32
#define MAXCMDLEN   12  

char cbuf[BUFSIZE];
char saved_cmd[BUFSIZE];

typedef struct command 
{
    char *cmd ;              /* command string (+1 for NUL terminator) */
    void (*cmdfunc)() ;      /* pointer to command function */
}CMD_TBL;

static char *get_token(char *line, char *cmd);
void run_cmd(char *cmd_line);
void show_help(char *c_ln);

const CMD_TBL cmd_tbl[] =
{	
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

void scan_cmd (char *chr_ptr)
{
	char *tptr, *xptr;	
	while(1)
	{
		*chr_ptr = ((char) uart_getchar_stream(0));

		if(*chr_ptr=='['){
			*chr_ptr = ((char) uart_getchar_stream(0));
			if(*chr_ptr=='A'){
				if(saved_cmd[0]==0){
					continue;
				} else { // saved command exist
					tptr = saved_cmd;
					while(*tptr){
						uart_putchar(*tptr); // print saved cmd
						*chr_ptr++ = *tptr++; // copy cmd string
					}
					uart_getchar_stream(0); // clear '\n'
					continue;
				}
			}
		}
		if (*chr_ptr == '\n')
		*chr_ptr = '\0' ;
		
		if(!*chr_ptr)
			break;
		chr_ptr++;
	}
}

void cmd_loop(void)
{
	char *cbuf_ptr;
	int i;

	printf("\n");
	for (;;)
    {
	    cbuf_ptr = cbuf;
        printf ("$ ");
		scan_cmd(cbuf_ptr);
        run_cmd(cbuf_ptr);
	}
}

static char *get_token(char *str, char *cmd)
{
    int i;

	// skip blank or else
    while ((*str == ' ') || (*str == '\t'))  
        ++str;

    for (i = 0; i < MAXCMDLEN; i++)
    {
		// end of line or else
        if ((*str == ' ') || (*str == '\n') || (*str == '\0'))
            break;      
        *cmd++ = *str++;      
    }
	// add end of string
    *cmd = '\0';

   	// return ptr to the next
    return str;   
}

void run_cmd(char *cmd_line)
{
	CMD_TBL *p_tbl;
    char cmd[MAXCMDLEN+1];
    char *token, *cmd_ptr, *ptr;
    char *rem;
	int i;
	
    rem = get_token(cmd_line, cmd);

    if (!cmd[0])
		return;

	p_tbl = (CMD_TBL *)cmd_tbl;
	cmd_ptr = p_tbl->cmd;
	token = cmd;

    while (p_tbl->cmd) 
	{
		if(strncmp(token, cmd_ptr, sizeof(token))==0)
		{
            strcpy(saved_cmd, cmd_line);
			((*p_tbl->cmdfunc)(rem));
			return;
		}
		p_tbl++;
		cmd_ptr = p_tbl->cmd;
	}

    if (!p_tbl->cmd) 
        printf ("Invalid Command! (See Help.)\n");
}

void show_help(char *c_ln)
{
	int i=0;

	while(cmd_tbl[i].cmd != NULL)
	{
		printf("%s ", cmd_tbl[i].cmd);
		i++;
		if((i%7)==0)
			printf("\n");
	}
	printf("\n");
}

