#ifndef FINAL_PROJECT_COLOR_MOVING_AVERAGE_H
#define	FINAL_PROJECT_COLOR_MOVING_AVERAGE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern int buffer_index;
extern unsigned long int clear_avg, red_avg, green_avg, blue_avg;

void initBuffer();
void putClear(int clear);
void putRed(int red);
void putGreen(int green);
void putBlue(int blue);
unsigned int clearAvg();
unsigned int redAvg();
unsigned int greenAvg();
unsigned int blueAvg();
void get_color_avg(void);
char identify_color(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FINAL_PROJECT_COLOR_MOVING_AVERAGE_H */

