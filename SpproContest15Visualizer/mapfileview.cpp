#include "mapfileview.h"
#include <QTabWidget>
#include <QFile>
#include <QIcon>
#include <QPainter>
#include <QBitmap>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <qdebug.h>


QPixmap *MapFileView::pixmapIce = 0;
QPixmap *MapFileView::pixmapWall = 0;
QPixmap *MapFileView::pixmapTokenRed = 0;
QPixmap *MapFileView::pixmapTokenGreen = 0;
QPixmap *MapFileView::pixmapTokenBlue = 0;
QPixmap *MapFileView::pixmapDestRed = 0;
QPixmap *MapFileView::pixmapDestGreen = 0;
QPixmap *MapFileView::pixmapDestBlue = 0;

MapFileView::MapFileView(QWidget *parent, const QString &fileName, const QStringList &maps)
	: QWidget(parent), saveFileName(fileName), mapList(maps), fileEdited(false), mapEdited(false)
{
	ui.setupUi(this);

	ui.pushButtonItemIce		->setIcon(QIcon(*pixmapIce));
	ui.pushButtonItemWall		->setIcon(QIcon(*pixmapWall));
	ui.pushButtonItemSrcRed		->setIcon(QIcon(*pixmapTokenRed));
	ui.pushButtonItemSrcGreen	->setIcon(QIcon(*pixmapTokenGreen));
	ui.pushButtonItemSrcBlue	->setIcon(QIcon(*pixmapTokenBlue));
	ui.pushButtonItemDestRed	->setIcon(QIcon(*pixmapDestRed));
	ui.pushButtonItemDestGreen	->setIcon(QIcon(*pixmapDestGreen));
	ui.pushButtonItemDestBlue	->setIcon(QIcon(*pixmapDestBlue));

	for (int i=1; i<=mapList.count(); ++i)
		ui.listWidgetMapList->addItem(new QListWidgetItem(QString::number(i), ui.listWidgetMapList));
	
	if (!mapList.count())
		on_pushButtonAdd_clicked();

	ui.listWidgetMapList->setCurrentRow(0);
	ui.comboBox->setCurrentIndex(0);
}

MapFileView::~MapFileView()
{

}

MapFileView* MapFileView::open(const QString &fileName) {

	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QString data(file.readAll());
		file.close();

		QStringList lines = data.split('\n');
		QStringList maps;
		QString map;

		int lineNr = 0;
		foreach (const QString line, lines ) {
			QString tmp = line.trimmed();

			if ((lineNr % 17) < 16) {
				if (tmp.length() != 16)
					return 0;

				for (int i=0; i<16; ++i) {
					switch (tmp.at(i).toAscii())
					{
						case '#':case '.':case 'r':case 'g':case 'b':case 'R':case 'G':case 'B':
						break;
						default:
							return 0;
					}
				}

				map += tmp;
			}
			else {
				map += QString(" %1").arg(tmp.toInt());
				maps << map;
				map.clear();
			}

			lineNr++;
		}

		MapFileView *view = new MapFileView(0, fileName, maps);
		return view;
	}

	return 0;
}

void MapFileView::initImages() {
	MapFileView::pixmapIce			= new QPixmap(24, 24);
	MapFileView::pixmapWall			= new QPixmap(24, 24);
	MapFileView::pixmapTokenRed		= new QPixmap(24, 24);
	MapFileView::pixmapTokenGreen	= new QPixmap(24, 24);
	MapFileView::pixmapTokenBlue	= new QPixmap(24, 24);
	MapFileView::pixmapDestRed		= new QPixmap(24, 24);
	MapFileView::pixmapDestGreen	= new QPixmap(24, 24);
	MapFileView::pixmapDestBlue		= new QPixmap(24, 24);

	QPainter painter;

	painter.begin(MapFileView::pixmapIce);
	painter.setBrush(QBrush("#cfe7f5"));
	painter.drawRect(0,0,23,23);
	painter.end();

	painter.begin(MapFileView::pixmapWall);
	painter.setBrush(QBrush("#006b6b"));
	painter.drawRect(0,0,23,23);
	painter.end();

	// draw tokens
	MapFileView::pixmapTokenRed->fill(Qt::transparent);
	painter.begin(MapFileView::pixmapTokenRed);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(QBrush("#c00000"));
	painter.drawEllipse(1,1,22,22);
	painter.end();

	MapFileView::pixmapTokenGreen->fill(Qt::transparent);
	painter.begin(MapFileView::pixmapTokenGreen);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(QBrush("#00c000"));
	painter.drawEllipse(1,1,22,22);
	painter.end();

	MapFileView::pixmapTokenBlue->fill(Qt::transparent);
	painter.begin(MapFileView::pixmapTokenBlue);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(QBrush("#0000c0"));
	painter.drawEllipse(1,1,22,22);
	painter.end();

	// draw destinations
	painter.begin(MapFileView::pixmapDestRed);
	painter.setBrush(QBrush("#c00000"));		
	painter.drawRect(0,0,23,23);
	painter.end();

	painter.begin(MapFileView::pixmapDestGreen);
	painter.setBrush(QBrush("#00c000"));		
	painter.drawRect(0,0,23,23);
	painter.end();

	painter.begin(MapFileView::pixmapDestBlue);
	painter.setBrush(QBrush("#0000c0"));		
	painter.drawRect(0,0,23,23);
	painter.end();
}

void MapFileView::freeImages() {
	delete MapFileView::pixmapIce;
	delete MapFileView::pixmapWall;
	delete MapFileView::pixmapTokenRed;
	delete MapFileView::pixmapTokenGreen;
	delete MapFileView::pixmapTokenBlue;
	delete MapFileView::pixmapDestRed;
	delete MapFileView::pixmapDestGreen;
	delete MapFileView::pixmapDestBlue;
}

void MapFileView::closeEvent(QCloseEvent *event)
{
	if (mapEdited)
		applyMapChanges(ui.listWidgetMapList->currentIndex().row());

	if (fileEdited) {
		QTabWidget *parentTabWidget = qobject_cast<QTabWidget*>(this->parent()->parent());
		QString title = parentTabWidget->tabText(parentTabWidget->indexOf(this));
		int i = QMessageBox::question(this, tr("Close"), QString(tr("Map '%1' was modified. Would you like to save the changes?")).arg(title), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort, QMessageBox::Discard);

		event->accept();
		if (i==QMessageBox::Save)
			save();
		else
		if (i==QMessageBox::Abort)
			event->ignore();
		else
			fileEdited = false;
	}

	if (event->isAccepted())
	{
		QTabWidget *widget = qobject_cast<QTabWidget*>(parent());
		if (widget)
			widget->removeTab(widget->indexOf(this));
		deleteLater();
	}
}


void MapFileView::applyMapChanges(int index) {
	int iNr = ui.lineEditOptimumTurnCount->text().trimmed().toInt();
	mapList[index] = ui.mapWidget->getMap() + " " + QString::number(iNr);
	fileEdited = true;
}

void MapFileView::save() {
	if (saveFileName.isEmpty()) {
		saveAs();
		return;
	}

	QString shortFileName = QFileInfo(saveFileName).fileName();
	QTabWidget *parentTabWidget = qobject_cast<QTabWidget*>(this->parent()->parent());
	parentTabWidget->setTabText(parentTabWidget->indexOf(this), shortFileName);

	QFile file(saveFileName);
	if (file.open(QFile::WriteOnly | QFile::Truncate))
	{
		foreach(const QString &string, mapList) {
			QStringList map = string.split(" ");

			for (int i=0; i<(map.at(0).size()/16); ++i) {
				QString str = map.at(0).mid(i*16, 16);
				file.write(str.toAscii(), 16);
				file.write("\n");
			}
			file.write(map.at(1).toAscii());
			if (mapList.last() != string)
				file.write("\n");
		}
		file.close();
		fileEdited = false;
	}
	else
		QMessageBox::warning(this, tr("Error"), QString(tr("Cann't save File '%1'.")).arg(shortFileName));
}

void MapFileView::saveAs() {
	QString fileName = QFileDialog::getSaveFileName(this, QString(), QString(), trUtf8("Text Files (*.txt);;All Files (*.*)"));
	if (fileName.isEmpty())
		return;

	saveFileName = fileName;
	save();
}

void MapFileView::showMap(int index) {
	if (index >= mapList.size())
		return;
	mapEdited = false;

	QStringList map = mapList.at(index).split(" ");
	ui.lineEditOptimumTurnCount->setText(map.at(1));
	ui.mapWidget->setMap(map.at(0));
	ui.mapWidget->setSelectedItem(ui.buttonGroup->checkedId());
}

void MapFileView::on_pushButtonAdd_clicked() {
	QString map;
	map += QString(16, '#');
	for (int i=0; i<14; ++i)
		map += '#' + QString(14, '.') + '#';
	map += QString(16, '#');
	map += " 0";

	ui.listWidgetMapList->addItem(QString::number(ui.listWidgetMapList->count()+1));
	mapList.push_back(map);
	mapEdited = true;
}

void MapFileView::on_pushButtonDelete_clicked() {
	if (ui.listWidgetMapList->count() <= 1)
		return;
	
	fileEdited = true;

	int i = ui.listWidgetMapList->currentRow();
	mapList.removeAt(i);

	ui.listWidgetMapList->setCurrentRow(0);

	delete ui.listWidgetMapList->takeItem(ui.listWidgetMapList->count()-1);
}

void MapFileView::on_pushButtonResetEdit_clicked() {
	int i = ui.listWidgetMapList->currentRow();
	if (i>=0)
		on_listWidgetMapList_currentItemChanged(ui.listWidgetMapList->item(i), 0);
}

void MapFileView::on_pushButtonGameReset_clicked() {
	on_pushButtonResetEdit_clicked();
}

void MapFileView::on_buttonGroup_buttonClicked(int id) {
	ui.mapWidget->setSelectedItem(id);
}

void MapFileView::on_comboBox_activated(int index) {
	int current = ui.listWidgetMapList->currentIndex().row();
	if (mapEdited)
		applyMapChanges(current);
	else
		showMap(current);

	ui.mapWidget->setMode(index);
}

void MapFileView::on_lineEditOptimumTurnCount_textEdited(const QString &text) {
	mapEdited = true;
}

void MapFileView::on_listWidgetMapList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
	if (previous && mapEdited)
		applyMapChanges(ui.listWidgetMapList->row(previous));

	showMap(ui.listWidgetMapList->row(current));
}

void MapFileView::on_mapWidget_turnChanged(int index) {
	ui.labelTurnCount->setText(QString(tr("TurnCount: %1/%2")).arg(abs(index)).arg(ui.lineEditOptimumTurnCount->text()));
	ui.labelDone->setText(index < 0 ? tr("Done") : QString());
}

void MapFileView::on_mapWidget_mapEdited() {
	mapEdited = true;
}