#define _USE_MATH_DEFINES
#include "qmapviewwidget.h"
#include "qcustomvariantanimator.h"
#include <cmath>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>


char getTokenType(char c) {
	switch (c) {
		case 'm':
		case 'u':
		case 'x':
			c = 'r';
		break;
		case 'n':
		case 'v':
		case 'y':
			c = 'g';
		break;		
		case 'o':
		case 'w':
		case 'z':
			c = 'b';
		break;
		default:
			break;
	}
	return c;
}

char getDestType(char c) {
	switch (c) {
		case 'm':
		case 'n':
		case 'o':
			c = 'R';
		break;
		case 'u':
		case 'v':
		case 'w':
			c = 'G';
		break;		
		case 'x':
		case 'y':
		case 'z':
			c = 'B';
		break;
		default:
			c = '.';
			break;
	}
	return c;
}

void setTokenType(char &c, char token) {
	switch (c) {
		case 'R':
			switch (token) {
				case 'r':
					token = 'm';
					break;
				case 'g':
					token = 'n';
					break;
				case 'b':
					token = 'o';
					break;
				default:
					break;
			}
			break;
		case 'G':
			switch (token) {
				case 'r':
					token = 'u';
					break;
				case 'g':
					token = 'v';
					break;
				case 'b':
					token = 'w';
					break;
				default:
					break;
			}
			break;
		case 'B':
			switch (token) {
				case 'r':
					token = 'x';
					break;
				case 'g':
					token = 'y';
					break;
				case 'b':
					token = 'z';
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	c = token;
}

inline bool isFiledMoveable(char c) {
	c = getTokenType(c);
	return (c == '.') || (c == 'R') || (c == 'G') || (c == 'B'); 
}

QPoint getEndPointOfMovement(char map[16][16], QPoint curPos, int dir) {
	int mx = 0;
	int my = 0;

	if (dir==1)
		mx = 1;
	else
	if (dir==3)
		mx = -1;

	if (dir==2)
		my = -1;
	else
	if (dir==4)
		my = 1;

	if ((mx==0) && (my==0))
		return QPoint(-1,-1);

	do {
		curPos.setX(curPos.x()+mx);
		curPos.setY(curPos.y()+my);
	} while (isFiledMoveable(map[curPos.y()][curPos.x()]));
		
	curPos.setX(curPos.x()-mx);
	curPos.setY(curPos.y()-my);
	return curPos;
}

void drawElement(QPainter &painter, QRect &paintRect, char type) {
	switch (type) {
		case 'r':
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setBrush(QBrush("#C00000"));
			paintRect.adjust(1,1,-1,-1);
			painter.drawEllipse(paintRect);
			paintRect.adjust(-1,-1,1,1);
			break;
		case 'g':
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setBrush(QBrush("#00C000"));
			paintRect.adjust(1,1,-1,-1);
			painter.drawEllipse(paintRect);
			paintRect.adjust(-1,-1,1,1);
			break;
		case 'b':
			painter.setRenderHint(QPainter::Antialiasing);
			painter.setBrush(QBrush("#0000C0"));
			paintRect.adjust(1,1,-1,-1);
			painter.drawEllipse(paintRect);
			paintRect.adjust(-1,-1,1,1);
			break;
		case 'R':
			painter.setBrush(QBrush("#C00000"));
			painter.drawRect(paintRect);
			break;
		case 'G':
			painter.setBrush(QBrush("#00C000"));
			painter.drawRect(paintRect);
			break;
		case 'B':
			painter.setBrush(QBrush("#0000C0"));
			painter.drawRect(paintRect);
			break;
		default:
			break;
	}
}

QMapViewWidget::QMapViewWidget(QWidget *parent)
	: QWidget(parent), pMode(0), pSelectedItem(-1), pTurnCount(0), pRectSize(24), pNewTokenType(0), pIsAnimationRun(false)
{
	pBackBuffer = QPixmap(16*pRectSize, 16*pRectSize);
	pColorWall = QColor("#006b6b");
	pColorIce = QColor("#cfe7f5");

	setMinimumSize(16*pRectSize+2, 16*pRectSize+2);
}

QMapViewWidget::~QMapViewWidget()
{

}

void QMapViewWidget::setMap(const QString &str) {
	QByteArray asc = str.toLatin1();
	memcpy(pMap, asc.data(), sizeof(pMap)/sizeof(char));
	pTurnCount = 0;
	emit turnChanged(pTurnCount);

	pSelDir = 0;
	pSelTokenType = 0;
	pNewTokenType = 0;
	pIsAnimationRun = false;

	redrawMap();
	update();
}

void QMapViewWidget::setMode(int mode) {
	pMode = mode;
	pTurnCount = 0;
}

void QMapViewWidget::setSelectedItem(int selection) {
	pSelectedItem = selection;
}

void QMapViewWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	painter.setBrush(QBrush(Qt::white));
	painter.setPen(QPen(Qt::black));
	painter.drawRect(0,0,width()-1, height()-1);

	QRect rect = pBackBuffer.rect();
	rect.moveCenter(QPoint(width()/2-1, height()/2-1));
	painter.drawPixmap(rect.topLeft(), pBackBuffer);

	if (pNewTokenType) {
		QRect paintRect(pNewToken.x()*pRectSize + rect.left(), pNewToken.y()*pRectSize + rect.top(), pRectSize-1, pRectSize-1);

		switch (pNewTokenType) {
				case 'r':
					painter.setRenderHint(QPainter::Antialiasing);
					painter.setBrush(QBrush("#FF0000"));
					paintRect.adjust(1,1,-1,-1);
					painter.drawEllipse(paintRect);
					break;
				case 'g':
					painter.setRenderHint(QPainter::Antialiasing);
					painter.setBrush(QBrush("#00FF00"));
					paintRect.adjust(1,1,-1,-1);
					painter.drawEllipse(paintRect);
					break;
				case 'b':
					painter.setRenderHint(QPainter::Antialiasing);
					painter.setBrush(QBrush("#0000FF"));
					paintRect.adjust(1,1,-1,-1);
					painter.drawEllipse(paintRect);
					break;
				case 'R':
					painter.setBrush(QBrush("#FF0000"));
					painter.drawRect(paintRect);
					break;
				case 'G':
					painter.setBrush(QBrush("#00FF00"));
					painter.drawRect(paintRect);
					break;
				case 'B':
					painter.setBrush(QBrush("#0000FF"));
					painter.drawRect(paintRect);
					break;
				default:
					break;
			}
	}

	if (pSelDir) {
		QPen pen(Qt::black);
		pen.setWidth(3);

		painter.setRenderHint(QPainter::Antialiasing);
		painter.setPen(pen);
		painter.setBrush(QBrush(Qt::black));
		painter.drawLine(pArrow[0], pArrow[1]);
		painter.drawPolygon(&pArrow[1], 3);
	}
}

void QMapViewWidget::mouseMoveEvent(QMouseEvent *event) {
	switch (pMode) {
		case 0: {
				if (pSelectedItem >= -1)
					return;

				QRect rect = pBackBuffer.rect();
				rect.moveCenter(QPoint(width()/2-1, height()/2-1));
				if (rect.contains(event->pos())) {
					QPoint scenePos = event->pos() - rect.topLeft();
					scenePos.setX(scenePos.x() / pRectSize);
					scenePos.setY(scenePos.y() / pRectSize);

					switch (pSelectedItem) {
					case -2: // ice
						if (!QChar(pMap[scenePos.y()][scenePos.x()]).isLetter())
							pMap[scenePos.y()][scenePos.x()] = '.';
						redrawMap(scenePos.y(),scenePos.x(), 1, 1);
						break;
					case -3: // wall
						pMap[scenePos.y()][scenePos.x()] = '#';
						redrawMap(scenePos.y(),scenePos.x(), 1, 1);
						break;
					case -4: // red token
					case -5: // green token
					case -6: // blue token					
					case -7: // red dest
					case -8: // green dest
					case -9: // blue dest
						pNewToken = scenePos;
						break;
					default:
						break;
					}

					update();
					emit mapEdited();
				}
			}
			break;
		case 1: {
				if (pIsAnimationRun)
					return;

				QRect rect = pBackBuffer.rect();
				rect.moveCenter(QPoint(width()/2-1, height()/2-1));
				QPoint tokenMousePoint(pSelToken.x()*24+ 12 + rect.left(), pSelToken.y()*24 + 12 + rect.top());

				QLineF mouseVector(tokenMousePoint, event->pos());

				pNewTokenType = (mouseVector.length() <= 12) ? 0 : pSelTokenType;
			
				if (pNewTokenType)
					calculateSelDirArrow(tokenMousePoint, mouseVector.angle());
				else
					pSelDir = 0;


				update();
			}
			break;
		default:
			break;
	}
}

void QMapViewWidget::mousePressEvent(QMouseEvent *event) {

	switch (pMode) {
		case 0: {
				QRect rect = pBackBuffer.rect();
				rect.moveCenter(QPoint(width()/2-1, height()/2-1));
				if (rect.contains(event->pos())) {
					QPoint scenePos = event->pos() - rect.topLeft();
					scenePos.setX(scenePos.x() / pRectSize);
					scenePos.setY(scenePos.y() / pRectSize);

					QPoint pos;
					switch (pSelectedItem) {
						case -4: // red token
							pos = findMapItem('r');
							pNewTokenType = 'r';
							break;
						case -5: // green token
							pos = findMapItem('g');
							pNewTokenType = 'g';
							break;
						case -6: // blue token
							pos = findMapItem('b');
							pNewTokenType = 'b';
							break;					
						case -7: // red dest
							pos = findMapItem('R');
							pNewTokenType = 'R';
							break;
						case -8: // green dest
							pos = findMapItem('G');
							pNewTokenType = 'G';
							break;
						case -9: // blue dest
							pos = findMapItem('B');
							pNewTokenType = 'B';
							break;
						default:
							pos = QPoint(-1, -1);
							pNewTokenType = 0;
							break;
					}

					if ((pos.x() != -1) && (pos.y() != -1))
						pMap[pos.y()][pos.x()] = '.';

					redrawMap();
					update();
				}
				mouseMoveEvent(event);
			}
			break;
		case 1: {
				if (pIsAnimationRun)
					return;

				QRect rect = pBackBuffer.rect();
				rect.moveCenter(QPoint(width()/2-1, height()/2-1));
				if (rect.contains(event->pos())) {
					
					QPoint scenePos = event->pos() - rect.topLeft();
					scenePos.setX(scenePos.x() / pRectSize);
					scenePos.setY(scenePos.y() / pRectSize);

					char c = getTokenType(pMap[scenePos.y()][scenePos.x()]);
					if ( (c == 'r') || (c == 'g') || (c == 'b') ) {
						pSelToken = scenePos;
						pSelTokenType = c;

						pNewToken = scenePos;
						pNewTokenType = c;

						pSelDir = 0;
						update();
					}
					else
						pSelTokenType = 0;
				}
			}
			break;
		default:
			break;
	}
}

void QMapViewWidget::mouseReleaseEvent(QMouseEvent *event) {
	mouseMoveEvent(event);

	switch (pMode) {
		case 0: {
			if (pNewTokenType) {
				pMap[(int)pNewToken.y()][(int)pNewToken.x()] = pNewTokenType;
				pNewTokenType = 0;
				redrawMap();
				update();
			}
		}
			break;
		case 1: {
			if (pSelDir) {				
				pNewTokenType = getTokenType(pMap[pSelToken.y()][pSelToken.x()]);
				pNewToken = pSelToken;
				pMap[pSelToken.y()][pSelToken.x()] = getDestType(pMap[pSelToken.y()][pSelToken.x()]);

				pSelToken = getEndPointOfMovement(pMap, pNewToken.toPoint(), pSelDir);
				
				QCustomVariantAnimator *animation = new QCustomVariantAnimator();
				animation->setStartValue(pNewToken);
				animation->setEndValue(pSelToken);
				animation->setDuration(500);
				animation->setEasingCurve(QEasingCurve::InOutCubic);
				connect(animation, SIGNAL(valueChanged(const QVariant&)), this, SLOT(performAnimation(const QVariant&)));
				connect(animation, SIGNAL(finished()), this, SLOT(performAnimationFinished()));
				pIsAnimationRun = true;
				animation->start(QAbstractAnimation::DeleteWhenStopped);
				redrawMap();
			}
			else {
				pNewTokenType = 0;
			}

			pSelDir = 0;
			update();
		}
			break;
		default:
			break;
	}
}

void QMapViewWidget::calculateSelDirArrow(const QPoint &pStart, qreal pAngle, qreal pOffset, qreal pLength, qreal pArrowAngle, qreal pArrowLength) {
	
	if ((pAngle < 45.0) || (pAngle > 315.0))	{
		pSelDir = 1;

		char c = pMap[pSelToken.y()][pSelToken.x()+1];
		if ((pSelToken.x() == 15) || !isFiledMoveable(c)) {
			pSelDir = 0;
			return;
		}
	}
	else
	if (pAngle < 135.0) {
		pSelDir = 2;

		char c = pMap[pSelToken.y()-1][pSelToken.x()];
		if ((pSelToken.y() == 0) || !isFiledMoveable(c)) {
			pSelDir = 0;
			return;
		}
	}
	else
	if (pAngle < 225.0) {	
		pSelDir = 3;

		char c = pMap[pSelToken.y()][pSelToken.x()-1];
		if ((pSelToken.x() == 0) || !isFiledMoveable(c)) {
			pSelDir = 0;
			return;
		}
	}
	else  {
		pSelDir = 4;

		char c = pMap[pSelToken.y()+1][pSelToken.x()];
		if ((pSelToken.y() == 15) || !isFiledMoveable(c)) {
			pSelDir = 0;
			return;
		}
	}
	
	pAngle += 135.0;
	pAngle  = int(pAngle / 90) * 90;

	qreal xAlpha = sin(pAngle * M_PI / 180.0);
	qreal yAlpha = cos(pAngle * M_PI / 180.0);

	pArrow[0].setX(xAlpha * pOffset + pStart.x());
	pArrow[0].setY(yAlpha * pOffset + pStart.y());

	pArrow[1].setX(xAlpha * (pOffset + pLength) + pStart.x());
	pArrow[1].setY(yAlpha * (pOffset + pLength) + pStart.y());

	qreal tmpAngle = pAngle + (pArrowAngle / 2);
	xAlpha = sin(tmpAngle * M_PI / 180.0);
	yAlpha = cos(tmpAngle * M_PI / 180.0);

	pArrow[2].setX(pArrow[1].x() - xAlpha * pArrowLength);
	pArrow[2].setY(pArrow[1].y() - yAlpha * pArrowLength);

	tmpAngle = pAngle - (pArrowAngle / 2);
	xAlpha = sin(tmpAngle * M_PI / 180.0);
	yAlpha = cos(tmpAngle * M_PI / 180.0);

	pArrow[3].setX(pArrow[1].x() - xAlpha * pArrowLength);
	pArrow[3].setY(pArrow[1].y() - yAlpha * pArrowLength);
}

void QMapViewWidget::performAnimation(const QVariant& position) {

	pNewToken = position.toPointF();
	update();
}

void QMapViewWidget::performAnimationFinished() {
	++pTurnCount;
	setTokenType(pMap[pSelToken.y()][pSelToken.x()], pNewTokenType);
	pNewTokenType = 0;
	pSelTokenType = 0;
				
	{
		QPoint r = findMapItem('r');
		QPoint g = findMapItem('g');
		QPoint b = findMapItem('b');

		if (!((r.x() == -1) && (r.y() == -1))) {
			char cR = getDestType(pMap[r.y()][r.x()]);
			if (cR != 'R')
				pIsAnimationRun = false;
		}
		if (!((g.x() == -1) && (g.y() == -1))) {
			char cG = getDestType(pMap[g.y()][g.x()]);
			if (cG != 'G')
				pIsAnimationRun = false;
		}
		if (!((b.x() == -1) && (b.y() == -1))) {
			char cB = getDestType(pMap[b.y()][b.x()]);
			if (cB != 'B')
				pIsAnimationRun = false;
		}
		
		if (pIsAnimationRun)
			emit turnChanged(-pTurnCount);
		else
			emit turnChanged(pTurnCount);
	}
	redrawMap();
	update();
}

void QMapViewWidget::redrawMap(int x, int y, int w, int h) {
	int ex = x+w;
	int ey = y+h;

	QPainter painter(&pBackBuffer);
	for (int ix=x; ix<ex; ++ix) {
		for (int iy=y; iy<ey; ++iy) {
			char c = pMap[ix][iy];
			QRect paintRect(iy*pRectSize, ix*pRectSize, pRectSize-1, pRectSize-1);

			painter.setRenderHint(QPainter::Antialiasing, false);
			if (c=='#') {
				painter.setBrush(QBrush(pColorWall));
				painter.setPen(QPen(pColorWall));
			}
			else {
				painter.setBrush(QBrush(pColorIce));
				painter.setPen(QPen(pColorIce));
			}

			painter.drawRect(paintRect);

			painter.setPen(QPen(Qt::black));
			switch (c) {
				case 'r':
				case 'g':
				case 'b':
				case 'R':
				case 'G':
				case 'B':
					drawElement(painter, paintRect, c);
					break;
				case 'm':
					drawElement(painter, paintRect, 'R');
					drawElement(painter, paintRect, 'r');
					break;
				case 'n':
					drawElement(painter, paintRect, 'R');
					drawElement(painter, paintRect, 'g');
					break;
				case 'o':
					drawElement(painter, paintRect, 'R');
					drawElement(painter, paintRect, 'b');
					break;	

				case 'u':
					drawElement(painter, paintRect, 'G');
					drawElement(painter, paintRect, 'r');
					break;
				case 'v':
					drawElement(painter, paintRect, 'G');
					drawElement(painter, paintRect, 'g');
					break;
				case 'w':
					drawElement(painter, paintRect, 'G');
					drawElement(painter, paintRect, 'b');
					break;

				case 'x':
					drawElement(painter, paintRect, 'B');
					drawElement(painter, paintRect, 'r');
					break;
				case 'y':
					drawElement(painter, paintRect, 'B');
					drawElement(painter, paintRect, 'g');
					break;
				case 'z':
					drawElement(painter, paintRect, 'B');
					drawElement(painter, paintRect, 'b');
					break;
				default:
					break;
			}
		}
	}
}

QPoint QMapViewWidget::findMapItem(char c) {
	for (int ix=0; ix<16; ++ix) {
		for (int iy=0; iy<16; ++iy) {
			if (getTokenType(pMap[iy][ix]) == c)
				return QPoint(ix,iy);
		}
	}
	return QPoint(-1,-1);
}