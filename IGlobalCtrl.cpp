#include "IGlobalCtrl.h"
#include <earthwnd.h>
#include <QApplication>
#include <QMessageBox>

int i = 0;
//QApplication a(i, NULL);

earthWnd* earthwnd = nullptr;

earthWnd* GetEarthWidget(void* ptr)
{
	if (!earthwnd)
		//earthwnd = new earthWnd((QWidget*)ptr);
		earthwnd = new earthWnd((QWidget*)ptr);
	return earthwnd;
}

void addUAV(string name, QVector<STRPoint> _vecPoint)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->addUAV(name, _vecPoint);
}

void addShip(string name, QVector<STRPoint> _vecPoint)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->addShip(name, _vecPoint);
}

void addVehicle(string name, QVector<STRPoint> _vecPoint)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->addVehicle(name, _vecPoint);
}

void updateUAVPos(string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->updateUAVPosition(name, _long, _lat, _altitude, _heading, _pitch, _roll, _speed);
}

void updateShipPos(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->updateShipPosition(name, _long, _lat, _altitude, _heading, _speed);
}

void updateVehiclePos(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->updateVehiclePosition(name, _long, _lat, _altitude, _heading, _speed);
}

GlobalCtrlApi void createTrail(int _target, QVector<STRPoint> _vecPoint)
{
	if (!checkptr(earthwnd))
		return;
	earthwnd->createTrail(_target, _vecPoint);
}

bool checkptr(void* ptr)
{
	if (NULL == ptr)
	{
		QMessageBox::about(NULL, "Error", "The earth control was not created !");
		return false;
	}
	return true;
}

GlobalCtrlApi void delUAV(string name)
{
	earthwnd->delUAV(name);
}

GlobalCtrlApi void delShip(string name)
{
	earthwnd->delShip(name);
}

GlobalCtrlApi void delVehicle(string name)
{
	earthwnd->delVehicle(name);
}

GlobalCtrlApi void delAllUAV()
{
	earthwnd->delAllUAV();
}

GlobalCtrlApi void delAllShip()
{
	earthwnd->delAllShip();
}

GlobalCtrlApi void delAllVehicle()
{
	earthwnd->delAllVehicle();
}


