#include "spprocontest15visualizer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SpproContest15Visualizer w;
	w.show();
	return a.exec();
}
