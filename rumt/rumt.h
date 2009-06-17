/***************************************************************************
 * Running Unix Memory Test
 * (C) Nicolas George - 2003
 * rumt.h: common header
 ***************************************************************************/

/* md5stream.c */
extern void md5stream_init(void);
extern void md5stream_seed(const unsigned char *, int);
extern void md5stream_fill(unsigned char *, int);
