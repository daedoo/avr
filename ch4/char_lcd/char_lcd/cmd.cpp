#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "uart.h"

#define BUFSIZE     32
#define MAXCMDLEN   12

char cbuf[BUFSIZE];
char saved_cmd[BUFSIZE];

typedef struct command 
{
    char *cmd ;              /* command string (+1 for NUL terminator) */
    void (*cmdfunc)(char*) ;      /* pointer to command function */
}CMD_TBL;

static char *get_token(char *line, char *cmd);
void run_cmd(char *cmd_line);
void show_help(char *c_ln);

extern void hello(void);
extern void blink(void);
extern void cursor(void);
extern void display(void);
extern void direction(void);
extern void scroll(void);
extern void serial(void);
extern void setCursor(void);
extern void autoScroll(void);

void cmd_hello(char *c_ln){
	hello();	
}
void cmd_blink(char *c_ln){
	blink();
}
void cmd_cursor(char *c_ln){
	cursor();
}
void cmd_display(char *c_ln){
	display();
}
void cmd_direction(char *c_ln){
	direction();
}
void cmd_scroll(char *c_ln){
	scroll();
}
void cmd_serial(char *c_ln){
	serial();
}
void cmd_setCursor(char *c_ln){
	setCursor();
}
void cmd_autoScroll(char *c_ln){
	autoScroll();
}


const CMD_TBL cmd_tbl[] =
{
	{"hello", cmd_hello},
	{"blink", cmd_blink},
	{"cursor", cmd_cursor},
	{"display", cmd_display},
	{"direction", cmd_direction},
	{"scroll", cmd_scroll},
	{"serial", cmd_serial},
	{"setcursor", cmd_setCursor},
	{"autoscroll", cmd_autoScroll},
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

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

