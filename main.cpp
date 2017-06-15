#pragma once
#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QHBoxLayout>
#include "earthWidget.h"
#include <QDialog>
#include <QMainWindow>
#include <QMimeData>
#include <QMdiSubWindow>
#include <QMdiArea>
#include "earthwnd.h"
#include <IGlobalCtrl.h>
#include <QTextCodec>
#include <QDir>

#ifdef GlobalCtrlDll
#define xxx main
#else
#define xxx main
#endif
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QWidget* widget = new QWidget;
	widget->setFixedSize(1620, 847);
	GetEarthWidget((void*)widget);
	widget->show();

	return a.exec();
}

