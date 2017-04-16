QT       += core gui widgets

TARGET   = SpproContest15Visualizer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
			mapfileview.cpp \
			qcustomvariantanimator.cpp \
			qmapviewwidget.cpp \
			spprocontest15visualizer.cpp

HEADERS += mapfileview.h \
			qcustomvariantanimator.h \
			qmapviewwidget.h \
			spprocontest15visualizer.h

FORMS   += mapfileview.ui \
			spprocontest15visualizer.ui

RESOURCES += spprocontest15visualizer.qrc

win32:RC_FILE += SpproContest15Visualizer.rc
