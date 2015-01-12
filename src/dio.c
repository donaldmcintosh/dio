/*
    dio v1.5

    Source code for dio: device I/O analysis tool
    Email: donaldbmcintosh@yahoo.co.uk
    Licence: MIT License
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/time.h>
#include "dio.h"

#define WIDTH 80
#define HDHEIGHT 4
#define TOTHEIGHT 3

#define SCRLHEIGHT 17
int scrlheight = 17;
#define	HEIGHT 24
int height = 24;

void initwindow(WINDOW *win);
void setheadwin(WINDOW *win, char* dev);
void updatescreen(char* dev, WINDOW*, WINDOW*, int ival);
void getdiskstats(char* dev, struct blkio_info* new);
void calcstats(char* line, struct blkio_info new, struct blkio_info old, int ival);
void printtots(WINDOW *win);
void usage();
void listio();
void bomb(int exitv, char *msg, int showusage);
void close_end(int signum);
void check_input();

int main(int argv, char** argc)
{
	char 	*dev;
	char 	*intervalc;
	int		interval;
	int		maxy, maxx;
	int		pad = 0; /*arg padding*/
	char	msg[200];
	struct sigaction quitaction;

	if(argv == 2){
		if(argc[1][0] == '-'){
			if(argc[1][1] == 'l'){
				listio();
			}
			else{
				bomb(1, "", 1);
			}
		}
	}

	if (argv == 5){
		pad = 2;

		if(argc[1][0] == '-'){
			if(argc[1][1] == 'o'){
				ofile = fopen(argc[2], "w");
				if(ofile == NULL){
					sprintf(msg,"Cannot open %s.\n",argc[2]);
					bomb(1, msg, 0);
				}
				fprintf(ofile, "# dio v%s data output file\n",version);
			}
			else{
				sprintf(msg,"Argument %c is not valid.\n",argc[1][1]);
				bomb(1, msg, 1);
			}
		}
	}
	else if( argv != 3 ){
		bomb(1, "", 1);
	}
	
	dev = argc[pad +1];
	if(dev == NULL){
		sprintf(msg, "You must specify a device name.\n");
		bomb(1, msg, 1);
	}
	intervalc = argc[pad +2];
	if(intervalc == NULL){
		sprintf(msg, "You must specify a time interval.\n");
		bomb(1, msg, 1);
	}
	interval = atoi(intervalc);
	if(interval < 1){
 		sprintf(msg, "You must specify a time interval greater than or equal to 1.\n");
		bomb(1, msg, 0);
	}
	
	/* confirm dimensions of window */
	WINDOW		*mainwin = initscr();
	getmaxyx(mainwin, maxy, maxx);
	if(maxy < height){
		sprintf(msg, "Please extend the height of your terminal window.\nIt must be 24 characters in height to run dio.\n");
		bomb(1, msg, 0);
	}
    else {
        height = maxy;
        scrlheight = height - HDHEIGHT - TOTHEIGHT;
    }
	if(maxx < WIDTH){
		sprintf(msg, "Please extend the width of your terminal window.\nIt must be 80 characters wide to run dio.\n");
		bomb(1, msg, 0);
	}

	/* dont echo and don't block characters */
	noecho();
	cbreak();

	/* setup signal handlers */
	quitaction.sa_handler = close_end;
	sigemptyset(&quitaction.sa_mask);
	quitaction.sa_flags = 0;
	
	/* run close_end on receipt of SIGINT */
	sigaction(SIGINT, &quitaction, NULL);
	sigaction(SIGTERM, &quitaction, NULL);
	sigaction(SIGHUP, &quitaction, NULL);

	/* initialize all three windows */
	WINDOW		*headwin = newwin(HDHEIGHT,WIDTH,0,0);
	WINDOW		*scrollwin = newwin(scrlheight,WIDTH,HDHEIGHT,0);
	WINDOW		*totwin = newwin(TOTHEIGHT,WIDTH,scrlheight+HDHEIGHT,0);

	/* initialize all window*/
	initwindow(mainwin);
	initwindow(headwin);
	initwindow(scrollwin);
	initwindow(totwin);

	/* set scrolling active on scrollable window*/
	scrollok(scrollwin, TRUE);

	/* write the heading window */
	setheadwin(headwin, dev);

	updatescreen(dev, scrollwin, totwin, interval);

	endwin();
	if(ofile != NULL){
		fclose(ofile);
	}
	exit(0);
}

void opendiskstats() {
    iofp = fopen("/proc/diskstats", "r");
}

void initwindow(WINDOW* win)
{
	nodelay(win, TRUE);
	wclear(win);
	wrefresh(win);
}
	
void setheadwin(WINDOW *win, char* dev)
{
	char colhead[WIDTH+1];
	char header[WIDTH+1];
	time_t 	now;	/* time now */

	sprintf(header,"Device name: %-8s                                                  dio v%s", dev, version);
	waddstr(win, header);
	wmove(win, 1,0);
	sprintf(colhead,"                                                                       ");
	waddstr(win, colhead);
	wmove(win, 2,0);
	sprintf(colhead,"    br    |    bw    |  rkb/s |  wkb/s | %%r:%%w | ro | wo | io |  rt   |  wt   |");
	waddstr(win, colhead);
	wrefresh(win);

	if(ofile != NULL){
		fprintf(ofile,"# Device name: %-8s\n", dev);

		now = time(NULL);
		fprintf(ofile,"# Started: %s\n", ctime(&now));
		fprintf(ofile,"    br    ,    bw    ,  rkb/s ,  wkb/s , %%r,%%w , ro , wo ,  rq ,rq%%b,  wq ,wq%%b\n");
	}

}

void updatescreen(char* dev, WINDOW *win, WINDOW* tot, int ival)
{
	struct blkio_info	bio;
	struct blkio_info	bioold;
	char   line[WIDTH];
	int	   i;

        opendiskstats();
        getdiskstats(dev, &bio);
	sleep(ival);
	bioold = bio;

	for(i = 0; i<scrlheight-1; i++){
        getdiskstats(dev, &bio);
		calcstats(line, bio, bioold, ival);
		wmove(win,i,0);
		waddstr(win,line);
		wrefresh(win);
		printtots(tot);
		sleep(ival);
		check_input();
		bioold = bio;
	}
	
	while(1){
        getdiskstats(dev, &bio);
		calcstats(line, bio, bioold, ival);
		wmove(win,scrlheight-1,0);
		waddstr(win,line);
		scroll(win);
		wrefresh(win);
		printtots(tot);
		sleep(ival);
		check_input();
		bioold = bio;
	}
}

void listio(){
	const char *scan_fmt = NULL;
    char buffer[256];       /* Temporary buffer for parsing */
    int major;
    int minor;
    char devname[8];

    opendiskstats();
    printf("Available devices are:\n");
    scan_fmt = "%4d %4d %s";

    while (fgets(buffer, sizeof(buffer), iofp)) {
        sscanf(buffer, scan_fmt,
			       &major, &minor,
			       &devname);
           
        printf("   %s\n", devname);
    }

    close_end(0);
}

void getdiskstats(char* dev, struct blkio_info *new)
{
	const char *scan_fmt = NULL;
    char buffer[256];       /* Temporary buffer for parsing */
	char	msg[200];

    scan_fmt = "%4d %4d %s %u %u %llu %u %u %u %llu %u %u %u %u";

    rewind(iofp);
    while (fgets(buffer, sizeof(buffer), iofp)) {
        sscanf(buffer, scan_fmt,
			       &(new->major), &(new->minor),
			       &(new->devname), 
			       &(new->rd_ios), &(new->rd_merges),
			       &(new->rd_sectors), &(new->rd_ticks), 
			       &(new->wr_ios), &(new->wr_merges),
			       &(new->wr_sectors), &(new->wr_ticks),
			       &(new->ios_in_prog), 
			       &(new->ticks), &(new->wt_ticks)
        );
        gettimeofday(&(new->tv), NULL);
           
        if(strcmp(new->devname, dev) == 0){
            return;
        }
    }

    sprintf(msg,"Cannot find %s, please chose a valid device from dio -l.\n", dev);
    bomb(1, msg, 0);
}

void calcstats(char* line, struct blkio_info new, struct blkio_info old, int ival)
{
	long long br;	/* bytes read */
	long long bw;	/* bytes written */
	long total;	/* sum of reads and write */
	float brs;	/* bytes read per second */
	float bws;	/* bytes written per second */
	int rr;		/* read ratio */
	int wr;		/* read ratio */
	float active_write_time;	/* active_write_time */
	float active_read_time;	/* active_read_time */
	float deltawlentime;
	float deltarlentime;
	int reads; /* no. of read ops */
	int writes; /* no. of write ops */
        int iosinprog; /* I/Os in progress */
        int realival; /* Real interval, not always ival */

        double milli_old = (old.tv.tv_sec) * 1000 + (old.tv.tv_usec) / 1000 ;
        double milli_new = (new.tv.tv_sec) * 1000 + (new.tv.tv_usec) / 1000 ;
        realival = milli_new - milli_old;
	
	br = (new.rd_sectors - old.rd_sectors) * 512L;
	bw = (new.wr_sectors - old.wr_sectors) * 512L;
	reads = new.rd_ios - old.rd_ios;
	writes = new.wr_ios - old.wr_ios;
	brs = (float)(br / realival);
	bws = (float)(bw / realival);
	total = br + bw;
	if(total > 0){
		rr = (int)((float)br / (float)total * 100.0);
		wr = (int)((float)bw / (float)total * 100.0);
	}
	else{
		rr = 0.0F;
		wr = 0.0F;
	}

	totr+=br;
	totw+=bw;
	if(brs > maxbrs)
		maxbrs = brs;
	if(bws > maxbws)
		maxbws = bws;

        /* Change in milliseconds spent writing */
	deltawlentime = (new.wr_ticks - old.wr_ticks); 
	if(deltawlentime > 0){
		active_write_time = deltawlentime / realival;
	}
	else{
		active_write_time = 0.0;
	}

	deltarlentime = (new.rd_ticks - old.rd_ticks);
	if(deltarlentime > 0){
		active_read_time = deltarlentime / realival;
	}
	else{
		active_read_time = 0.0;
	}

        iosinprog = new.ios_in_prog;

	sprintf(line,"%10lld %10lld %8.1f %8.1f %3d:%-3d %4d  %3d %3d %7.3f %7.3f", br, bw, brs, bws, rr, wr, reads, writes, iosinprog, active_read_time, active_write_time);

	if(ofile != NULL){
	        fprintf(ofile, "%10lld %10lld %8.1f %8.1f %3d:%-3d %4d %4d %3d %7.3f %7.3f\n", br, bw, brs, bws, rr, wr, reads, writes, iosinprog, active_read_time, active_write_time);
		fflush(ofile);
	}
}

void printtots(WINDOW *win)
{
	char line[WIDTH];
	
	sprintf(line, "Max read rate:  %8.1f KB/s     Total bytes read:    %10.2f MB", maxbrs, (float)((totr) / 1048576.0));
	wmove(win,0,0);
	waddstr(win,line);
	sprintf(line, "Max write rate: %8.1f KB/s     Total bytes written: %10.2f MB", maxbws, (float)((totw) / 1048576.0));
	wmove(win,1,0);
	waddstr(win,line);
	wmove(win,2,0);
	wrefresh(win);
}

void usage(){

	printf("Usage: dio [-o <output file>] <device name> <time interval>\n");
	printf("       dio -l\n");
}

void bomb(int exitv, char *msg, int showusage){
	
	endwin();
	printf("%s",msg);
	if(showusage == 1){
		usage();
	}
	exit(exitv);
}

void check_input(){
	int c = 0;

	while(c != -1){
		c = getch();
		if(c == 4 || c == 'q'){
			/* you hit ctrl-d or q*/
			close_end(0);
		}
	}
}

void close_end(int signum) {
	time_t 	end;	/* time now */

	endwin();
	if(ofile != NULL){
		end = time(NULL);
		fprintf(ofile,"\n# Finished: %s", ctime(&end));
		fflush(ofile);
		fclose(ofile);
	}

	exit(0);
}
