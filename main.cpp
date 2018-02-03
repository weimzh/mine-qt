#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qlocale.h>
#include "mine.h"
#ifdef __APPLE__
#include <stdint.h>
extern "C" extern int _NSGetExecutablePath(char *buf, uint32_t *bufsize);
#endif
int main(int argc,char **argv)
{
#ifdef __APPLE__
	// Hack for OS X Mavericks or later
	char buf[4096];
	uint32_t size = 4096;
	_NSGetExecutablePath(buf, &size);

	char *p = &buf[strlen(buf) - 1];
	while (*p != '/') {
		p--;
	}
	*p = '\0';
	strcat(buf, "/../../..");
	printf("%s\n", buf);
	chdir(buf);
#endif
	QApplication qapp(argc,argv);

	//translation file for Qt
	QTranslator qt(0);
	qt.load(QString("qt_")+QTextCodec::locale(),".");
	qapp.installTranslator(&qt);

	//translation file for application strings
	QTranslator myapp(0);
	myapp.load(QString(DATA_PREFIX "mine_")+QLocale::system().name(),".");
	qapp.installTranslator(&myapp);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

	Mine *mine=new Mine();
	mine->show();
	qapp.setMainWidget(mine);
	return qapp.exec();
}
