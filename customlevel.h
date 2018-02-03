/******************************************************************************
**
** Definition of CustomLevel class
**
******************************************************************************/
#ifndef _CUSTOMLEVEL_H_
#define _CUSTOMLEVEL_H_

#include <qdialog.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

class CustomLevel:public QDialog
{
	Q_OBJECT
public:
	CustomLevel(int m,int r,int c,
			QWidget *parent=0,const char *name=0,bool modal=true,WFlags f=0)
		:QDialog(parent,name,modal,f)
	{
		setCaption(tr("Custom"));
		setFixedSize(240,180);
		QGridLayout *grid=new QGridLayout(this,4,2);
		grid->addWidget(new QLabel(tr("Rows:"),this),0,0);
		grid->addWidget(new QLabel(tr("Cols:"),this),1,0);
		grid->addWidget(new QLabel(tr("Mines:"),this),2,0);

		rows=new QSpinBox(9,24,1,this,"rows");
		rows->setValue(r);
		grid->addWidget(rows,0,1);
		cols=new QSpinBox(9,30,1,this,"cols");
		cols->setValue(c);
		grid->addWidget(cols,1,1);
		mines=new QSpinBox(10,648,1,this,"mines");
		mines->setValue(m);
		grid->addWidget(mines,2,1);

		QPushButton *ok=new QPushButton(tr("&OK"),this,"ok");
		ok->setFixedSize(60,24);
		connect(ok,SIGNAL(clicked()),this,SLOT(myAccept()));
		grid->addWidget(ok,3,0);
		QPushButton *cancel=new QPushButton(tr("&Cancel"),this,"cancel");
		cancel->setFixedSize(60,24);
		connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
		grid->addWidget(cancel,3,1);
	}
public slots:
	void myAccept(void)
	{
		int i=(int)(rows->value()*cols->value()*0.9);
		if(mines->value()>i)
		{
			mines->setValue(i);
			QMessageBox::information(this,tr("Wrong Amount!"),
				tr("Please input a logical mines amount!\n"
				"rows:9-24\n"
				"cols:9-30\n"
				"mines:10-rows * cols * 90%\n"),
				QString(tr("&OK")));
		}
		else
			accept();
	}
public:
	QSpinBox *rows;
	QSpinBox *cols;
	QSpinBox *mines;
};

#endif	//_CUSTOMLEVEL_H_
