#ifndef FINAL_PROJECT_LCD_H
#define	FINAL_PROJECT_LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

void clearLCD();
void lcd_init();
void lcd_cmd(char Package);
void lcd_printChar(char Package);
void lcd_setCursor(char x, char y);
void lcd_printStr(const char s[]);

#ifdef	__cplusplus
}
#endif

#endif	/* FINAL_PROJECT_LCD_H */

