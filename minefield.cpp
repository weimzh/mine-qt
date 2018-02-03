/******************************************************************************
**
** Implementation MineField class
**
******************************************************************************/
#include "minefield.h"
#include "mine.h"

#include <qpainter.h>
#include <stdlib.h>
//#include <mcheck.h>
#include <time.h>

int oldEmotion;		//记录鼠标按键事件之前face的状态
int startX,startY;		//鼠标移动事件的起始位置
int startRow,startCol;		//鼠标移动事件起始位置对应格子坐标
int l,r,t,b,m,n;		//临时判定用边界
bool faceDown;		//记录鼠标按键事件之前face是否被按下
bool cheatInput;		//标记处于作弊码输入状态
bool cheatTimeStop;		//已经没有实际用处，保留以表示存在相应作弊状态
bool cheatPowerOverwhelming;	//无敌状态是否开启
int cheatSecond;		//输入作弊码到消失计时5秒
QString inputString;	//储存用户输入，和作弊码比较
bool disableLeft;		//为忽略双键事件后的左键事件，临时禁用左键标志

/*判定局部循环边界*************************************************************/
void MineField::initMN(int i,int j)
{
	l=(j-1)>0?(j-1):0;
	r=(j+1)>(MAX_COLS-1)?(MAX_COLS-1):(j+1);
	t=(i-1)>0?(i-1):0;
	b=(i+1)>(MAX_ROWS-1)?(MAX_ROWS-1):(i+1);
}

/*初始化作弊状态***************************************************************/
void initCheat(void)
{
	cheatInput=false;
	cheatTimeStop=false;
	cheatPowerOverwhelming=false;
	inputString="";
	cheatSecond=0;
}

/*construtor*****************************************************/
MineField::MineField(int total,int rows,int columns,
						QWidget *parent,const char *name)
	:QWidget(parent,name),TOTAL(total),MAX_ROWS(rows),MAX_COLS(columns)
{
	pixmap=0;
	timeStop=false;
	initCheat();
	initParameters();
	setWFlags(WNoAutoErase);
	setPalette(QPalette(QColor(192,192,192)));
	setFixedSize(16*MAX_COLS+20,16*MAX_ROWS+66);
	setFocusPolicy(QWidget::StrongFocus);
	lcd.load(DATA_PREFIX "icons/lcd.bmp");
	face.load(DATA_PREFIX "icons/face.bmp");
	pan.load(DATA_PREFIX "icons/pan.bmp");
	timer=startTimer(1000);
//	setKeyCompression(true);
}

// MineField::~MineField(void)
// {
// fprintf(stderr,"minefield's distructor...\n");
// 	destroy();
// }

void MineField::initParameters(void)
{
	if(timeStop==false&&cheatPowerOverwhelming==false)
		cheater=false;
	else
		cheater=true;
	oldEmotion=4;
	startX=0,startY=0;
	startRow=-1,startCol=-1;
	faceDown=false;
	disableLeft=false;
	started=false,gameOver=false;
//	enableInterrogation=true;
	if(TOTAL>667) TOTAL=667;
	else if(TOTAL<10) TOTAL=10;
	if(MAX_ROWS>24) MAX_ROWS=24;
	else if(MAX_ROWS<6) MAX_ROWS=6;
	if(MAX_COLS>30) MAX_COLS=30;
	else if(MAX_COLS<6) MAX_COLS=6;
	CNum=TOTAL;
	emotion=4;
	second=0;
	buttonstate=oldbuttonstate=0;
	for(int i=0;i<MAX_ROWS;i++)
		for(int j=0;j<MAX_COLS;j++)
			bomb[i][j]=0,flag[i][j]=0;
	if(pixmap!=0)
		delete pixmap;
	pixmap=new QPixmap(16*MAX_COLS+20,16*MAX_ROWS+66);
}

void MineField::paintFrame(QPainter *p)
{
	int w=width(),h=height();
	QPointArray a;
	QPixmap pix(w,h);
	pix.fill(this,0,0);
	QPainter *tmp=new QPainter(&pix);

	tmp->setPen(NoPen);
	tmp->setBrush(white);
	tmp->drawRect(0,0,w,4);
	tmp->drawRect(0,0,3,h);
	tmp->drawRect(8,46,w-14,2);
	tmp->drawRect(w-8,9,2,37);
	tmp->drawRect(8,h-9,w-14,3);
	tmp->drawRect(w-9,54,3,h-60);

	tmp->setBrush(QColor(128,128,128));
	tmp->drawRect(8,9,w-16,2);
	tmp->drawRect(8,9,2,37);
	tmp->drawRect(8,54,w-17,3);
	tmp->drawRect(8,54,3,h-63);
	tmp->drawRect(2,h-2,w-3,2);
	tmp->drawRect(w-2,3,2,h-4);

	a.setPoints(3,w-8,9,w-8,11,w-6,9);
	tmp->drawPolygon(a);
	a.setPoints(3,8,46,8,48,10,46);
	tmp->drawPolygon(a);
	a.setPoints(3,w-9,54,w-9,57,w-6,54);
	tmp->drawPolygon(a);
	a.setPoints(3,8,h-9,8,h-6,11,h-9);
	tmp->drawPolygon(a);
	a.setPoints(3,w-1,3,w-1,0,w-3,3);
	tmp->drawPolygon(a);
	a.setPoints(3,2,h-1,2,h-3,0,h-1);
	tmp->drawPolygon(a);
	tmp->end();

	p->drawPixmap(0,0,pix);
//	bitBlt(this,0,0,&pix);
}

void MineField::paintLCDPanel(QPainter *p)
{
	QRect cr=lcdRect();
	QPixmap pix(cr.size());
	pix.fill(this,cr.topLeft());
	QPainter tmp(&pix);
	int w=cr.width();
	int tnum=CNum;

	tmp.setPen(QPen(QColor(128,128,128),1));
	tmp.drawLine(5,5,44,5);
	tmp.drawLine(5,5,5,28);
	tmp.drawLine(w/2-14,4,w/2+10,4);
	tmp.drawLine(w/2-14,4,w/2-14,28);
	tmp.drawLine(w/2-13,29,w/2+11,29);
	tmp.drawLine(w/2+11,5,w/2+11,29);
	tmp.drawLine(w-47,5,w-7,5);
	tmp.drawLine(w-47,5,w-47,28);

	tmp.setPen(QPen(white,1));
	tmp.drawLine(45,29,45,6);
	tmp.drawLine(45,29,6,29);
	tmp.drawLine(w-7,29,w-7,5);
	tmp.drawLine(w-7,29,w-46,29);

	if(CNum<0)
	{
		tnum=-CNum;
		tmp.drawPixmap(6,6,lcd,0,0,-1,23);
	}
	else
		tmp.drawPixmap(6,6,lcd,0,(11-tnum/100)*23,-1,23);

	tmp.drawPixmap(19,6,lcd,0,(11-(tnum%100)/10)*23,-1,23);
	tmp.drawPixmap(32,6,lcd,0,(11-tnum%10)*23,-1,23);

	tmp.drawPixmap(w/2-13,5,face,0,emotion*24,-1,24);

	tmp.drawPixmap(w-46,6,lcd,0,(11-second/100)*23,-1,23);
	tmp.drawPixmap(w-33,6,lcd,0,(11-(second%100)/10)*23,-1,23);
	tmp.drawPixmap(w-20,6,lcd,0,(11-second%10)*23,-1,23);

	tmp.end();
	p->drawPixmap(cr.topLeft(),pix);
//	bitBlt(this,cr.topLeft(),&pix);
}

// void MineField::paintMines(QPainter *p)
// {
// 	QPixmap pan("icons/pan.bmp");
// 	mtrace();
// 	for(int i=0;i<MAX_ROWS;i++)
// 		for(int j=0;j<MAX_COLS;j++)
// 		{
// 			int x=minesRect(i,j).x(),y=minesRect(i,j).y();
// 			if(flag[i][j]==-1)
// 				p->drawPixmap(x,y,pan,0,16*(15-bomb[i][j]),-1,16);
// 			else if(flag[i][j]==-2)
// 				p->drawPixmap(x,y,pan,0,16*15,-1,16);
// 			else if(flag[i][j]==-3)
// 				p->drawPixmap(x,y,pan,0,16*6,-1,16);
// 			else
// 				p->drawPixmap(x,y,pan,0,16*flag[i][j],-1,16);
// 		}
// 	muntrace();
// }

void MineField::paintMines(QPainter *p)
{
	QRect cr=minesRect();
	QPixmap pix(cr.size());
	pix.fill(this, cr.topLeft());
	QPainter tmp( &pix );
//	mtrace();
	for(int i=0;i<MAX_ROWS;i++)
		for(int j=0;j<MAX_COLS;j++)
		{
			if(flag[i][j]==-1)
				tmp.drawPixmap(16*j,16*i,pan,0,16*(15-bomb[i][j]),-1,16);
			else if(flag[i][j]==-2)
				tmp.drawPixmap(16*j,16*i,pan,0,16*15,-1,16);
			else if(flag[i][j]==-3)
				tmp.drawPixmap(16*j,16*i,pan,0,16*6,-1,16);
			else
				tmp.drawPixmap(16*j,16*i,pan,0,16*flag[i][j],-1,16);
		}
	tmp.end();
	p->drawPixmap( cr.topLeft(), pix );
//	bitBlt(this,cr.topLeft(),&pix);
//	muntrace();
}

void MineField::paintCheat(QPainter *p)
{
	QRect r=cheatRect();
	p->setFont(QFont("times",12,QFont::Bold));
	p->setPen(green);
	p->drawText(r.x(),r.y(),inputString);
}

void MineField::paintEvent(QPaintEvent *e)
{
	QPainter p(pixmap);
	if(!started)
		paintFrame(&p);
	paintLCDPanel(&p);
	if(inputString!="")
		paintCheat(&p);
//	if ( e->rect().intersects(minesRect()))
		paintMines(&p);
	p.end();
	bitBlt(this,0,0,pixmap);
}

QRect MineField::lcdRect(int index) const
{
	QRect *r;
	switch(index)
	{
		case 1:		//CNum
			r=new QRect(lcdLeftEdge+6,lcdUpEdge+6,39,23);
			break;
		case 2:		//second
			r=new QRect(lcdRightEdge-45,lcdUpEdge+6,39,23);
			break;
		case 3:		//face
			r=new QRect(width()/2-12,lcdUpEdge+5,24,24);
			break;
		case 0:		//lcdPanel
		default:
			r=new QRect(lcdLeftEdge,lcdUpEdge,MAX_COLS*16+2,35);
//			r.moveTopLeft(QPoint(10,11));
	}
	return *r;
}

QRect MineField::minesRect(int r,int c) const
{
	QRect *mr;
	if(r==-1&&c==-1)
		mr=new QRect(leftEdge,upEdge,MAX_COLS*16,MAX_ROWS*16);
	else
		mr=new QRect(leftEdge+c*16,upEdge+r*16,16,16);
	return *mr;
}

QRect MineField::cheatRect(void) const
{
	QRect r(lcdLeftEdge+50,lcdUpEdge+20,120,24);
	return r;
}

QSizePolicy MineField::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void MineField::reset(void)
{
	initParameters();
	update();
}

void MineField::setMRC(int m,int r,int c)
{
	TOTAL=m;
	MAX_ROWS=r;
	MAX_COLS=c;
	reset();
}

void MineField::click(int i,int j)
{
if(cheatPowerOverwhelming)
{
	if(flag[i][j]!=1&&flag[i][j]!=-1&&flag[i][j]!=5)
		if(bomb[i][j]==-1)
		{
			flag[i][j]=5;
			update(minesRect(i,j));
			CNum--;
			update(lcdRect(1));
		}
		else
		{
			flag[i][j]=-1;
			update(minesRect(i,j));
			if(bomb[i][j]==0)
			{
				int tl,tr,tt,tb,tm,tn;
				tl=(j-1)>0?(j-1):0;
				tr=(j+1)>(MAX_COLS-1)?(MAX_COLS-1):(j+1);
				tt=(i-1)>0?(i-1):0;
				tb=(i+1)>(MAX_ROWS-1)?(MAX_ROWS-1):(i+1);
				for(tm=tt;tm<=tb;tm++)
					for(tn=tl;tn<=tr;tn++)
						click(tm,tn);
			}
		}
}
else
{
	if(flag[i][j]!=1&&flag[i][j]!=-1&&flag[i][j]!=5)
		if(bomb[i][j]==-1)
		{
			gameOver=true;
			flag[i][j]=3;
//			printf("GAME OVER!...\n");
			endGame(false);
		}
		else
		{
			flag[i][j]=-1;
			update(minesRect(i,j));
			if(bomb[i][j]==0)
			{
				int tl,tr,tt,tb,tm,tn;
				tl=(j-1)>0?(j-1):0;
				tr=(j+1)>(MAX_COLS-1)?(MAX_COLS-1):(j+1);
				tt=(i-1)>0?(i-1):0;
				tb=(i+1)>(MAX_ROWS-1)?(MAX_ROWS-1):(i+1);
				for(tm=tt;tm<=tb;tm++)
					for(tn=tl;tn<=tr;tn++)
						click(tm,tn);
			}
		}
}
}

void MineField::checkIfWin(void)
{
	int panRemain=0;
	for(int i=0;i<MAX_ROWS;i++)
		for(int j=0;j<MAX_COLS;j++)
			if(flag[i][j]!=-1)
			{
				if((flag[i][j]==1)&&(bomb[i][j]!=-1))
					return;
				else
					panRemain++;
			}
	if(panRemain==TOTAL)
		endGame(true);
}

void MineField::startGame(int r,int c)
{
	if(!started)
	{
		layMines(r,c);
		started=true;
		second=1;
		update(lcdRect(2));
//		timer=startTimer(1000);
	}
}

void MineField::endGame(bool win)
{
	for(int i=0;i<MAX_ROWS;i++)
		for(int j=0;j<MAX_COLS;j++)
			if(flag[i][j]==0||flag[i][j]==2)
			{
				if(bomb[i][j]==-1)
					if(win)
						flag[i][j]=1;
					else
						flag[i][j]=5;
//加入这个else触雷失败时打开不是雷的格子
//				else
//					flag[i][j]=-1;
			}
			else if(!win)
			{
				if((flag[i][j]==1)&&(bomb[i][j]!=-1))
					flag[i][j]=4;
			}

	if(win)
	{
		emotion=1;
		CNum=0;
	}
	else
		emotion=2;

	started=false;
	gameOver=true;
	update();

	if(win&&cheater==false)
		emit newRecord(second);

}

void MineField::timerEvent(QTimerEvent *)
{
	if(started&&!timeStop)
		if(++second<=999)
			update(lcdRect(2));
		else
			timeStop=true;
	if(cheatSecond)
		if(++cheatSecond>=5)
		{
			cheatSecond=0;
			inputString="";
			update(cheatRect());
		}
}

void MineField::keyPressEvent(QKeyEvent *ke)
{
	if(ke->ascii()=='`'&&cheatInput==false)
	{
		cheatInput=true;
		cheatSecond=0;
		inputString="";
		update(cheatRect());
		return;
	}
	if(cheatInput==true)
	{
		if(ke->ascii()!=13)
		{
			if(ke->key()==Qt::Key_Backspace)
				inputString.truncate(inputString.length()-1);
			else
				inputString+=ke->text();
			update(cheatRect());
		}
		else
		{
			cheatSecond=1;
			if(inputString=="time stop")
				stopTime(true),cheater=true;
			else if(inputString=="time restart")
				stopTime(false);
			else if(inputString=="power overwhelming")
				cheatPowerOverwhelming=!cheatPowerOverwhelming,cheater=true;
			cheatInput=false;
//			inputString="";
		}
	}
}

void MineField::layMines(int sRow,int sCol)
{
	int lay=0;
	int i,j;
	int bnum[MAX_ROWS][MAX_COLS];

	srand((unsigned)time(NULL));

	for(i=0;i<MAX_ROWS;i++)
		for(j=0;j<MAX_COLS;j++)
			bnum[i][j]=0;

	bomb[sRow][sCol]=0;
	while(lay<TOTAL)
	{
		i=(int)(((double)MAX_ROWS*rand())/(RAND_MAX+1.0));
		j=(int)(((double)MAX_COLS*rand())/(RAND_MAX+1.0));
		if(bomb[i][j]!=-1&&(i!=sRow||j!=sCol))
		{
			bomb[i][j]=-1;
			lay++;
			initMN(i,j);
			for(m=t;m<=b;m++)
				for(n=l;n<=r;n++)
					if(bomb[m][n]!=-1)
						bnum[m][n]++;
		}
	}

	for(i=0;i<MAX_ROWS;i++)
		for(j=0;j<MAX_COLS;j++)
			if(bomb[i][j]!=-1)
				bomb[i][j]=bnum[i][j];
}

void MineField::mousePressEvent(QMouseEvent *e)
{
	int w=width();//,h=height();
	int curX=e->x(),curY=e->y();

	if(curX>=w/2-12&&curX<=w/2+12&&curY>=lcdUpEdge+6
			&&curY<=lcdUpEdge+30&&e->button()==LeftButton)
	{
		startX=e->x(),startY=e->y();
		oldEmotion=emotion;
		emotion=0;
		faceDown=true;
		update(lcdRect(3));
	}
	else if(!gameOver)
		 if(curX>=leftEdge&&curX<=rightEdge&&curY>=upEdge
					&&curY<=bottomEdge)
	{
		int mX=curX-leftEdge,mY=curY-upEdge;
		int mRow=mY/16,mCol=mX/16;
		int i=mRow,j=mCol;

		buttonstate+=e->button()&MouseButtonMask;
//		printf("button state: %d\n",buttonstate);
		if(buttonstate>=3)
			pressLRButton(i,j);
		else if(buttonstate==1)
			pressLeftButton(i,j);
		else// if(buttonstate==2)
		{
			if(flag[i][j]>=0&&flag[i][j]<=2)
			{
				if(enableInterrogation==false)
				{
					flag[i][j]=(++flag[i][j]%2);
					if(flag[i][j]==1)
						CNum--;
					else
						CNum++;
				}
				else
				{
					flag[i][j]=(++flag[i][j]%3);
					if(flag[i][j]==1)
						CNum--;
					else if(flag[i][j]==2)
						CNum++;
				}
				update(lcdRect(1));
				update(minesRect(i,j));
			}
		}

/*		switch(e->stateAfter()&MouseButtonMask)
		{
			case LeftButton:
//				printf("L!R pressed...\n");
				if(!disableLeft)
					pressLeftButton(i,j);
				break;
			case RightButton:
				if(flag[i][j]>=0&&flag[i][j]<=2)
				{
					if(enableInterrogation==false)
					{
						flag[i][j]=(++flag[i][j]%2);
						if(flag[i][j]==1)
							CNum--;
						else
							CNum++;
					}
					else
					{
						flag[i][j]=(++flag[i][j]%3);
						if(flag[i][j]==1)
							CNum--;
						else if(flag[i][j]==2)
							CNum++;
					}
					update(lcdRect(1));
					update(minesRect(i,j));
				}
				break;
//			case ShiftButton+LeftButton:
			case MidButton:
			case LeftButton+RightButton:
//				printf("L&R pressed...\n");
				pressLRButton(i,j);
				break;
			default:
				if(e->state()==LeftButton)
					releaseLeftButton(i,j);
				else if(e->state()==LeftButton+RightButton)
					releaseLRButton(i,j);
		}
*/
	}
}

void MineField::mouseReleaseEvent(QMouseEvent *e)
{
	int w=width();//h=height();
	int curX=e->x(),curY=e->y();

	if(curX>=w/2-12&&curX<=w/2+12&&curY>=lcdUpEdge+6
			&&curY<=lcdUpEdge+30&&e->button()==LeftButton)
		reset();

	if(!gameOver&&emotion!=1&&emotion!=2)//&&e->button()==LeftButton)
	{
		emotion=4;
		update(lcdRect(3));
	}

	 if(!gameOver&&curX>=leftEdge&&curX<=rightEdge&&curY>=upEdge
		&&curY<=bottomEdge)
	{
		int mX=curX-leftEdge,mY=curY-upEdge;
		int mRow=mY/16,mCol=mX/16;
		int i=mRow,j=mCol;

		initMN(i,j);

		if(buttonstate>=3)
		{
			if(flag[i][j]==-1&&bomb[i][j]!=0)
			{
				int flags=0;
				for(m=t;m<=b;m++)
					for(n=l;n<=r;n++)
						if(flag[m][n]==1||flag[m][n]==5)
							flags++;

				if(flags==bomb[i][j])
				{
					for(m=t;m<=b;m++)
						for(n=l;n<=r;n++)
						{
							click(m,n);
							if(!gameOver)
								checkIfWin();
						}
				}
			}
			releaseLRButton(i,j);
		}
		else if(buttonstate==1)
		{
			if(!started)
			{
				startGame(i,j);
				click(i,j);
			}
			else if(flag[i][j]!=1&&flag[i][j]!=-1)
			{
				click(i,j);
				if(!gameOver)
					checkIfWin();
			}
		}
		buttonstate=0;

/*		switch(e->state()&MouseButtonMask)
		{
			case LeftButton:
				if(disableLeft)
					disableLeft=false;
				else
				{
//					printf("L!R released...\n");
					if(!started)
					{
						startGame(i,j);
						click(i,j);
					}
					else if(flag[i][j]!=1&&flag[i][j]!=-1)
					{
						click(i,j);
						if(!gameOver)
							checkIfWin();
					}
				}
				break;
			case RightButton:
				if(disableLeft)
					disableLeft=false;
//				else
//				printf("R!L released...\n");
				break;
			case MidButton:
			case LeftButton+RightButton:
//				printf("L&R released...\n");
				if(flag[i][j]==-1&&bomb[i][j]!=0)
				{
					int flags=0;
					for(m=t;m<=b;m++)
						for(n=l;n<=r;n++)
							if(flag[m][n]==1||flag[m][n]==5)
								flags++;

					if(flags==bomb[i][j])
					{
						for(m=t;m<=b;m++)
							for(n=l;n<=r;n++)
							{
								click(m,n);
								if(!gameOver)
									checkIfWin();
							}
//						if(e->button()==RightButton)
//							disableLeft=true;
						break;
					}
				}
				releaseLRButton(i,j);
				if(e->button()==RightButton)
					disableLeft=true;
				break;
			default:
				;
		}
*/
	}
}

void MineField::mouseMoveEvent(QMouseEvent *e)
{
	int curX=e->x(),curY=e->y();
	QRect cr=lcdRect(3);

	if(startX>=cr.left()&&startX<=cr.right()&&startY>=cr.top()
			&&startY<=cr.bottom())//&&e->state()==LeftButton)
	if(curX<cr.left()||curX>cr.right()||curY<cr.top()||curY>cr.bottom())
	{
		if(faceDown)
		{
			emotion=oldEmotion;
			faceDown=false;
			update(lcdRect(3));
		}
	}
	else if(faceDown==false)
	{
		oldEmotion=emotion;
		emotion=0;
		faceDown=true;
		update(lcdRect(3));
	}

	if(!gameOver)
		 if(curX>=leftEdge&&curX<=rightEdge&&curY>=upEdge
			&&curY<=bottomEdge)
	{
		int mX=curX-leftEdge,mY=curY-upEdge;
		int mRow=mY/16,mCol=mX/16;
		int i=mRow,j=mCol;

		initMN(i,j);
		if(mRow!=startRow||mCol!=startCol)
			if(buttonstate>=3)
			{
				releaseLRButton(startRow,startCol);
				pressLRButton(i,j);
			}
			else if(buttonstate==1)
			{
				releaseLeftButton(startRow,startCol);
				pressLeftButton(i,j);
			}
/*		switch(e->state()&MouseButtonMask)		//????
		{
			case LeftButton:
				if(!disableLeft)
				{
					releaseLeftButton(startRow,startCol);
					pressLeftButton(i,j);
				}
				break;
			case MidButton:
			case LeftButton+RightButton:
			default:
				releaseLRButton(startRow,startCol);
				pressLRButton(i,j);
				break;
//			default:
//				;
		}
*/
	}
	else if(startRow!=-1&&startCol!=-1)  //mouse move out from minefield, recover the edge squares.
	{
		releaseLRButton(startRow,startCol);
		startRow=-1,startCol=-1;
	}
}

void MineField::pressLeftButton(int i,int j)
{
	if(flag[i][j]==0||flag[i][j]==2)
	{
		if(flag[i][j]==0)
			flag[i][j]=-2;
		else
			flag[i][j]=-3;
		update(minesRect(i,j));
		if(emotion==4)
		{
			emotion=3;
			update(lcdRect(3));
		}
	}
	startRow=i,startCol=j;
}

void MineField::releaseLeftButton(int i,int j)
{
	if(flag[i][j]==-2||flag[i][j]==-3)
	{
		if(flag[i][j]==-2)
			flag[i][j]=0;
		else
			flag[i][j]=2;
		update(minesRect(i,j));
	}
}

void MineField::pressLRButton(int i,int j)
{
	initMN(i,j);
	if(flag[i][j]!=1)
	{
		for(m=t;m<=b;m++)
			for(n=l;n<=r;n++)
				if(flag[m][n]==0||flag[m][n]==2)
				{
					if(flag[m][n]==0)
						flag[m][n]=-2;
					else
						flag[m][n]=-3;
					update(minesRect(m,n));
					if(emotion==4)
					{
						emotion=3;
						update(lcdRect(3));
					}
				}
		startRow=i,startCol=j;
	}
}

void MineField::releaseLRButton(int i,int j)
{
	initMN(i,j);
	for(m=t;m<=b;m++)
		for(n=l;n<=r;n++)
		{
			if(flag[m][n]==-2||flag[m][n]==-3)
			{
				if(flag[m][n]==-2)
					flag[m][n]=0;
				else
					flag[m][n]=2;
				update(minesRect(m,n));
			}
		}
//	disableLeft=true;
}

