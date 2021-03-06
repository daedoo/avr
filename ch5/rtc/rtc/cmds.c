#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include "cmd.h"
#include "rtc.h"

int cmd_set_date(char *cmd_ln);
int cmd_date(char *cmd_ln);

const CMD_TBL cmd_tbl[] =
{
    {"date", cmd_date},
    {"set_date", cmd_set_date},
    {"help", show_help},
    {"?", show_help},
    {0, 0}
};

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

