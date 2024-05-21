#ifndef FINAL_PROJECT_GAME_H
#define	FINAL_PROJECT_GAME_H

#ifdef	__cplusplus
extern "C" {
#endif

extern int codeCompare[4]; //compared code 
extern int masterCode[4]; //Random generated code
extern int userCode[4]; //User input codes
extern int winCond;
extern int didYouWin;

void masterCodeGen(void);
void calculateCompare(void);
void printCompare(int row);
void winCondition(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FINAL_PROJECT_GAME_H */

