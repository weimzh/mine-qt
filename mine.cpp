/******************************************************************************
**
** Implementation Mine class
**
******************************************************************************/
#include <qapplication.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
//#include <qdialog.h>
#include <qmessagebox.h>
//#include <qinputdialog.h>
#include <stdlib.h>
#include <stdio.h>

#include "mine.h"

Mine::Mine(QWidget *parent,const char *name)
	:QWidget(parent,name)
{
	readINI();
	vbox=new QVBoxLayout(this,0,-1,"vbox");
	initMenubar();
	vbox->setMenuBar(menubar);
//	menubar->setSeparator(QMenuBar::InWindowsStyle);
	minefield=new MineField(mine_num,rows,cols,this,"minefield");
	minefield->setInterrogation(enableInterrogation);
	connect(minefield,SIGNAL(newRecord(int)),this,SLOT(newRecord(int)));
	setCaption(tr("mine"));
	setFixedSize(16*cols+20,16*rows+66+menubar->height());
	vbox->addWidget(minefield);
	setFocusProxy(minefield);
	setWFlags(WNoAutoErase);
}

// Mine::~Mine(void)
// {
// 	writeINI();
// 	destroy();
// }

void Mine::closeEvent(QCloseEvent *ce)
{
	writeINI();
	ce->accept();
	return;
}

void Mine::readINI(void)
{
	FILE* fp;
	char buf[4096];
#ifdef __APPLE__
	sprintf(buf, "%s/Library/Preferences/com.jhwu.minesweeper.plist", getenv("HOME"));
#elif defined (_WIN32)
	sprintf(buf, "mine.ini");
#else
	sprintf(buf, "%s/.minerc", getenv("HOME"));
#endif
	if((fp=fopen(buf,"rb+"))==NULL)
	{
#if 0
		QMessageBox warn(tr("Config Lost!"),
			tr("Config File Not Found,use default!"),
			QMessageBox::Warning,
			QMessageBox::Yes|QMessageBox::Default,0,0);
    	warn.setButtonText(QMessageBox::Yes,QString(tr("&OK")));
    	warn.exec();
#endif
		initDefaultParameters();
		writeINI();
	}
	else
	{
		int count=0;
		count+=fread(&level,sizeof(int),1,fp);//current level
		count+=fread(&rows,sizeof(int),1,fp);//rows
		count+=fread(&cols,sizeof(int),1,fp);//cols
		count+=fread(&mine_num,sizeof(int),1,fp);//mine amount
		count+=fread(&enableInterrogation,sizeof(bool),1,fp);//whether use tag "?"
		for(int i=0;i<=2;i++)
			count+=fread(&taxis[i],sizeof(TAXIS),1,fp);
		fclose(fp);
		if(count!=8)
		{
#if 0
			QMessageBox warn(tr("Config Lost!"),
				tr("Config file has been crashed,use default!"),
				QMessageBox::Warning,
				QMessageBox::Yes|QMessageBox::Default,0,0);
			warn.setButtonText(QMessageBox::Yes,QString(tr("&OK")));
			warn.exec();
#endif
			initDefaultParameters();
			writeINI();
		}
	}
}

void Mine::writeINI(void)
{
	FILE* fp;
	char buf[4096];
#ifdef __APPLE__
	sprintf(buf, "%s/Library/Preferences/com.jhwu.minesweeper.plist", getenv("HOME"));
#elif defined (_WIN32)
	sprintf(buf, "mine.ini");
#else
	sprintf(buf, "%s/.minerc", getenv("HOME"));
#endif
	if((fp=fopen(buf,"wb"))==NULL)
	{
		QMessageBox::critical(this,tr("Critical Error!"),
			tr("write config file fail!\n"
			"terminate now!\n"),
			QString(tr("&OK")));
		exit(1);
	}
	fwrite(&level,sizeof(int),1,fp);//current level
	fwrite(&rows,sizeof(int),1,fp);//rows
	fwrite(&cols,sizeof(int),1,fp);//cols
	fwrite(&mine_num,sizeof(int),1,fp);//mine amount
	fwrite(&enableInterrogation,sizeof(bool),1,fp);//whether use tag "?"
	for(int i=0;i<=2;i++)
		fwrite(&taxis[i],sizeof(TAXIS),1,fp);
	fclose(fp);
}

void Mine::initDefaultParameters(void)
{
	level=0;
	rows=cols=9,mine_num=10;
	enableInterrogation=true;
	for(int i=0;i<=2;i++)
	{
		taxis[i].level=0;
		taxis[i].time=999;
		sprintf(taxis[i].name, "%s", QString(tr("noname")).local8Bit().data());
	}
}

void Mine::selectLevel(void)
{
	switch(level)
	{
		case 0:
			rows=9;
			cols=9;
			mine_num=10;
			break;
		case 1:
			rows=16;
			cols=16;
			mine_num=40;
			break;
		case 2:
			rows=16;
			cols=30;
			mine_num=99;
			break;
	}
}

void Mine::reset(void)
{
	minefield->reset();
}

void Mine::setLevel(int l)
{
	if(l>=0&&l<=2)
		if(level==l)
			reset();
		else
		{
			for(int i=0;i<=3;i++)
				if(i==l)
					game->setItemChecked(i,true);
				else
					game->setItemChecked(i,false);

			level=l;
			selectLevel();
			minefield->setMRC(mine_num,rows,cols);
			minefield->setFixedSize(16*cols+20,16*rows+66);
			setFixedSize(16*cols+20,16*rows+66+menubar->height());
			update();
//	printf("rows=%d,cols=%d...setLevel\n",rows,cols);
		}
	else if(l==3)
	{
		customlevel=new CustomLevel(mine_num,rows,cols,this,"customlevel",true);
		if(customlevel->exec())
		{
			level=l;
			rows=customlevel->rows->value();
			cols=customlevel->cols->value();
			mine_num=customlevel->mines->value();
			for(int i=0;i<=3;i++)
				if(i==l)
					game->setItemChecked(i,true);
				else
					game->setItemChecked(i,false);
			minefield->setMRC(mine_num,rows,cols);
			minefield->setFixedSize(16*cols+20,16*rows+66);
			setFixedSize(16*cols+20,16*rows+66+menubar->height());
			update();
		}
	}
	else if(l==4)
	{
		enableInterrogation=!enableInterrogation;
		minefield->setInterrogation(enableInterrogation);
		game->setItemChecked(4,enableInterrogation);
	}
}

void Mine::newRecord(int t)
{
	if(level>=0&&level<=2&&t<taxis[level].time)
	{
		newrecord=new NewRecord(this,"newrecord",true);
		if(newrecord->exec())
		{
			taxis[level].time=t;
			QCString CName=newrecord->username->text().local8Bit();
			sprintf(taxis[level].name,"%s",CName.data());
			writeINI();
			showTaxis();
		}
	}
}

void Mine::showTaxis(void)
{
	QString str="";
	str+=QString(tr("Beginer:\t\t%1\t%2\n"
				"Middle:\t\t%3\t%4\n"
				"Experienced:\t%5\t%6\n").
				arg(taxis[0].time).arg(taxis[0].name).
				arg(taxis[1].time).arg(taxis[1].name).
				arg(taxis[2].time).arg(taxis[2].name));
	QMessageBox info(tr("Taxis"),str,QMessageBox::NoIcon,
		QMessageBox::Ok|QMessageBox::Default,0,0,this,"info",true);
	info.setButtonText(QMessageBox::Ok,QString(tr("&OK")));
	info.exec();
}

void Mine::quit(void)
{
	close();
	qApp->quit();
}

void Mine::showAbout(void)
{
    QMessageBox::information(this,tr("About mine"),
        tr("mine-1.0\n\nMine was written for QT program practice,\nand wish you enjoy it.\n\n"
        "Author: jhwu\n"
        "Email: jhwu@redflag-linux.com\n"),
		QString(tr("&OK")));
}

void Mine::showAboutQt(void)
{
	QMessageBox::aboutQt(this);
}

void Mine::initMenubar(void)
{
	menubar=new QMenuBar(this);
	Q_CHECK_PTR(menubar);

	game=new QPopupMenu(menubar);
	Q_CHECK_PTR(game);
	menubar->insertItem(tr("&Game"),game);
	game->setCheckable(true);
	game->insertItem(tr("&New"),this,SLOT(reset()),Key_F2);
	game->insertSeparator();
	game->insertItem(tr("&Beginner"),0);
	game->insertItem(tr("M&iddle"),1);
	game->insertItem(tr("&Experienced"),2);
	game->insertItem(tr("&Custom"),3);
	game->insertSeparator();
	game->insertItem(tr("use &mark (?)"),4);
	game->insertSeparator();
	game->insertItem(tr("&Taxis"),this,SLOT(showTaxis()));
	game->insertSeparator();
	game->insertItem(tr("&Quit"),this,SLOT(quit()),CTRL+Key_Q);
	connect(game,SIGNAL(activated(int)),this,SLOT(setLevel(int)));

	QPopupMenu *help=new QPopupMenu(menubar);
	menubar->insertItem(tr("&Help"),help);
	help->insertItem(tr("&About"),this,SLOT(showAbout()));
	help->insertItem(tr("About&QT"),this,SLOT(showAboutQt()));
//help->insertItem("new record",this,SLOT(newRecord()));

	game->setItemChecked(level,true);
	game->setItemChecked(4,enableInterrogation);
}

