#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "cmd.h"
#include "rtc.h"
#include "e2p_24c32.h"

int cmd_write(char *c_ln);
int cmd_read(char *c_ln);
int cmd_set_date(char *cmd_ln);
int cmd_date(char *cmd_ln);

const CMD_TBL cmd_tbl[] =
{
    {"ewr", cmd_write},
    {"erd", cmd_read},
    {"date", cmd_date},
    {"set_date", cmd_set_date},		
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

int cmd_write(char *c_ln)
{
	char str_addr[MAXCMDLEN];
	char str_dat[MAXCMDLEN];
	uint8_t dat[E2P_PAGE_LEN];
	int addr, *index, byt;
	int len = 0;

	index = get_token(c_ln, str_addr);

	if(str_addr[0]==0)
	{
		printf("Usage:ewr <addr> <data> ...\n");
		return -1;
	}

	if (sscanf(str_addr, "%x", &addr) != 1)
	{
		printf("Invalid parameter\n");
		return -1;
	}
	printf("addr: %x\n", addr);
	
	do{
		printf("addra: %x\n", addr);
		index = get_token(index, str_dat);
		printf("addrb: %x\n", addr);
		if(str_dat[0]==0)
		{
			if(len==0){
				printf("Usage:ewr <addr> <data> ...\n");
				return -1;		
			}
			break;
		}
		printf("addrc: %x\n", addr);
		
		if (sscanf(str_dat, "%x", &byt) != 1)
		{
			if(len==0){
				printf("Invalid parameter\n");
				return -1;
			}
			break;
		}		
		dat[len++] = (uint8_t)byt;
		printf("addr: %x\n", addr);
		
	}while(len < E2P_PAGE_LEN);
	
	printf("Write %d byte(s) to addr: %x ...\n", len, addr);
	
	return write_e2p16((uint16_t)addr, dat, len);	
}

int cmd_read(char *c_ln)
{
	char str_addr[MAXCMDLEN];
	char str_len[MAXCMDLEN];
	int  addr, len, *index, i, j, lines;
	uint8_t	byt;

	index = get_token(c_ln, str_addr);
	get_token(index, str_len);

	if(str_addr[0]==0)
	{
		printf("Usage:erd <addr> <len>\n");
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
			if(addr >= E2P_MAX_ADDR){
				printf("\n");
				return 0;				
			}
			read_e2p16((uint16_t)addr++, &byt, 1);
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