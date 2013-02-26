#include "spprocontest15visualizer.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>

const char *OPTIONS_FILE		= "./options.cfg";
const char *OPTION_RECENT_FILES = "recentFileList";
const int 	MAX_RECENT_FILES	= 5;

SpproContest15Visualizer::SpproContest15Visualizer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	loadRecentFileActions();
	MapFileView::initImages();
}

SpproContest15Visualizer::~SpproContest15Visualizer() {
	MapFileView::freeImages();
	saveRecentFileActions();
}

void SpproContest15Visualizer::closeEvent(QCloseEvent *event) {
	action_Close_All_triggered();

	for (int i=0; i<ui.tabWidgetOpenedFiles->count(); ++i) {
		if (qobject_cast<MapFileView*>(ui.tabWidgetOpenedFiles->widget(i))->isEdited()) {
			event->ignore();
			break;
		}
	}
}

void SpproContest15Visualizer::loadRecentFileActions() {
	QSettings settings(OPTIONS_FILE, QSettings::IniFormat);
	settings.beginGroup(OPTION_RECENT_FILES);
	QStringList files = settings.allKeys();

	int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

	QList<QAction *> recentFiles = ui.menu_Recent->actions();
	foreach(QAction *recent, recentFiles) {
		ui.menu_Recent->removeAction(recent);
		recent->deleteLater();
	}

	if (numRecentFiles) {
		for (int i=0; i<numRecentFiles; ++i) {
			QString fileName = settings.value(files.at(i)).toString();
			QFileInfo fileInfo(fileName);
			QAction *action = new QAction("../"+fileInfo.dir().dirName()+"/"+fileInfo.fileName(),this);
			action->setData(fileName);
			ui.menu_Recent->addAction(action);		
		}
	}
	else {
		QAction *action = new QAction(tr("No recent files"), this);
		ui.menu_Recent->addAction(action);
	}
}

void SpproContest15Visualizer::saveRecentFileActions() {
	QSettings settings(OPTIONS_FILE, QSettings::IniFormat);
	settings.beginGroup(OPTION_RECENT_FILES);
		QList<QAction *> recentFiles = ui.menu_Recent->actions();
		int i=0;
		foreach(QAction *recent, recentFiles) {
			QString fileName = recent->data().toString();
			if (!fileName.isEmpty())
				settings.setValue(QString::number(i++), fileName);
		}
	settings.endGroup();
}

void SpproContest15Visualizer::addRecentFileAction(const QString &fileName) {
	QList<QAction *> recentFiles = ui.menu_Recent->actions();
	foreach(QAction *recent, recentFiles) 
	{
		QString fileRecent = recent->data().toString();
		if (fileRecent.isEmpty() || (fileRecent == fileName))
		{
			ui.menu_Recent->removeAction(recent);
			recent->deleteLater();
			break;
		}
	}

	QFileInfo fileInfo(fileName);
	QAction *action = new QAction("../"+fileInfo.dir().dirName()+"/"+fileInfo.fileName(),this);
	action->setData(fileName);

	if (ui.menu_Recent->actions().count())
		ui.menu_Recent->insertAction(ui.menu_Recent->actions().at(0), action);
	else
		ui.menu_Recent->addAction(action);
}

void SpproContest15Visualizer::open(const QString &fileName, int *answer) {
	
	QList<MapFileView*> openedFiles = ui.tabWidgetOpenedFiles->findChildren<MapFileView*>();
	foreach(MapFileView *openedFile, openedFiles) {
		QString file = openedFile->getSaveFileName();
		if (file == fileName) {
			if (answer)
			{
				if (*answer == QMessageBox::NoAll)
					return;

				if (*answer == QMessageBox::YesAll)
				{
					openedFile->close();
					break;
				}
			}

			QMessageBox question(QMessageBox::Question, QString(), tr("File already opened. Reload File?"));
			question.addButton(QMessageBox::Yes);
			question.addButton(QMessageBox::YesAll);
			question.addButton(QMessageBox::No);
			question.addButton(QMessageBox::NoAll);
			question.addButton(QMessageBox::Abort);
			int i = question.exec();

			if (i == QMessageBox::No)
				return;

			if (i == QMessageBox::Yes) {
				openedFile->close();
				break;
			}

			if (answer)
				*answer = i;

			if ((i == QMessageBox::Abort) || (i == QMessageBox::NoAll))
				return;

			openedFile->close();
			break;
		}
	}
	
	MapFileView *widget = MapFileView::open(fileName);
	if (widget)
	{
		addRecentFileAction(fileName);
		
		widget->setParent(ui.tabWidgetOpenedFiles);
		ui.tabWidgetOpenedFiles->addTab(widget, QFileInfo(fileName).fileName());
		ui.statusBar->showMessage(QString(tr("File '%1' opened successfully.")).arg(fileName), 5000);
	}
	else
		ui.statusBar->showMessage(QString(tr("Error: Cann't open File '%1'")).arg(fileName), 5000);
}

void SpproContest15Visualizer::action_New_triggered() {
	MapFileView *widget = new MapFileView(ui.tabWidgetOpenedFiles);
	ui.tabWidgetOpenedFiles->addTab(widget, tr("new"));
}

void SpproContest15Visualizer::action_Open_triggered() {

	QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open MapFile"), QString(), trUtf8("Text Files (*.txt);;All Files (*.*)"));

	int answer = 0;
	foreach(QString fileName, fileNames) {
		open(fileName, &answer);

		if (answer == QMessageBox::Abort)
			break;
	}
}

void SpproContest15Visualizer::action_Save_triggered() {
	MapFileView *widget = qobject_cast<MapFileView*>(ui.tabWidgetOpenedFiles->currentWidget());
	if (widget)
		widget->save();
}

void SpproContest15Visualizer::action_Save_As_triggered() {
	MapFileView *widget = qobject_cast<MapFileView*>(ui.tabWidgetOpenedFiles->currentWidget());
	if (widget)
		widget->saveAs();
}

void SpproContest15Visualizer::action_Save_All_triggered() {
	for (int i=0; i<ui.tabWidgetOpenedFiles->count(); ++i) {
		MapFileView *widget = qobject_cast<MapFileView*>(ui.tabWidgetOpenedFiles->widget(i));
		if (widget)
			widget->save();
	}
}

void SpproContest15Visualizer::action_Close_triggered() {
	QWidget *widget = ui.tabWidgetOpenedFiles->currentWidget();
	if (widget)
		widget->close();
}

void SpproContest15Visualizer::action_Close_All_triggered() {
	for (int i=0; i<ui.tabWidgetOpenedFiles->count(); ++i) {
		ui.tabWidgetOpenedFiles->widget(i)->close();
	}
}

void SpproContest15Visualizer::action_Quit_triggered() {
	close();
}

void SpproContest15Visualizer::on_menu_Recent_triggered(QAction *action) {
	if (!action)
		return;

	QString recent = action->data().toString();
	if (!recent.isEmpty())
		open(recent);
}

void SpproContest15Visualizer::on_tabWidgetOpenedFiles_tabCloseRequested(int index) {
	QWidget *widget = ui.tabWidgetOpenedFiles->currentWidget();
	if (widget)
		widget->close();	
}
