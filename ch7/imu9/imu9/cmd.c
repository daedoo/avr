#include <stdio.h>
#include <stdbool.h>

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


const CMDTBL cmd_tbl[] =
{
    {"help", show_help},
    {0, 0}
};



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

