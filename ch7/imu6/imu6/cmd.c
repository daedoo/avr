#include <stdio.h>
#include <stdbool.h>
#include "pwm.h"
#include "spi.h"

#define BUFSIZE  80
#define CMDLEN   12        

char cbuf[BUFSIZE];

typedef struct command 
{
    char *cmd ;              /* command string (+1 for NUL terminator) */
    int (*cmdfunc)() ;       /* pointer to command function */
}CMDTBL;

static char *get_token(char *line, char *cmd);
int run_cmd(char *cmd_line);
int show_help(char *c_ln);
/*int cmd_set_left_wheel_speed(char *c_ln);
int cmd_set_right_wheel_speed(char *c_ln);
int cmd_spi_test(char *c_ln);

int cmd_set_go(char *c_ln);
int cmd_set_back(char *c_ln);
int cmd_set_left(char *c_ln);
int cmd_set_right(char *c_ln);
int cmd_set_stop(char *c_ln);*/

const CMDTBL cmd_tbl[] =
{
    {"help", show_help},
/*    {"?", show_help},
    {"spi", cmd_spi_test},
    {"lsp", cmd_set_left_wheel_speed},
    {"rsp", cmd_set_right_wheel_speed},
    {"go", cmd_set_go},
    {"back", cmd_set_back},
    {"right", cmd_set_right},
    {"left", cmd_set_left},
    {"stop", cmd_set_stop},*/
    {0, 0}
};

/*
int cmd_spi_test(char *c_ln)
{
	spi_test2();	
	return 0;
}

int cmd_set_go(char *c_ln)
{
	set_go_forward(20);
	return 0;
}

int cmd_set_back(char *c_ln)
{
	set_go_backward(20);
	return 0;
}

int cmd_set_left(char *c_ln)
{
	set_go_left(20);
	return 0;
}

int cmd_set_right(char *c_ln)
{
	set_go_right(20);
	return 0;
}

int cmd_set_stop(char *c_ln)
{
	set_stop();
	return 0;
}


int cmd_set_left_wheel_speed(char *c_ln)
{
	char str_speed[CMDLEN];
	int speed;
	
	get_token(c_ln, str_speed);
	
	if(str_speed[0]!=0)
	{
		if(sscanf(str_speed,"%d",&speed) == 1)
		{	
			if(speed > 100) 
				speed = 100;
			if(speed <-100)
				speed = -100;
			printf("speed: %d\n", speed);
			set_left_wheel_speed((char)speed);
		}
	}
	return 0;
}

int cmd_set_right_wheel_speed(char *c_ln)
{
	char str_speed[CMDLEN];
	int speed;
	
	get_token(c_ln, str_speed);
	
	if(str_speed[0]!=0)
	{
		if(sscanf(str_speed,"%d",&speed)==1)
		{
			if(speed > 100)
				speed = 100;
			if(speed <-100)
				speed = -100;
			printf("speed: %d\n", speed);
			set_right_wheel_speed((char) speed);
		}
	}
	return 0;
}
*/

void cmd_loop(void)
{
	char *cbuf_ptr;

	printf("\n");
	for (;;)
    {
	    cbuf_ptr = cbuf;
        printf ("$ ");
        *cbuf_ptr++ = '\0';
        *cbuf_ptr++ = '\0';
		if (fgets(cbuf_ptr, sizeof cbuf - 1, stdin) == NULL)
			break;
        run_cmd(cbuf_ptr);
	}
}

static char *get_token(char *line, char *cmd)
{
    int i;

	/* skip blanks*/
    while ((*line == ' ') || (*line == '\t'))  
        ++line;

    for (i = 0; i < CMDLEN; i++)
    {
		/* break out of the loop when meet the end of line*/
        if ((*line == ' ') || (*line == '\n') || (*line == '\0'))
            break;      
		/* get a byte*/
        *cmd++ = *line++;      
    }
	/* end of the string*/
    *cmd = '\0';

   	/* return pointer to the next char*/
    return(line);   
}

int run_cmd(char *cmd_line)
{
	CMDTBL *p_tbl;
    char cmd[CMDLEN+1];
    char *token, *cmd_ptr;
    char *rem;

    rem = get_token(cmd_line, cmd);

    if (!cmd[0]) 
		return 0 ;

	p_tbl = (CMDTBL *)cmd_tbl;
    token = p_tbl->cmd;
	cmd_ptr = cmd;

    while (p_tbl->cmd) 
	{
        if (*token != *cmd_ptr) 
		{
			/* different command string, go to the next */
            if ((*cmd_ptr)) 
			{
                p_tbl++ ;
                token = p_tbl->cmd ;
				cmd_ptr = cmd;
            } 
			else
                return ((*p_tbl->cmdfunc)(rem));
        } 
		else 
		{
            if (!(*cmd_ptr))
                return ((*p_tbl->cmdfunc)(rem));
			/* same char, go to the next char */
            cmd_ptr++ ;
            token++ ;
        }
    }

    if (!p_tbl->cmd) 
	{
        printf ("%s", "Invalid Command! (See Help.)\n\r");
    }
	return 0 ;
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

