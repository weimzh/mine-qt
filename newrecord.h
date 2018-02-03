/******************************************************************************
**
** Definition of NewRecord class
**
******************************************************************************/
#ifndef _NEWRECORD_H_
#define _NEWRECORD_H_

#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

class NewRecord:public QDialog
{
	Q_OBJECT
public:
	NewRecord(QWidget *parent=0,const char *name=0,bool modal=true,WFlags f=0)
		:QDialog(parent,name,modal,f)
	{
		setCaption(tr("New Record"));
		setFixedSize(160,90);
		QVBoxLayout *vbox=new QVBoxLayout(this,5,-1,"vbox");
		vbox->addWidget(new QLabel(tr("You have broken a record!\n"
				"Please input your name:"),this));

		username=new QLineEdit(this,"name");
		username->setMaxLength(10);
		vbox->addWidget(username);

		QHBoxLayout *hbox=new QHBoxLayout(vbox,-1,"hbox");
		hbox->setDirection(QBoxLayout::RightToLeft);
		QPushButton *cancel=new QPushButton(tr("&Cancel"),this,"cancel");
		cancel->setFixedSize(60,24);
		connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));
		hbox->addWidget(cancel);
		QPushButton *ok=new QPushButton(tr("&OK"),this,"ok");
		ok->setDefault(true);
		ok->setFixedSize(60,24);
		connect(ok,SIGNAL(clicked()),this,SLOT(myAccept()));
		hbox->addWidget(ok);
		hbox->addStretch();
	}
public slots:
	void myAccept(void)
	{
		QString str=username->text();
		if(str=="")
		{
			QMessageBox::information(this,tr("Error: Name Empty"),
				tr("Name can't be empty!\nPlease input your name."),
				QString(tr("&OK")));
		}
		else
			accept();
	}
public:
	QLineEdit *username;
};

#endif	//_NEWRECORD_H_
