#ifndef QMAPVIEWWIDGET_H
#define QMAPVIEWWIDGET_H

#include <QWidget>
#include <QPixmap>

class QMapViewWidget: public QWidget
{
		Q_OBJECT

	public:
		QMapViewWidget(QWidget *parent);
		~QMapViewWidget();

		void setMap(const QString &str);
		QString getMap() const;

		int getMode() const;
		void setMode(int mode);

		int getSelectedItem() const;
		void setSelectedItem(int selection);

		int getTurnCount() const;

	protected:
		virtual void paintEvent(QPaintEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);

		void calculateSelDirArrow(const QPoint &pStart, qreal pAngle, qreal pOffset = 12, qreal pLength = 22, qreal pArrowAngle = 40, qreal pArrowLength = 10);

	signals:
		void turnChanged(int count);
		void mapEdited();

	public slots:
		void performAnimation(const QVariant& position);
		void performAnimationFinished();
	private:
		void redrawMap(int x=0, int y=0, int w=16, int h=16);
		QPoint findMapItem(char c);

		int pMode;
		int pSelectedItem;
		int pTurnCount;
		int pRectSize;
		char pMap[16][16];
		
		QColor pColorWall;
		QColor pColorIce;
		QPixmap pBackBuffer;

		QPointF pNewToken;
		char    pNewTokenType;

		QPointF pArrow[4];
		QPoint pSelToken;
		char pSelTokenType;
		char pSelDir;

		bool pIsAnimationRun;
};

inline int QMapViewWidget::getMode() const
{ return pMode; }

inline QString QMapViewWidget::getMap() const
{ return QByteArray((char*)pMap, sizeof(pMap)/sizeof(char)); }

inline int QMapViewWidget::getSelectedItem() const
{ return pSelectedItem; }

inline int QMapViewWidget::getTurnCount() const
{ return pTurnCount; }

#endif // QMAPVIEWWIDGET_H
