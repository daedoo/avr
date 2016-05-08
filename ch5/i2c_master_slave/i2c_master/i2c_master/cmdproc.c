#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "uart.h"
#include "cmd.h"

char cbuf[BUFSIZE];
char saved_cmd[BUFSIZE];

void run_cmd(char *cmd_line);

void scan_cmd (char *chr_ptr)
{
	char *tptr;
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

	printf("\n");
	for (;;)
	{
		cbuf_ptr = cbuf;
		printf ("$ ");
		scan_cmd(cbuf_ptr);
		run_cmd(cbuf_ptr);
	}
}

char *get_token(char *str, char *cmd)
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
	char *token, *cmd_ptr;
	char *rem;
	
	rem = get_token(cmd_line, cmd);

	if (!cmd[0])
		return;

	p_tbl = (CMD_TBL *)cmd_tbl;
	cmd_ptr = p_tbl->cmd;
	token = cmd;

	while (p_tbl->cmd)
	{
		if(strcmp(token, cmd_ptr)==0)
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

int show_help(char *c_ln)
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
	return 0;
}
