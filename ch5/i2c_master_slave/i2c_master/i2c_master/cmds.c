#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "cmd.h"
#include "rtc.h"
#include "e2p_24c32.h"

extern int write_avr(uint8_t addr, uint8_t *data, int len);
extern int read_avr(uint8_t addr, uint8_t *data, int len);
extern int write_e2p16(uint16_t addr, uint8_t *data, int len);
extern int read_e2p16(uint16_t addr, uint8_t *data, int len);

int cmd_write_avr(char *c_ln);
int cmd_read_avr(char *c_ln);
int cmd_write_e2p(char *c_ln);
int cmd_read_e2p(char *c_ln);
int cmd_set_date(char *cmd_ln);
int cmd_date(char *cmd_ln);

const CMD_TBL cmd_tbl[] =
{
    {"awr", cmd_write_avr},
    {"ard", cmd_read_avr},
    {"ewr", cmd_write_e2p},
    {"erd", cmd_read_e2p},
    {"date", cmd_date},
    {"set_date", cmd_set_date},		
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

int write_slave_mem(char *c_ln, char *cmd, int max_addr, int max_page_len, int sub_mode, int (*func)())
{
	char str_addr[MAXCMDLEN];
	char str_dat[MAXCMDLEN];
	uint8_t dat[16];
	int addr, *index, byt;
	int len = 0;

	index = get_token(c_ln, str_addr);

	if(str_addr[0]==0)
	{
		printf("Usage:%s <addr> <data> ...\n", cmd);
		return -1;
	}

	if (sscanf(str_addr, "%x", &addr) != 1)
	{
		printf("Invalid parameter\n");
		return -1;
	}
	
	do{
		index = get_token(index, str_dat);
		if(str_dat[0]==0)
		{
			if(len==0){
				printf("Usage:%s <addr> <data> ...\n", cmd);
				return -1;		
			}
			break;
		}
		
		if (sscanf(str_dat, "%x", &byt) != 1)
		{
			if(len==0){
				printf("Invalid parameter\n");
				return -1;
			}
			break;
		}		
		dat[len++] = (uint8_t)byt;
		
	}while(((addr+len) <= max_addr)&&(len < max_page_len));
	
	printf("Write %d byte(s) to addr: %x ...\n", len, addr);
	if(sub_mode==16)
		return (*func)((uint16_t)addr, dat, len);	
	else
		return (*func)((uint8_t)addr, dat, len);		
}

int read_slave_mem(char *c_ln, char *cmd, int max_addr, int sub_mode, int (*func)())
{
	char str_addr[MAXCMDLEN];
	char str_len[MAXCMDLEN];
	int  addr, len, *index, i, j, lines;
	uint8_t	byt;

	index = get_token(c_ln, str_addr);
	get_token(index, str_len);

	if(str_addr[0]==0)
	{
		printf("Usage:%s <addr> <len>\n", cmd);
		return -1;
	}

	if (sscanf(str_addr, "%x", &addr) != 1)
	{
		printf("Invalid parameter\n");
		return -1;
	}

	if(str_len[0]==0)
	{
		len = 1;
	}
	else
	{
		if (sscanf(str_len, "%d", &len) != 1)
		{
			printf("Invalid length\n");
			return -1;
		}
	}
	
	lines = (len-1)/16 + 1;
	
	printf("Read %d byte(s) from addr: %x ...\n", len, addr);
	for (i = 0; i < lines; i++)
	{
		printf(" %04x: ", addr);

		for (j = 0; j < 16; j++)
		{
			if(addr >= max_addr){
				printf("\n");
				return 0;				
			}
			if(sub_mode==16)
				(*func)((uint16_t)addr++, &byt, 1);
			else
				(*func)((uint8_t)addr++, &byt, 1);
			printf("%02X ", byt);
			if(i*16 + j + 1 >= len)
			{
				printf("\n");
				return 0;
			}
		}
		printf("\n");
	}
	return 0;
}

int cmd_write_avr(char *c_ln){
	return write_slave_mem(c_ln, "awr", 15, 16, 8, write_avr);
}

int cmd_read_avr(char *c_ln){
	return read_slave_mem(c_ln, "ard", 15, 8, read_avr);	
}

int cmd_write_e2p(char *c_ln){
	return write_slave_mem(c_ln, "ewr", 4095, 16, 16, write_e2p16);	
}

int cmd_read_e2p(char *c_ln){
	return read_slave_mem(c_ln, "erd", 4095, 16, read_e2p16);
}


int cmd_date(char *cmd_ln)
{
	return date();
}

int cmd_set_date(char *cmd_ln)
{
	char	str_dat[12];
	char	*index;
	uint8_t	dat[7];
	int		val;
	int		ret, num=0;
	
	index = get_token(cmd_ln, str_dat);
	
	do
	{
		ret = sscanf(str_dat,"%x", &val);
		if(ret < 0)
		{
			if(num < 6)
			{
setdate_err:
				printf("usage : setdate <YY MM dd hh mm ss\n");
				return 0;
			}
			break;
		}
		dat[num] = (uint8_t)val;
		num++;
		index = get_token(index, str_dat);
	}while(1);

	/* check YY validity */
	if(((dat[0]&0xf0) >= 0xa0)||((dat[0]&0xf) >= 0xa))
	goto setdate_err;

	/* check MM validity */
	if(((dat[1]&0xf0) >= 0x20)||((dat[1]&0xf) >= 0xa)||(dat[1] >= 0x13))
	goto setdate_err;

	/* check dd validity */
	if(((dat[2]&0xf0) >= 0x40)||((dat[2]&0xf) >= 0xa)||(dat[2] >= 0x32))
	goto setdate_err;

	/* check hh validity */
	if(((dat[3]&0xf0) >= 0x30)||((dat[3]&0xf) >= 0xa)||(dat[3] >= 0x24))
	goto setdate_err;

	/* check mm validity */
	if(((dat[4]&0xf0) >= 0x60)||((dat[4]&0xf) >= 0xa)||(dat[4] >= 0x60))
	goto setdate_err;

	/* check ss validity */
	if(((dat[5]&0xf0) >= 0x60)||((dat[5]&0xf) >= 0xa)||(dat[5] >= 0x60))
	goto setdate_err;

	return set_date(dat[0], dat[1], dat[2], dat[3]&0x3f, dat[4], dat[5]);
}