/*
 *     dio v1.5
 *
 *     Source code for dio: device I/O analysis tool
 *     Email: donaldbmcintosh@yahoo.co.uk
 *     Licence: MIT License
*/

struct blkio_info {
        struct timeval tv;      
        unsigned int major;     /* Read I/O operations */
        unsigned int minor;     /* Read I/O operations */
        char devname[8];        /* Device name */
        unsigned int rd_ios;    /* Read I/O operations */
        unsigned int rd_merges; /* Reads merged */
        unsigned long long rd_sectors; /* Sectors read */
        unsigned int rd_ticks;  /* Time in queue + service for read */
        unsigned int wr_ios;    /* Write I/O operations */
        unsigned int wr_merges; /* Writes merged */
        unsigned long long wr_sectors; /* Sectors written */
        unsigned int wr_ticks;  /* Time in queue + service for write */
        unsigned int ios_in_prog;  /* I/O's in progress */
        unsigned int ticks;     /* Time spent doing I/Os */
        unsigned int wt_ticks;  /* Weighted time spent doing I/Os */
} ;

/* version number */
char	version[4] = "1.5";

/* shared totals */
unsigned long long 	totr;
unsigned long long 	totw;
float	maxbrs;
float	maxbws;
FILE	*ofile;
