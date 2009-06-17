
/*	$Id: memcmds.c,v 1.1.1.1 2006/06/29 06:43:25 cpu Exp $ */

/*
 * Copyright (c) 2000-2001 Opsycon AB  (www.opsycon.se)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB, Sweden.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


unsigned int smbaddr = 0;

#define ior(reg) *(volatile unsigned char *)(smbaddr +(reg))
#define iow(v, reg) *(volatile unsigned char *)(smbaddr + (reg)) =(v)

static void i2cwait()
{
	int to = 1000;

	while (to > 0){
		if(ior(0x01) & 0x40) 
			return; /*ok*/

		if(ior(0x01) & 0x30) 
			return; /*bad*/
		to --;
	}
}

unsigned char i2crd(int slave, int reg)
{
	unsigned char v, sts;


	v = ior(0x03); /*ctrl*/
	iow(v|0x01, 0x03);
	sts = ior(0x01);
	if(sts & 0x20)
		return 0xff;
	i2cwait();

#if	1
	iow(slave, 0x00); /*send slave address) */ 
	sts = ior(0x01);
	if(sts & 0x30)
		return 0xff;
	i2cwait();
#endif
	v = ior(0x03);
	iow(v|0x10, 0x03); /*Ack*/

	iow(reg,0x00); /*Send command)*/ 
	if(ior(0x01)&0x30)
		return 0xff;
	i2cwait();

	v = ior(0x03);
	iow(v|0x10, 0x03); /*Ack*/


#if	1
	v = ior(0x03);   //start agatin 
	iow(v|0x01, 0x03);
	if(ior(0x01)&0x20)
		return 0xff;

	iow(slave|0x01, 0x00); //send slave address) 
	sts = ior(0x01);
	if(sts & 0x30)
		return 0xff;
	i2cwait();
#endif

#if	0
	iow(0x80,0x00); /*Write data */
	v = ior(0x03);
	iow(v|0x10, 0x03); /*Ack*/
#endif

	v = ior(0x00); /*read data*/

	iow(0x2, 0x03); /*stop*/
	i2cwait();


	return  v;

}

unsigned char i2cwr(int slave, int reg, int val)
{
	unsigned char v, sts;


	v = ior(0x03); /*ctrl*/
	iow(v|0x01, 0x03);
	sts = ior(0x01);
	if(sts & 0x20)
		return 0xff;
	i2cwait();

#if	1
	iow(slave, 0x00); /*send slave address) */ 
	sts = ior(0x01);
	if(sts & 0x30)
		return 0xff;
	i2cwait();
#endif
	v = ior(0x03);
	iow(v|0x10, 0x03); /*Ack*/

	iow(reg,0x00); /*Send command)*/ 
	if(ior(0x01)&0x30)
		return 0xff;
	i2cwait();

	v = ior(0x03);
	iow(v|0x10, 0x03); /*Ack*/


#if	0
	v = ior(0x03);   //start agatin 
	iow(v|0x01, 0x03);
	if(ior(0x01)&0x20)
		return 0xff;

	iow(slave|0x01, 0x00); //send slave address) 
	sts = ior(0x01);
	if(sts & 0x30)
		return 0xff;
	i2cwait();
#endif

#if	1
	iow(val,0x00); /*Write data */
	v = ior(0x03);
	i2cwait();

	iow(v|0x10, 0x03); /*Ack*/
#endif

	iow(0x2, 0x03); /*stop*/
	i2cwait();


	return  v;

}

/*

int cmd_i2cread( int ac, char *av[])
{
	u_int32_t slave, reg, i;
	int len = 1;
	unsigned char val;

	if (!get_rsa (&slave, av[1]) || !get_rsa (&reg, av[2])) {
		return (-1);
	}

	if(ac == 4) {
		get_rsa (&len, av[3]) ;
		if(len <= 0)
			len = 1;
	}

	for(i=0; i<len; i++) {
		val = i2crd(slave, reg + i);
		printf("%02d: %02x\n", reg +i, val);
	}

	return (0);
}

int cmd_i2cwrite( int ac, char *av[])
{
	u_int32_t slave, reg, i;
	int len = 1;
	unsigned char val;

	if (!get_rsa (&slave, av[1]) || !get_rsa (&reg, av[2])) {
		return (-1);
	}

	if(ac == 4) {
		get_rsa (&val, av[3]) ;
	}

	val = i2cwr(slave, reg, val);

	return (0);
}

*/

#define SMBADDR 0x4c90

int main() 
{
	int fd = 0;
	unsigned int iobaseaddr = 0;
	int i = 0;
	unsigned char value = 0;
		
	if ((fd = open("/dev/mem", O_RDWR)) < 0) {
		perror("open");
		return -1;
	}
	
	iobaseaddr = mmap( 0,
				0x100000, 
				PROT_READ|PROT_WRITE, 
				MAP_SHARED, 
				fd, 
				(unsigned int)(0x1fd00000) );
	
	smbaddr = iobaseaddr + SMBADDR; 
	
	for( i = 0; i < 256; i++) {
		value = i2crd( 0xa0, i);
		printf("%x\n", value);
	}
	
	return 0;
	
}	
	
