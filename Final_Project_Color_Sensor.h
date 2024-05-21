/* 
 * File:   Final_Project_Color_Sensor.h
 * Author: balamuruganarumugam
 *
 * Created on April 26, 2024, 11:30 PM
 */

#ifndef FINAL_PROJECT_COLOR_SENSOR_H
#define	FINAL_PROJECT_COLOR_SENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void read_color_clear(void);
void read_color_red(void);
void read_color_green(void);
void read_color_blue(void);
void i2c_write_byte(char device_address, char register_address, char data);
char read_device_id(void);
void color_init();

extern unsigned char data_clear[2];// clear data stored from ADC
extern unsigned char data_red[2];// red data stored from ADC
extern unsigned char data_green[2];// green data stored from ADC
extern unsigned char data_blue[2]; // blue data stored from ADC

#ifdef	__cplusplus
}
#endif

#endif	/* FINAL_PROJECT_COLOR_SENSOR_H */

