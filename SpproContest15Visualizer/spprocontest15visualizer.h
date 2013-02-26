#ifndef SPPROCONTEST15VISUALIZER_H
#define SPPROCONTEST15VISUALIZER_H

#include <QtGui/QMainWindow>
#include "ui_spprocontest15visualizer.h"
#include "mapfileview.h"

class SpproContest15Visualizer : public QMainWindow
{
		Q_OBJECT

	public:
		SpproContest15Visualizer(QWidget *parent = 0, Qt::WFlags flags = 0);
		~SpproContest15Visualizer();

		void open(const QString &fileName, int *answer = 0);

	private:
		Ui::SpproContest15VisualizerClass ui;
		QStringList maps;

	protected:
		void closeEvent(QCloseEvent *event);

	public slots:
		void loadRecentFileActions();
		void saveRecentFileActions();
		void addRecentFileAction(const QString &fileName);
 
		void action_New_triggered();
		void action_Open_triggered();
		void action_Save_triggered();
		void action_Save_As_triggered();
		void action_Save_All_triggered();
		void action_Close_triggered();
		void action_Close_All_triggered();
		void action_Quit_triggered();

		void on_menu_Recent_triggered(QAction *action);
		void on_tabWidgetOpenedFiles_tabCloseRequested(int index);
};

#endif // SPPROCONTEST15VISUALIZER_H
