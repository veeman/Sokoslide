#ifndef MAPFILE_H
#define MAPFILE_H

#include <QWidget>
#include <QCloseEvent>
#include <QPixmap>
#include <QGraphicsScene>
#include "ui_mapfileview.h"

class MapFileView : public QWidget
{
		Q_OBJECT

	public:
		MapFileView(QWidget *parent = 0, const QString &fileName = QString(), const QStringList &maps = QStringList());
		~MapFileView();

		QString getSaveFileName() const;
		void showMap(int index);
		bool isEdited() const;

		static MapFileView* open(const QString &fileName);
		static void initImages();
		static void freeImages();

	protected:
		void closeEvent(QCloseEvent *event);
		void applyMapChanges(int index);

	private:
		Ui::MapFileView ui;
		QString saveFileName;
		QStringList mapList;
		bool fileEdited;
		bool mapEdited;

		static QPixmap *pixmapIce;
		static QPixmap *pixmapWall;
		static QPixmap *pixmapTokenRed;
		static QPixmap *pixmapTokenGreen;
		static QPixmap *pixmapTokenBlue;
		static QPixmap *pixmapDestRed;
		static QPixmap *pixmapDestGreen;
		static QPixmap *pixmapDestBlue;

	public slots:
		void save();
		void saveAs();

		void on_pushButtonAdd_clicked();
		void on_pushButtonDelete_clicked();
		void on_pushButtonResetEdit_clicked();
		void on_pushButtonGameReset_clicked();
		void on_buttonGroup_buttonClicked(int id);
		void on_comboBox_activated(int index);
		void on_lineEditOptimumTurnCount_textEdited(const QString &text);
		void on_listWidgetMapList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
		void on_mapWidget_turnChanged(int index);
		void on_mapWidget_mapEdited();
};

inline QString MapFileView::getSaveFileName() const
{ return saveFileName; }

inline bool MapFileView::isEdited() const
{ return fileEdited; }

#endif // MAPFILE_H
