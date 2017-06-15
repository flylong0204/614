#ifndef EARTHWND_H
#define EARTHWND_H

#include <QMainWindow>
#include <QtCore/QCoreApplication>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include "earthWidget.h"
#include <QDialog>
#include <QMimeData>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QCheckBox>
#include <QButtonGroup>
#include <QToolBar>
#include <QRadioButton>
#include <QPushButton>
#include <QComboBox>
#include <iostream>
#include <QTimer>
#include "global.h"

class earthWnd : public QWidget
{
	Q_OBJECT

public:
	earthWnd(QWidget *parent = NULL);
	~earthWnd();

	void addUAV(string name, QVector<STRPoint> _vecPoint);
	void addShip(string name, QVector<STRPoint> _vecPoint);
	void addVehicle(string name, QVector<STRPoint> _vecPoint);

	
	void delUAV(string name);
	void delShip(string name);
	void delVehicle(string name);

	void delAllUAV();
	void delAllShip();
	void delAllVehicle();

	void updateUAVPosition(string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed);
	void updateShipPosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed);
	void updateVehiclePosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed);

	void createTrail(int _target, QVector<STRPoint> _vecPoint);

protected:
	void createToolBars();
	void createWidgets();
	void connectWidgets();
	void delComboItem(QComboBox* combox, string name);
	void setEarthTrack(int _type, string name);

public slots:

	void slot_test();
	void slot_makeDataTest();
	void slot_addUAV();
	void slot_addShip();
	void slot_addVehicle();
	void slot_addTrail();
	void slot_X();
	void slot_Y();
	void slot_Z();
	void slot_X1();
	void slot_Y1();
	void slot_Z1();
	void slot_UAVComBoChanged(QString _name);
	void slot_ShipComBoChanged(QString _name);
	void slot_VehicleComBoChanged(QString _name);
	void slot_setTrackObject(int);

private:
	
	earthWidget *earth;
	QMdiArea* mdiArea;
	QToolBar* toolBar;
	QCheckBox* isfollow;
	QCheckBox* OceanWnd;
	QRadioButton* rBtnPlane, *rBtnShip, *rBtnVehicle;
	QComboBox* ComboUAV, *ComboShip, *ComboVehicle;
	QButtonGroup* btnGroup;
	QHBoxLayout*tooBarLayout;
	QVBoxLayout * mainLayout;
	QPushButton* addbtn, *subbtn, *playbtn, *addUAVBtn, *addShipBtn, *addVehicleBtn, *addTrailBtn, *xBtn, *yBtn, *zBtn, *x1Btn, *y1Btn, *z1Btn;
	QTimer* timer;
public slots:
	
signals:
	//void stateChanged(int state);
};

#endif // EARTHWND_H
