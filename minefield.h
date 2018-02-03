/******************************************************************************
**
** Definition of MineField class
**
******************************************************************************/
#ifndef _MINEFIELD_H_
#define _MINEFIELD_H_

#include <qwidget.h>
#include <qpixmap.h>

class MineField:public QWidget
{
	Q_OBJECT
public:
	MineField(int total=99,int rows=16,int columns=30,
			QWidget *parent=0,const char *name=0);
//	~MineField(void);
	void reset(void);
	void click(int,int);
	void checkIfWin(void);
	void startGame(int,int);
	void endGame(bool);
	void stopTime(bool st){timeStop=st;}
	void setMRC(int,int,int);
	void setInterrogation(bool b){enableInterrogation=b;}
	QSizePolicy sizePolicy() const;
protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);
	void hideEvent(QHideEvent*){stopTime(true);}
	void showEvent(QShowEvent*){stopTime(false);}
private:
	void initMN(int,int);
	void initParameters(void);
	void paintFrame(QPainter *);
	void paintLCDPanel(QPainter *p);
	void paintMines(QPainter *p);
	void paintCheat(QPainter *p);
	QRect minesRect(int r=-1,int c=-1) const;
	//r=-1&&c=-1: return minesfield;		else return the single mine
	QRect lcdRect(int index=0) const;
	//0: lcdPanel		1: CNum		2: second		3: face
	QRect cheatRect(void) const;
	void layMines(int sRow,int sCol);
	void pressLeftButton(int,int);
	void releaseLeftButton(int,int);
	void pressLRButton(int,int);
	void releaseLRButton(int,int);
	void timerEvent(QTimerEvent *);
signals:
	void newRecord(int);
private:
	QPainter	*paint;
	QPixmap		*pixmap;
	QPixmap		lcd;
	QPixmap		face;
	QPixmap		pan;
	int			TOTAL,CNum;
	int			MAX_ROWS,MAX_COLS;
	int			bomb[24][30];
	int			flag[24][30];
	int			emotion;
	int			second;
	int			timer;
	int			buttonstate,oldbuttonstate;
	bool		started;
	bool		gameOver;
	bool		timeStop;
	bool		enableInterrogation;
	bool		cheater;
};

#define lcdUpEdge 11
#define lcdBottomEdge 46
#define lcdLeftEdge 10
#define lcdRightEdge (width()-8)
#define upEdge 57
#define bottomEdge (height()-9)
#define leftEdge 11
#define rightEdge (width()-9)
/*
bomb:
	0-8		0-8
	-1		mine

flag:
	-3		? down
	-2		down
	-1		uncovered
	0		covered
	1		flag
	2		?
	3		explode
	4		wrong flag
	5		mine

emotion:
	same as face.bmp

pan.bmp:
	0-8		0-8 uncovered
	9		? uncovered
	10		mine
	11		wrong flag
	12		explode
	13		? covered
	14		flag
	15		covered

lcd.bmp:
	0-9		0-9
	10		blank
	11		-

face.bmp:
	0		down
	1		win
	2		fail
	3		wait
	4		normal
*/

#endif	//_MINEFIELD_H_
