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

int rd_byte (char *cmd_ln)
{
	char   str_addr[CMDLEN+1];

	WORD   addr;

	get_token(cmd_ln, str_addr);

	if(!str_addr[0])
	{
		printf ("Usage : Rdb <address>\n\n\r");
		return 0;
	}

	if((sscanf(str_addr,"%x",&addr)!=1)||(!chkhex(str_addr)))
	{
		printf("Invalid Address.\n\r");
		return 0;
	}

	printf ("%02X\n\r",get_byte(addr));
}

int wr_byte (char *cmd_ln)
{
	char   str_addr[CMDLEN+1];
	char   str_dat[CMDLEN+1];
	char   *index;

	WORD   addr;
	BYTE   dat;

	index = get_token(cmd_ln, str_addr);
	get_token(index, str_dat);

	if((!str_addr[0])||(!str_dat[0]))
	{
		printf ("Usage : Wrb <address data>\n\n\r");
		return 0;
	}

	if((sscanf(str_addr,"%x",&addr)!=1)||(!chkhex(str_addr)))
	{
		printf("Invalid Address.\n\r");
		return 0;
	}
	if((sscanf(str_dat,"%x",&dat)!=1)||(!chkhex(str_dat))||(strlen(str_dat)>2))
	{
		printf("Invalid Data.\n\r");
		return 0;
	}
	put_byte(addr,dat);
	printf ("%04X:%02X\n\r",addr,get_byte(addr));
}

int dump (char *cmd_ln)
{
	char   str_addr[CMDLEN+1];
	char   str_line[CMDLEN+1];
	char   *index;

	WORD   addr,line;
	int		i,j;

	index = get_token(cmd_ln, str_addr);
	get_token(index, str_line);

	if(!str_addr[0])
	{
		printf ("Usage : dump <start address> <number of lines>\n\n\r");
		return 0;
	}

	if((sscanf(str_addr,"%x",&addr)!=1)||(!chkhex(str_addr)))
	{
		printf("Invalid Address.\n\r");
		return 0;
	}

	if(!str_line[0])
	line = 1;
	else
	sscanf(str_line,"%d",&line);

	for (i=0; i<line; i++)
	{
		printf (" %04X: ",addr+i*16);

		for(j=0;j<16;j++)
		{
			printf ("%02X ",(BYTE)get_byte((WORD)(addr+i*16+j)));
			if(j==7)
			printf(" ");
		}
		printf(" ");
		for(j=0;j<16;j++)
		{
			if(is_printable(get_byte((WORD)(addr+i*16+j))))
			printf("%c",get_byte((WORD)(addr+i*16+j)));
			else
			printf(".");
			if(j==7)
			printf(" ");
		}
		printf ("\n\r") ;
	}
}

int chkhex (char *word)
{

	while (*word)
	{
		if (isxdigit (*word))
		word++ ;
		else
		return 0 ;
	}
	return 1 ;

}

BYTE get_byte(WORD addr)
{
	WORD                *address ;

	address = (WORD *)addr ;
	return (*address) ;
}

void put_byte(WORD addr, BYTE data)
{
	BYTE	*address;

	address = (BYTE*)addr;
	*address = data;
}

BYTE asc2bin(BYTE data)
{
	if((data >= '0') && (data <= '9'))
	return (data - '0') ;
	else
	return (data - 'A' + 10) ;
}

BOOL is_printable(char ch)
{
	if((ch>=32)&&(ch<=126))
	return TRUE;
	return FALSE;
}