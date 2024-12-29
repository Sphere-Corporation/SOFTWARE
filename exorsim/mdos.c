/*	MDOS diskette access
 *	Copyright
 *		(C) 2011 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* MDOS disk access */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define SECTOR_SIZE 128
#define CLUSTER_SIZE (SECTOR_SIZE * 4)

/* Specific sectors */

#define SECTOR_ID 0

#define SECTOR_CAT 1	/* Allocation bitmap */
/* Bit 7 of byte 0 is first cluster.  1 means allocated. */

#define SECTOR_LCAT 2	/* Bad block bitmap */

#define SECTOR_DIR 3
#define SECTOR_DIR_SIZE 20
#define SECTOR_BOOT_0 23
#define SECTOR_BOOT_1 24

struct rib {
	unsigned char sdw[114];
	unsigned char blank[3];
	unsigned char last_size;	/* 117 */
	unsigned char size_high;	/* 118 */
	unsigned char size_low;		/* 119 */
	unsigned char addr_high;	/* 120 */
	unsigned char addr_low;		/* 121 */
	unsigned char pc_high;		/* 122 */
	unsigned char pc_low;		/* 123 */
	unsigned char blank_1[4];
};

struct dirent {
	unsigned char name[8];
	unsigned char suffix[2];
	unsigned char sector_high;
	unsigned char sector_low;
	unsigned char attr_high;
	unsigned char attr_low;
	unsigned char blank[2];
};

FILE *disk;

void getsect(unsigned char *buf, int sect)
{
        fseek(disk, sect * SECTOR_SIZE, SEEK_SET);
        fread((char *)buf, SECTOR_SIZE, 1, disk);
}

void putsect(unsigned char *buf, int sect)
{
        fseek(disk, sect * SECTOR_SIZE, SEEK_SET);
        fwrite((char *)buf, SECTOR_SIZE, 1, disk);
}

int lower(int c)
{
        if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
        else
                return c;
}

struct name
{
        char *name;

        /* From RIB */
        int last_size;
        int size;
        int pc;
        int load;

        /* Calculated from RIB */
        int sects;

        int system;
        int write_protect;
        int delete_protect;
        int compr;
        int cont;
        int type;
        int is_cm; /* If suffix is .CM */
        int sector; /* Sector no. of RIB */
};

struct name *names[(SECTOR_DIR_SIZE * SECTOR_SIZE) / sizeof(struct dirent)];
int name_n;

int comp(struct name **l, struct name **r)
{
        return strcmp((*l)->name, (*r)->name);
}

int calc_sects(struct rib *r)
{
        int total = 0; /* Count rib */
        int x;
        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        /* printf("Total = %d\n", total * 4);
                        printf("Logical last = %d\n", (sdw & 0x7FFF)); */
                        return (sdw & 0x7FFF) + 1; /* Size in sectors */
                        break;
                }
                total += ((sdw >> 10) & 0x1F) + 1;
        }
        return total * 4; /* clusters to sectors */
}

void dump_rib(struct rib *r)
{
        int x;
        printf("Dumping rib:\n");
        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        printf("End mark found: last logical sector is %d\n", (sdw & 0x7FFF));
                        return;
                } else {
                        int clust = (sdw & 0x3FF);
                        //int sect = clust * 4;
                        int siz = ((sdw >> 10) & 0x1F) + 1;
                        //int len = (((sdw >> 10) & 0x1F) + 1) * 4;
                        printf("Segment start=%d, size=%d\n", clust, siz);
                }
        }
}

int no_convert;
int force_convert;

void read_file(int sector, int type, FILE *f)
{
        unsigned char rib_buf[SECTOR_SIZE];
        struct rib *r = (struct rib *)rib_buf;
        int x;
        int secno = 0;
        unsigned char last_size;
        unsigned short size;
        unsigned short ribsize;

        getsect(rib_buf, sector);

        ribsize = (r->size_high << 8) + r->size_low; /* Only valid for loadable type == 2 files */
        size = calc_sects(r); /* Should be valid for all file types.. */
        last_size = r->last_size;

        /* printf("Size is %d sectors, %d bytes in last, total=%d bytes\n", size, last_size,
               (size - 1) * SECTOR_SIZE + last_size); */

        if (type == 2)
                fprintf(stderr,"Load=$%4.4x, Start=$%4.4x\n",(r->addr_high << 8) + r->addr_low,
                        (r->pc_high << 8) + r->pc_low);

        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        return;
                } else {
                        int clust = (sdw & 0x3FF);
                        int sect = clust * 4;
                        int len = (((sdw >> 10) & 0x1F) + 1) * 4;
                        int x;
                        /* printf("Clust = %d, Sect=%d, len=%d\n", clust, sect, len); */
                        for (x = 0; x != len; ++x) {
                                unsigned char buf[SECTOR_SIZE];
                                int n;
                                if (sect + x != sector) { /* Not RIB sector */
                                        /* printf("Sector %d:\n", sect + x); */
                                        getsect(buf, sect + x);
                                        if ((type == 5 && !no_convert) || force_convert) {
                                                int ends = SECTOR_SIZE;
                                                if (secno + 1 == size) {
                                                        /* Last sector of file, truncate trailing NULs */
                                                        while (ends && buf[ends-1] == 0) --ends;
                                                }
                                                /* Convert line end of ASCII file to MS-DOS unless no_convert is set */
                                                for (n = 0; n != ends; ++n) {
                                                        int c = buf[n];
                                                        if (c & 0x80) { // Expand compressed spaces
                                                                int z;
                                                                for (z = 0; z != (c & 0x7F); ++z)
                                                                        fputc(' ', f);
                                                        } else if (c == 13) {
                                                                /* Convert to MS-DOS */
                                                                fputc('\r', f);
                                                                fputc('\n', f);
                                                        } else if (c == 10) {
                                                                /* Delete Line Feeds */
                                                        } else if (c == 0) {
                                                                /* Delete NULs */
                                                        } else {
                                                                fputc(c, f);
                                                        }
                                                }
                                                /* printf("EOS\n"); */
                                        } else if (type == 2) {
                                                /* Binary image file.  Exact size is in RIB */
                                                if (secno + 1 == ribsize) {
                                                        /* printf("Sector %d, size=%d\n", secno, last_size); */
                                                        /* Last sector */
                                                        fwrite(buf, last_size, 1, f);
                                                        return;
                                                } else {
                                                        /* printf("Sector %d, size=128\n", secno); */
                                                        fwrite(buf, SECTOR_SIZE, 1, f);
                                                }
                                        } else {
                                                /* Unknown file.  Send all sectors out as-is. */
                                                fwrite(buf, SECTOR_SIZE, 1, f);
                                        }
                                        secno++;
                                        if (secno == size)
                                                return;
                                }
                        }
                }
        }
}

/* Read first byte of file - to guess if it's an object file */

int read_first(int sector)
{
        unsigned char rib_buf[SECTOR_SIZE];
        struct rib *r = (struct rib *)rib_buf;
        int x;

        getsect(rib_buf, sector);

        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        return -1;
                } else {
                        int clust = (sdw & 0x3FF);
                        int sect = clust * 4;
                        int len = (((sdw >> 10) & 0x1F) + 1) * 4;
                        int y;
                        for (y = 0; y != len; ++y) {
                                unsigned char buf[SECTOR_SIZE];
                                if (sect + y != sector) { /* Not RIB sector */
                                        getsect(buf, sect + y);
                                        return buf[0];
                                }
                        }
                }
        }
        return -1;
}

/* Count free clusters */

int amount_free(unsigned char *cat)
{
        int total = 0;
        int x;
        for (x = 0; x != (SECTOR_SIZE * 8); ++x) {
                if (!(cat[x >> 3] & (1 << (7 - (x & 7)))))
                        ++total;
        }
        return total;
}

/* Free command */

int do_free(void)
{
        int amount;
        unsigned char cat[SECTOR_SIZE];
        getsect(cat, SECTOR_CAT);
        amount = amount_free(cat);
        printf("%d free sectors, %d free bytes\n", amount * 4, amount * 512);
        return 0;
}

/* Mark a segment as clear or free */

void mark_space(unsigned char *cat, int start, int size, int alloc)
{
        int x;
        for (x = start; x != start + size; ++x) {
                if (alloc)
                        cat[x >> 3] |= (1 << (7 - (x & 7)));
                else
                        cat[x >> 3] &= ~(1 << (7 - (x & 7)));
        }
}

/* Allocate n clusters.  Returns with amount actually allocated and starting cluster in *start */

int find_space(unsigned char *cat, int n, int *start)
{
        int x;
        int bestfirst = -1;
        int bestsize = 0;
        int curfirst = -1;
        int cursize;
        printf("Find space %d\n", n);
        /* Find first contiguous block which fits requested size */
        /* Note longest contiguous block along the way */
        for (x = 0; x != SECTOR_SIZE * 8; ++x) {
                if ((1 << (7 - (x & 7)) & cat[x >> 3])) {
                        /* This cluster is allocated */
                        if (curfirst != -1) {
                                /* We're in a segment, note it */
                                printf("Found segment: start=%d, size=%d\n", curfirst, cursize);
                                if (cursize > bestsize) {
                                        bestsize = cursize;
                                        bestfirst = curfirst;
                                }
                                curfirst = -1;
                                cursize = 0;
                        } else {
                                /* Not in segment, do nothing */
                        }
                } else {
                        /* This cluster is free */
                        if (curfirst != -1) {
                                /* We're in a segment: extend it */
                                ++cursize;
                                if (cursize == n || cursize == 32) goto done;
                        } else {
                                /* We're not in a segment, start one */
                                curfirst = x;
                                cursize = 1;
                                if (cursize == n) goto done;
                        }
                }
        }
        done:
        /* In a segment? Finalize it. */
        if (curfirst != -1) {
                printf("Found segment: start=%d, size=%d\n", curfirst, cursize);
                if (cursize > bestsize) {
                        bestfirst = curfirst;
                        bestsize = cursize;
                }
        }
        *start = bestfirst;
        return bestsize;
}

/* Allocate space, build RIB */

int alloc_space(unsigned char *cat, struct rib *r, int sects)
{
        int req = (sects + 3) / 4; /* Number of clusters to allocate */
        int seg = 0; /* Segment pointer in RIB */
        int sdw;
        /* printf("Amount avail = %d clusters.\n", amount_free(cat)); */
        while (req) {
                int start = -1;
                int found;
                if (seg == 114) {
                        printf("Too many segments\n");
                        return -1;
                }
                found = find_space(cat, req, &start);
                /* printf("Segment=%d Size=%d\n", start, found); */
                if (found) {
                        mark_space(cat, start, found, 1);

                        /* Add segment to rib */
                        sdw = start + ((found - 1) << 10);
                        r->sdw[seg] = (sdw >> 8);
                        r->sdw[seg + 1] = sdw;
                        seg += 2;
                }

                req -= found;
                if (!req) {
                        /* printf("Done\n"); */
                } else if (!found) {
                        printf("Not enough space\n");
                        return -1;
                }
        }
        if (seg == 114) {
                printf("Too many segments\n");
                return -1;
        }
        /* Add end mark */
        sdw = 0x8000 + sects - 2; /* Last sector does not count rib */
        r->sdw[seg] = (sdw >> 8);
        r->sdw[seg + 1] = sdw;
        /* Need to write back cat here */
        /* Success */
        return 0;
}

/* Convert logical sector number to physical sector number */

int lsn_to_psn(struct rib *r, int lsn)
{
        int x;
        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        printf("End mark found?  Internal error.\n");
                        exit(-1);
                } else {
                        int clust = (sdw & 0x3FF);
                        int sect = clust * 4;
                        int siz = ((sdw >> 10) & 0x1F) + 1;
                        int len = siz * 4;
                        if (lsn < len) {
                                return sect + lsn;
                        } else {
                                lsn -= len;
                        }
                }
        }
        printf("End of SDW?  Internal error.\n");
        exit(-1);
}

/* Write a file */

int write_file(unsigned char *cat, unsigned char *buf, int sects)
{
        int x;
        int rib_sect;
        struct rib r[1];

        memset(r, 0, SECTOR_SIZE);
        if (alloc_space(cat, r, sects + 1)) {
                return -1;
        }

        putsect((unsigned char *)r, rib_sect = lsn_to_psn(r, 0));
        for (x = 0; x != sects; ++x) {
                printf("File to sector %x\n", lsn_to_psn(r, x + 1));
                putsect(buf + SECTOR_SIZE * x, lsn_to_psn(r, x + 1));
        }

        printf("Rib sector = %x\n", rib_sect);

        return rib_sect;
}

/* File name hash function */

int hash(unsigned char *str)
{
	int x;
	unsigned b;
	unsigned a;
	b = 0;
	for (x = 0; x != 10; ++x) {
		a = *str++;
		if (a >= 0x25)
			a -= 0x25;
		else
			a = 0;
		b = (b & 0xFF) + a + (b >> 8);
		b = (((b << 1) + (b >> 8)) & 0x1FF);
	};
	b = (((b >> 1) + (b << 8)) & 0x1FF);
	a = b;
	a = (((a >> 1) + (a << 8)) & 0x1FF);
	a = (((a >> 1) + (a << 8)) & 0x1FF);
	a = (((a >> 1) + (a << 8)) & 0x1FF);
	a = (((a >> 1) + (a << 8)) & 0x1FF);
	a = (a & 0xFF) + (b & 0xFF);
	b = a;
	b &= 0x1f;
	if (b >= 20) {
		b -= 20;
		if (b < 10) {
			b = (b << 1) + (a & 1);
		}
	}
	return b;
}

/* Write directory entry */

int write_dir(char *name, int rib_sect, int attr)
{
        struct dirent d[1];
        int x;
        int hsh;
        int count;
        unsigned char dir_buf[SECTOR_SIZE];
        /* Copy file name into directory entry */
        x = 0;
        while (*name && *name != '.' && x < 8) {
                if (*name >= 'a' && *name <= 'z')
                        d->name[x++] = *name++ - 'a' + 'A';
                else
                        d->name[x++] = *name++;
        }
        while (x < 8) {
                d->name[x++] = ' ';
        }
        x = 0;
        while (*name && *name != '.')
                ++name;
        if (*name == '.') {
                ++name;
                while (*name && x < 2) {
                        if (*name >= 'a' && *name <= 'z')
                                d->suffix[x++] = *name++ - 'a' + 'A';
                        else
                                d->suffix[x++] = *name++;
                }
        } else {
                d->name[x++] = 'S';
                d->name[x++] = 'A';
        }
        while (x < 2) {
                d->suffix[x++] = ' ';
        }
        d->sector_high = (rib_sect >> 8);
        d->sector_low = rib_sect;
        d->attr_high = (attr >> 8);
        d->attr_low = attr;
        d->blank[0] = 0;
        d->blank[1] = 0;
        
        /* Compute hash of directory entry */
        hsh = hash(d->name);
        /* printf("hash=%d\n", hsh); */


        for (count = 0; count != 20; ++count) {
                int y;
                getsect(dir_buf, hsh + SECTOR_DIR);
                printf("Trying directory sector %d\n", hsh + SECTOR_DIR);
                for (y = 0; y != SECTOR_SIZE; y += 16) {
                        if (dir_buf[y] == 0x00 || dir_buf[y] == 0xFF) {
                                printf("OK, using entry %d\n", y);
                                memcpy(dir_buf + y, (char *)d, sizeof(struct dirent));
                                putsect(dir_buf, hsh + SECTOR_DIR);
                                return 0;
                        }
                }
                if (++hsh == SECTOR_DIR_SIZE)
                        hsh = 0;
        }

        printf("Out of directory entries\n");
        return -1;
}

/* Delete file */

int del_file(int rib_sector)
{
        int x;
        unsigned char rib[SECTOR_SIZE];
        struct rib *r = (struct rib *)rib;
        unsigned char cat[SECTOR_SIZE];
        getsect(rib, rib_sector);
        getsect(cat, SECTOR_CAT);
        for (x = 0; x != sizeof(r->sdw); x += 2) {
                int sdw = ((r->sdw[x] << 8) + r->sdw[x + 1]);
                if (sdw & 0x8000) {
                        break;
                } else {
                        int clust = (sdw & 0x3FF);
                        int siz = ((sdw >> 10) & 0x1F) + 1;
                        mark_space(cat, clust, siz, 0);
                }
        }
        putsect(cat, SECTOR_CAT);
        return 0;
}

/* Find a file, return sector number of its rib */

int find_file(char *filename, int *type, int del)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y != SECTOR_SIZE; y += sizeof(struct dirent)) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (d->name[0] && d->name[0] != 0xFF) {
                                //struct rib *rib = (struct rib *)rib_buf;
                                char s[50];
                                int p = 0;
                                int i;
                                for (i = 0; i != sizeof(d->name); i++) {
                                        s[p++] = lower(d->name[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p++] = '.';
                                for (i = 0; i != sizeof(d->suffix); i++) {
                                        s[p++] = lower(d->suffix[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p] = 0;
                                if (!jstricmp(s, filename)) {
                                        if (type)
                                                *type = (d->attr_high & 7);
                                        if (del) {
                                                d->name[0] = 0xFF;
                                                d->name[1] = 0xFF;
                                                putsect(buf, x);
                                        }
                                        return (d->sector_high << 8) + d->sector_low;
                                }
                        }
                }
        }
        return -1;
}

/* Delete file name */

int rm(char *name)
{
        int type;
        int rib = find_file(name, &type, 1);
        if (rib != -1) {
                if (del_file(rib)) {
                        printf("Error deleting file '%s'\n", name);
                        return -1;
                } else {
                }	return 0;
        } else {
                printf("File '%s' not found\n", name);
                return -1;
        }
}

/* Put a file on the disk */

int put_file(char *local_name, char *mdos_name)
{
        FILE *f = fopen(local_name, "r");
        long size;
        long up;
        long x;
        int c;
        unsigned char *buf;
        unsigned char cat[SECTOR_SIZE];
        int rib_sect;
        if (!f) {
                printf("Couldn't open '%s'\n", local_name);
                return -1;
        }
        if (fseek(f, 0, SEEK_END)) {
                printf("Couldn't get file size of '%s'\n", local_name);
                fclose(f);
                return -1;
        }
        size = ftell(f);
        if (size < 0)  {
                printf("Couldn't get file size of '%s'\n", local_name);
                fclose(f);
                return -1;
        }
        rewind(f);
        up = ((size + SECTOR_SIZE - 1) & ~(long)(SECTOR_SIZE - 1));
        buf = (unsigned char *)malloc(up);


        /* Read file, convert UNIX or MS-DOS ASCII to MDOS ASCII */
        x = 0;
        while ((c = fgetc(f)) != -1)
        {
                if (c == '\n') {
                        /* MDOS line terminator is CR */
                        buf[x++] = '\r';
                } else if (c == '\r') {
                        /* Ignore carriage returns */
                } else {
                        buf[x++] = c;
                }
        }
        size = x;
        fclose(f);
        /* Round up to sector size again */
        up = ((size + SECTOR_SIZE - 1) & ~(long)(SECTOR_SIZE - 1));

        /* Fill with NULs to end of sector */
        while (x != up)
                buf[x++] = 0;

        /* Delete existing file */
        rm(mdos_name);

        /* Get cat... */
        getsect(cat, SECTOR_CAT);

        /* Allocate space and write file */
        rib_sect = write_file(cat, buf, up / SECTOR_SIZE);

        if (rib_sect == -1) {
                printf("Couldn't write file\n");
                return -1;
        }

        if (write_dir(mdos_name, rib_sect, 0x0500)) {
                printf("Couldn't write directory entry\n");
                return -1;
        }

        /* Success! */
        putsect(cat, SECTOR_CAT);
        return 0;
}

/* Directory listing */

char *typestr[]=
{
        "0 User defined",
        "1 Unknown",
        "2 Image",
        "3 Object",
        "4 Unknown",
        "5 ASCII",
        "6 Unknown",
        "7 ASCII coverted"
};

int fixcopy;

void mdos_load_dir(int all, int only_ascii)
{
        unsigned char buf[SECTOR_SIZE];
        unsigned char rib_buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y != SECTOR_SIZE; y += sizeof(struct dirent)) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (d->name[0] && d->name[0] != 0xFF) {
                                struct name *nam;
                                struct rib *rib = (struct rib *)rib_buf;
                                char s[50];
                                int p = 0;
                                int i;
                                for (i = 0; i != sizeof(d->name); i++) {
                                        s[p++] = lower(d->name[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p++] = '.';
                                for (i = 0; i != sizeof(d->suffix); i++) {
                                        s[p++] = lower(d->suffix[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p] = 0;
                                nam = (struct name *)malloc(sizeof(struct name));
                                nam->name = strdup(s);
                                nam->type = (d->attr_high & 7);
                                if (d->suffix[0] == 'C' && d->suffix[1] == 'M')
                                        nam->is_cm = 1;
                                else
                                        nam->is_cm = 0;
                                if (d->attr_high & 0x80)
                                        nam->write_protect = 1;
                                else
                                        nam->write_protect = 0;
                                if (d->attr_high & 0x40)
                                        nam->delete_protect = 1;
                                else
                                        nam->delete_protect = 0;
                                if (d->attr_high & 0x20)
                                        nam->system = 1;
                                else
                                        nam->system = 0;
                                if (d->attr_high & 0x10)
                                        nam->cont = 1;
                                else
                                        nam->cont = 0;
                                if (d->attr_high & 0x08)
                                        nam->compr = 1;
                                else
                                        nam->compr = 0;

                                nam->sector = (d->sector_high << 8) + d->sector_low;

                                /* Get RIB */
                                getsect(rib_buf, nam->sector);

                                /* Only valid for loadable files */
                                nam->size = (rib->size_high << 8) + rib->size_low;
                                nam->pc = (rib->pc_high << 8) + rib->pc_low;
                                nam->load = (rib->addr_high << 8) + rib->addr_low;

                                nam->last_size = rib->last_size;
                                nam->sects = calc_sects(rib); /* Size in sectors */

#if 0
                                printf("\nName=%s\n", nam->name);
                                printf("Size in sectors=%d, %d bytes\n", nam->sects, nam->sects * SECTOR_SIZE);
                                printf("load size=%d sectors, %d bytes\n", nam->size, (nam->size - 1) * SECTOR_SIZE + nam->last_size);
                                printf("Initial pc=%x\n", nam->pc);
                                printf("Load addr=%x\n", nam->load);
                                printf("Last_size=%d\n", nam->last_size);
#endif

                                if ((all || !nam->system) && (!only_ascii || nam->type == 5))
                                        names[name_n++] = nam;

                                if (fixcopy && d->suffix[0] == 'E' && d->suffix[1] == 'D' && read_first(nam->sector) != 'D')
                                {
                                        /* Set file type to 5 */
                                        d->attr_high = ((d->attr_high & ~7) | 5);
                                        printf("  Changed %s to ASCII\n", nam->name);
                                        putsect(buf, x);
                                }
                        }
                }
        }
        qsort(names, name_n, sizeof(struct name *), (int (*)(const void *, const void *))comp);
}

void mdos_dir(int all, int full, int single, int only_ascii)
{
        int x, y;
        int rows;
        int cols = (80 / 13);

        mdos_load_dir(all, only_ascii);

        if (full) {
                int totals = 0;
                printf("\n");
                for (x = 0; x != name_n; ++x) {
                        if (names[x]->type == 2) {
                                printf("-r%c%c%c%c%c%c %6d %-13s (%s) Load=$%4.4x, Jump=$%4.4x\n",
                                       (names[x]->write_protect ? '-' : 'w'),
                                       (names[x]->is_cm ? 'x' : '-'),
                                       (names[x]->system ? 's' : '-'),
                                       (names[x]->delete_protect ? 'd' : '-'),
                                       (names[x]->cont ? 'c' : '-'),
                                       (names[x]->compr ? 'z' : '-'),
                                       (names[x]->size - 1) * SECTOR_SIZE + names[x]->last_size, names[x]->name,
                                       typestr[names[x]->type], names[x]->load, names[x]->pc);
                                totals += names[x]->sects;
                        } else {
                                printf("-r%c%c%c%c%c%c %6d %-13s (%s)\n",
                                       (names[x]->write_protect ? '-' : 'w'),
                                       (names[x]->is_cm ? 'x' : '-'),
                                       (names[x]->system ? 's' : '-'),
                                       (names[x]->delete_protect ? 'd' : '-'),
                                       (names[x]->cont ? 'c' : '-'),
                                       (names[x]->compr ? 'z' : '-'),
                                       names[x]->sects * SECTOR_SIZE, names[x]->name,
                                       typestr[names[x]->type]);
                                totals += names[x]->sects;
                        }
                }
                printf("\n%d entries\n", name_n);
                printf("\n%d sectors, %d bytes\n", totals, totals * SECTOR_SIZE);
                printf("\n");
                do_free();
                printf("\n");
        } else if (single) {
                int x;
                for (x = 0; x != name_n; ++x) {
                        printf("%s\n", names[x]->name);
                }
        } else {

                /* Rows of 12 names each ordered like ls */

                rows = (name_n + cols - 1) / cols;

                for (y = 0; y != rows; ++y) {
                        for (x = 0; x != cols; ++x) {
                                int n = y + x * rows;
                                /* printf("%11d  ", n); */
                                if (n < name_n)
                                        printf("%-11s  ", names[n]->name);
                                else
                                        printf("             ");
                        }
                        printf("\n");
                }
        }
}

/* cat a file */

void cat(char *name)
{
        int type;
        int sector = find_file(name, &type, 0);
        if (sector == -1) {
                printf("File '%s' not found\n", name);
                exit(-1);
        } else {
                /* printf("Found file.  Sector of rib is %d\n", sector); */
                read_file(sector, type, stdout);
        }
}

/* get a file from the disk */

int get_file(char *mdos_name, char *local_name)
{
        int type;
        int sector = find_file(mdos_name, &type, 0);
        if (sector == -1) {
                printf("File '%s' not found\n", mdos_name);
                return -1;
        } else {
                FILE *f = fopen(local_name, "w");
                if (!f) {
                        printf("Couldn't open local file '%s'\n", local_name);
                        return -1;
                }
                /* printf("Found file.  Sector of rib is %d\n", sector); */
                read_file(sector, type, f);
                if (fclose(f)) {
                        printf("Couldn't close local file '%s'\n", local_name);
                        return -1;
                }
                return 0;
        }
}

int extract_name(char *mdos, char *local)
{
        char *start = 0;
        int x;
        /* Find last part of path */
        for (x = 0; local[x]; ++x)
                if (local[x] == '/')
                        start = local + x;
        /* Point to start of name */
        if (start)
                ++start;
        else
                start = local;
        x = 0;
        /* Copy name */
        if (!((start[x] >= 'a' && start[x] <= 'z') ||
            (start[x] >= 'A' && start[x] <= 'Z')))
        {
                printf("MDOS name must start with letter\n");
                return -1;
        }
        for (; start[x] && start[x] != '.' && x != 8; ++x)
        {
                if (!((start[x] >= 'a' && start[x] <= 'z') ||
                      (start[x] >= 'A' && start[x] <= 'Z') ||
                      (start[x] >= '0' && start[x] <= '9')))
                {
                        printf("MDOS name must be made of letter or numbers\n");
                        return -1;
                }
                mdos[x] = start[x];
        }


        if (x == 8 && start[x] != '.') {
                printf("MDOS name is too long\n");
                return -1;
        }
        if (x == 0) {
                printf("MDOS name is too short\n");
                return -1;
        }
        if (start[x] == '.') {
                int count;
                /* Copy the . */
                mdos[x] = start[x];
                ++x;
                if (!((start[x] >= 'a' && start[x] <= 'z') ||
                    (start[x] >= 'A' && start[x] <= 'Z')))
                {
                        printf("MDOS extension must start with letter\n");
                        return -1;
                }
                /* Copy the extension */
                for (count = 0; start[x] && count != 2; ++count, ++x)
                        mdos[x] = start[x];
                if (count == 2 && start[x]) {
                        if (!((start[x] >= 'a' && start[x] <= 'z') ||
                              (start[x] >= 'A' && start[x] <= 'Z') ||
                              (start[x] >= '0' && start[x] <= '9')))
                        {
                                printf("MDOS name must be made of letter or numbers\n");
                                return -1;
                        }
                        printf("MDOS extension is too long\n");
                        return -1;
                }
                if (count == 0) {
                        printf("MDOS extension is too short\n");
                        return -1;
                }
                mdos[x] = 0;
        } else {
                /* No extension */
                mdos[x] = 0;
                printf("MDOS extension is missing\n");
                return -1;
        }
        printf("MDOS name is %s\n", mdos);
        return 0;
}

int main(int argc, char *argv[])
{
        int all = 0;
        int full = 0;
        int single = 0;
        int only_ascii = 0;
	int x;
	char *disk_name;
	x = 1;
	if (x == argc || !strcmp(argv[x], "--help") || !strcmp(argv[x], "-h"))
	{
                help:
                printf("\nEXORciser MDOS diskette access\n");
                printf("\n");
                printf("Syntax: mdos path-to-diskette command args\n");
                printf("\n");
                printf("  Commands:\n");
                printf("      ls [-la1A]                    Directory listing\n");
                printf("                  -l for long\n");
                printf("                  -a to show system files\n");
                printf("                  -1 to show a single name per line\n");
                printf("                  -A show only ASCII files\n");
                printf("                  -F fix .ED (EDOS import) files as side effect\n");
                printf("      cat mdos-name                 Type file to console\n");
                printf("      rawcat mdos-name              Type file to console, no ASCII conversion\n");
                printf("      acat mdos-name                Type file to console, force ASCII conv\n");
                printf("      get mdos-name [local-name]    Copy file from diskette to local-name\n");
                printf("      rawget mdos-name [local-name] Copy file from disk, no ASCII conversion\n");
                printf("      aget mdos-name [local-name]   Copy file from disk, force ASCII conv\n");
                printf("      put local-name [mdos-name]    Copy file to diskette to mdos-name\n");
                printf("      free                          Print amount of free space\n");
                printf("      rm mdos-name                  Delete a file\n");
                printf("      x [-aA]                       Extract all files into current directory\n");
                printf("                  -a also extract system files\n");
                printf("                  -A extract only ASCII files\n");
                printf("\n");
                return -1;
	}
	disk_name = argv[x++];
	disk = fopen(disk_name, "r+");
	if (!disk) {
	        printf("Couldn't open '%s'\n", disk_name);
	        return -1;
	}

	if (x == argc || argv[x][0] == '-') {
	        /* Just print a directory listing */
	        goto dir;
        } else if (!strcmp(argv[x], "help")) {
                goto help;
        } else if (!strcmp(argv[x], "ls")) {
                ++x;
                dir:
                /* Directory options */
                while (x != argc && argv[x][0] == '-') {
                        int y;
                        for (y = 1;argv[x][y];++y) {
                                int opt = argv[x][y];
                                switch (opt) {
                                        case 'l': full = 1; break;
                                        case 'a': all = 1; break;
                                        case '1': single = 1; break;
                                        case 'A': only_ascii = 1; break;
                                        case 'F': fixcopy = 1; break;
                                        default: printf("Unknown option '%c'\n", opt); return -1;
                                }
                        }
                        ++x;
                }
	        mdos_dir(all, full, single, only_ascii);
	        return 0;
        } else if (!strcmp(argv[x], "free")) {
                return do_free();
	} else if (!strcmp(argv[x], "cat") || !strcmp(argv[x], "rawcat") || !strcmp(argv[x], "acat")) {
	        if (!strcmp(argv[x], "rawcat"))
	                no_convert = 1;
                else if (!strcmp(argv[x], "acat"))
                        force_convert = 1;
	        ++x;
	        if (x == argc) {
	                printf("Missing file name to cat\n");
	                return -1;
	        } else {
	                cat(argv[x++]);
	                return 0;
	        }
	} else if (!strcmp(argv[x], "get") || !strcmp(argv[x], "rawget") || !strcmp(argv[x], "aget")) {
                char *local_name;
                char *mdos_name;
                if (!strcmp(argv[x], "rawget"))
                        no_convert = 1;
                else if (!strcmp(argv[x], "aget"))
                        force_convert = 1;
                ++x;
                if (x == argc) {
                        printf("Missing file name to get\n");
                        return -1;
                }
                mdos_name = argv[x];
                local_name = mdos_name;
                if (x + 1 != argc)
                        local_name = argv[++x];
                return get_file(mdos_name, local_name);
        } else if (!strcmp(argv[x], "put")) {
                char mdos_name[100];
                char *local_name;
                ++x;
                if (x == argc) {
                        printf("Missing file name to put\n");
                        return -1;
                }
                local_name = argv[x];
                if (x + 1 != argc) {
                        if (extract_name(mdos_name, argv[++x]))
                                return -1;
                } else {
                        if (extract_name(mdos_name, local_name))
                                return -1;
                }
                return put_file(local_name, mdos_name);
        } else if (!strcmp(argv[x], "rm")) {
                char *name;
                ++x;
                if (x == argc) {
                        printf("Missing name to delete\n");
                        return -1;
                } else {
                        name = argv[x];
                }
                return rm(name);
        } else if (!strcmp(argv[x], "x")) {
                int sta = 0;
                ++x;
                while (x != argc && argv[x][0] == '-') {
                        int y;
                        for (y = 1;argv[x][y];++y) {
                                int opt = argv[x][y];
                                switch (opt) {
                                        case 'a': all = 1; break;
                                        case 'A': only_ascii = 1; break;
                                        default: printf("Unknown option '%c'\n", opt); return -1;
                                }
                        }
                        ++x;
                }
                mdos_load_dir(all, only_ascii);
                for (x = 0; x != name_n; ++x)
                {
                        printf("Extracting %s\n", names[x]->name);
                        if (get_file(names[x]->name, names[x]->name))
                        {
                                sta = -1;
                                printf("  failed.\n");
                        }
                }
                return sta;
	} else {
	        printf("Unknown command '%s'\n", argv[x]);
	        return -1;
	}
	return 0;
}
