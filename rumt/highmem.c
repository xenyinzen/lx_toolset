/**************************************************************************
 * highmem device driver
 * (C) Nicolas George - 2003
 * physical memory access, including highmem
 **************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * Compile with:
 *
 * gcc -Wall -O2 -D__KERNEL__ -DMODULE -I/usr/src/linux/include -c highmem.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/capability.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");

static int highmem_open(struct inode *, struct file *);
static ssize_t highmem_read(struct file *, char *, size_t, loff_t *);
static loff_t highmem_lseek(struct file *, loff_t, int);

static struct file_operations fops = {
    open: highmem_open,
    read: highmem_read,
    llseek: highmem_lseek,
};

static struct miscdevice mdev = {
    minor: MISC_DYNAMIC_MINOR,
    name: "highmem",
    fops: &fops,
};

static int
highmem_open(struct inode *inode, struct file *filp)
{
    return(capable(CAP_SYS_RAWIO) ? 0 : -EPERM);
}

static ssize_t
highmem_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    unsigned char *map;
    ssize_t r = 0;
    long long page;
    unsigned in_page, rest;

    while(count > 0) {
	page = (*ppos) >> PAGE_SHIFT;
	if(page >= num_physpages)
	    break;
	in_page = (*ppos) % PAGE_SIZE;
	rest = PAGE_SIZE - in_page > count ? count : PAGE_SIZE - in_page;
	map = kmap(&mem_map[page]);
	if(copy_to_user(buf, map + in_page, rest)) {
	    kunmap(&mem_map[page]);
	    return(-EFAULT);
	}
	kunmap(&mem_map[page]);
	count -= rest;
	r += rest;
	(*ppos) += rest;
	buf += rest;
    }
    return(r);
}

static loff_t
highmem_lseek(struct file *file, loff_t offset, int orig)
{
    switch(orig) {
	case 0:
	    file->f_pos = offset;
	    return(file->f_pos);
	case 1:
	    file->f_pos += offset;
	    return(file->f_pos);
	case 2:
	    file->f_pos += ((loff_t)num_physpages << PAGE_SHIFT) + offset;
	    return(file->f_pos);
	default:
	    return(-EINVAL);
    }
}

int
init_module(void)
{
    int r;

    
    if((r = misc_register(&mdev))) {
	printk(KERN_ERR "highmem failed: %d\n", r);
	return(1);
    }
    printk(KERN_INFO "highmem minor: %d\n", (int)mdev.minor);
    return(0);
}

void
cleanup_module(void)
{
    int r;

    if((r = misc_deregister(&mdev))) {
	printk(KERN_ERR "highmem cleanup failed: %d\n", r);
    }
}
