#include <string.h>
#include <stdio.h>
#include <avr/io.h>

#include "adc.h"

#define BUFSIZE     32
#define MAXCMDLEN   12

char cbuf[BUFSIZE];
char saved_cmd[BUFSIZE];

extern float kp;
extern float ki;
extern float kd;

typedef struct command 
{
    char *cmd ;              /* command string (+1 for NUL terminator) */
    void (*cmdfunc)() ;       /* pointer to command function */
}CMD_TBL;

static char *get_token(char *line, char *cmd);
void run_cmd(char *cmd_line);
void show_help(char *c_ln);

extern void kp_ang(uint8_t angle);
extern void ki_ang(uint8_t angle);
extern void kd_ang(uint8_t angle);
extern void kpi_ang(uint8_t angle);
extern void kpid_ang(uint8_t angle);

void cmd_pwm(char *c_ln);
void cmd_read_adc(char *c_ln);
void cmd_kp_ang(char *c_ln);
void cmd_ki_ang(char *c_ln);
void cmd_kd_ang(char *c_ln);
void cmd_kpi_ang(char *c_ln);
void cmd_kpid_ang(char *c_ln);
void cmd_set_kp(char *c_ln);
void cmd_set_ki(char *c_ln);
void cmd_set_kd(char *c_ln);

const CMD_TBL cmd_tbl[] =
{
	{"skp", cmd_set_kp},	
	{"ski", cmd_set_ki},
	{"skd", cmd_set_kd},
	{"adc", cmd_read_adc},
	{"kp", cmd_kp_ang},		
	{"ki", cmd_ki_ang},
	{"kd", cmd_kd_ang},
	{"kpi", cmd_kpi_ang},
	{"kpid", cmd_kpid_ang},
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

void cmd_read_adc(char *c_ln)
{
	char str_param[MAXCMDLEN];	
	int param;
	uint16_t value;
	
	get_token(c_ln, str_param);
	if(str_param[0]!=0)
	{
		if(sscanf(str_param,"%d", &param)==1)
		{
			if((param >= 0)&&(param<=5)){
				value = adc_read(param);
				printf("ADC%d: %d\n", param, value);	
			}
		}
	}
}

void cmd_pwm(char *c_ln)
{
	char str_pwm[MAXCMDLEN];
	int pwm;
	int i;
	
	get_token(c_ln, str_pwm);

	if(str_pwm[0]!=0)
	{
		if(sscanf(str_pwm,"%d", &pwm)==1)
			set_pwm(pwm);
	}
}

void cmd_set_kp(char *c_ln)
{
	char str_kp[MAXCMDLEN];
	float kp_in;
	
	get_token(c_ln, str_kp);

	if(str_kp[0]!=0)
	{
		if(sscanf(str_kp,"%lf", &kp_in)==1)
			kp = kp_in;
	}
	printf("kp:%lf\n", kp);
}

void cmd_set_ki(char *c_ln)
{
	char str_ki[MAXCMDLEN];
	float ki_in;
	
	get_token(c_ln, str_ki);

	if(str_ki[0]!=0)
	{
		if(sscanf(str_ki,"%lf", &ki_in)==1){
			ki = ki_in;
		}
	}
	printf("ki:%lf\n", ki);
}

void cmd_set_kd(char *c_ln)
{
	char str_kd[MAXCMDLEN];
	float kd_in;
	
	get_token(c_ln, str_kd);

	if(str_kd[0]!=0)
	{
		if(sscanf(str_kd,"%lf", &kd_in)==1){
			kd = kd_in;
		}
	}
	printf("kd:%lf\n", kd);
}

void cmd_kp_ang(char *c_ln)
{
	char str_ang[MAXCMDLEN];
	int ang;
	int i;
	
	get_token(c_ln, str_ang);

	if(str_ang[0]!=0)
	{
		if(sscanf(str_ang,"%d", &ang)==1)
		{
			printf("angle:%d\n",ang);
			if((ang >= 0)&&(ang<=180)){
				kp_ang(ang+50); //0-180 to 60-240
			}
		}
	}
}

void cmd_ki_ang(char *c_ln)
{
	char str_ang[MAXCMDLEN];
	int ang;
	int i;
	
	get_token(c_ln, str_ang);

	if(str_ang[0]!=0)
	{
		if(sscanf(str_ang,"%d", &ang)==1)
		{
			printf("angle:%d\n",ang);
			if((ang >= 0)&&(ang<=180)){
				ki_ang(ang+50); //0-180 to 60-240
			}
		}
	}
}

void cmd_kd_ang(char *c_ln)
{
	char str_ang[MAXCMDLEN];
	int ang;
	int i;
	
	get_token(c_ln, str_ang);

	if(str_ang[0]!=0)
	{
		if(sscanf(str_ang,"%d", &ang)==1)
		{
			printf("angle:%d\n",ang);
			if((ang >= 0)&&(ang<=180)){
				kd_ang(ang+50); //0-180 to 60-240
			}
		}
	}
}

void cmd_kpi_ang(char *c_ln)
{
	char str_ang[MAXCMDLEN];
	int ang;
	int i;
	
	get_token(c_ln, str_ang);

	if(str_ang[0]!=0)
	{
		if(sscanf(str_ang,"%d", &ang)==1)
		{
			printf("angle:%d\n",ang);
			if((ang >= 0)&&(ang<=180)){
				kpi_ang(ang+50); //0-180 to 60-240
			}
		}
	}
}

void cmd_kpid_ang(char *c_ln)
{
	char str_ang[MAXCMDLEN];
	int ang;
	int i;
	
	get_token(c_ln, str_ang);

	if(str_ang[0]!=0)
	{
		if(sscanf(str_ang,"%d", &ang)==1)
		{
			printf("angle:%d\n",ang);
			if((ang >= 0)&&(ang<=180)){
				kpid_ang(ang+50); //0-180 to 60-240
			}
		}
	}
}

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
		//if(strncmp(token, cmd_ptr, sizeof(token))==0)
		//if(strncmp(token, cmd_ptr, sizeof(cmd_ptr))==0)
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

