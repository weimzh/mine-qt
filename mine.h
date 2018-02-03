/******************************************************************************
**
** Definition of Mine class
**
******************************************************************************/
#ifndef _MINE_H_
#define _MINE_H_

#include <qwidget.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qlayout.h>
#include "minefield.h"
#include "customlevel.h"
#include "newrecord.h"

#ifdef __APPLE__
#define DATA_PREFIX "./mine.app/Contents/Resources/"
#elif defined (_WIN32)
#define DATA_PREFIX "./"
#else
#define DATA_PREFIX "/usr/share/apps/mine/"
#endif

typedef struct
{
	int level;
	int time;
	char name[20];
}TAXIS;

class Mine:public QWidget
{
	Q_OBJECT
public:
	Mine(QWidget *parent=0,const char *name=0);
//	~Mine(void);
public slots:
	void reset(void);
	void setLevel(int);
	void newRecord(int);
	void showTaxis(void);
	void quit(void);
	void showAbout();
	void showAboutQt();
private:
	void readINI(void);
	void writeINI(void);
	void selectLevel(void);
	void initDefaultParameters(void);
	void initMenubar(void);
	void closeEvent(QCloseEvent*);
	QVBoxLayout *vbox;
	QMenuBar *menubar;
	QPopupMenu *game;
	MineField *minefield;
	CustomLevel *customlevel;
	NewRecord *newrecord;
	int level;
	int	rows,cols,mine_num;
	bool enableInterrogation;
	TAXIS taxis[3];
};


#endif	//_MINE_H_
