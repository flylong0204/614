#include "earthwnd.h"
#include <QApplication>
#include "AdapterWidget.h"
#include "global.h"

double UAV_longitude = 108.93;
double UAV_latitude = 34.27;
double UAV_altitude = 6000;
double UAV_heading = 0.0;

double Ship_longitude = 108.96;
double Ship_latitude = 34.27;
double Ship_altitude = 550;
double Ship_heading = 0.0;

double Vehicle_longitude = 108.99;
double Vehicle_latitude = 34.27;
double Vehicle_altitude = 500;
double Vehicle_heading = 0.0;

earthWnd::earthWnd(QWidget *parent)
	: QWidget(parent)
{
	mainLayout = new QVBoxLayout;
	createToolBars();
	createWidgets();

	mainLayout->addLayout(tooBarLayout);
	mainLayout->addWidget(earth);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mainLayout);

	/*if (NULL == parent)
	{
	setFixedSize(1620, 847);
	}
	else
	{
	setFixedSize(parent->width(), parent->height());
	}*/
	

	connectWidgets();
	show();
}

earthWnd::~earthWnd()
{
	
}

void earthWnd::createToolBars()
{
	tooBarLayout = new QHBoxLayout;
	isfollow = new QCheckBox("Follow"); 
	isfollow->setFixedHeight(40);
	rBtnPlane = new QRadioButton("UAV");
	rBtnShip = new QRadioButton("Ship");
	rBtnVehicle = new QRadioButton("Vehicle");
	btnGroup = new QButtonGroup;
	btnGroup->addButton(rBtnPlane, 1);
	btnGroup->addButton(rBtnShip, 2);
	btnGroup->addButton(rBtnVehicle, 3);
	tooBarLayout->addWidget(isfollow);
	tooBarLayout->addWidget(rBtnPlane);
	tooBarLayout->addWidget(rBtnShip); 
	tooBarLayout->addWidget(rBtnVehicle);

	addbtn = new QPushButton(tr("+"));
	addbtn->setFixedSize(20,10);
	subbtn = new QPushButton(tr("-"));
	subbtn->setFixedSize(20, 10);
	playbtn = new QPushButton(tr("P"));
	playbtn->setFixedSize(20, 10);

	OceanWnd = new QCheckBox("Ocean");
	OceanWnd->setFixedHeight(40);

	addUAVBtn = new QPushButton(tr("ADDUAV"));
	addShipBtn = new QPushButton(tr("ADDShip"));
	addVehicleBtn = new QPushButton(tr("ADDVehicle"));
	addTrailBtn = new QPushButton(tr("ADDTrail"));
	xBtn = new QPushButton(tr("delUAV"));
	x1Btn = new QPushButton(tr("delShip"));

	yBtn = new QPushButton(tr("delVehicle"));
	y1Btn = new QPushButton(tr("delAllUAV"));

	zBtn = new QPushButton(tr("delAllShip"));
	z1Btn = new QPushButton(tr("delAllVehicle"));

	ComboUAV = new QComboBox;
	ComboShip = new QComboBox;
	ComboVehicle = new QComboBox;

	tooBarLayout->addWidget(ComboUAV);
	tooBarLayout->addWidget(ComboShip);
	tooBarLayout->addWidget(ComboVehicle);

	tooBarLayout->addWidget(addbtn);
	tooBarLayout->addWidget(subbtn);
	tooBarLayout->addWidget(playbtn);
	tooBarLayout->addWidget(addUAVBtn);
	tooBarLayout->addWidget(addShipBtn);
	tooBarLayout->addWidget(addVehicleBtn);
	tooBarLayout->addWidget(addTrailBtn);
	tooBarLayout->addWidget(xBtn);
	tooBarLayout->addWidget(x1Btn);
	tooBarLayout->addWidget(yBtn);
	tooBarLayout->addWidget(y1Btn);
	tooBarLayout->addWidget(zBtn);
	tooBarLayout->addWidget(z1Btn);
	tooBarLayout->addWidget(OceanWnd);
	tooBarLayout->addStretch();
}

void earthWnd::createWidgets()
{
	QApplication::setStyle("cleanlooks");
	earth = new earthWidget(this);
	earth->setFixedSize(1620, 880 - 40);
	setWindowTitle("Osg Eearth");
	mdiArea = new QMdiArea(this);

}

void earthWnd::connectWidgets()
{
	connect(isfollow, SIGNAL(clicked(bool)), earth, SLOT(slot_isFollowPlane(bool)));
	connect(OceanWnd, SIGNAL(clicked(bool)), earth, SLOT(slot_setOceanWndVisble(bool)));
	//connect(btnGroup, SIGNAL(buttonClicked(int)), earth, SLOT(slot_FollowOne(int)));
	connect(btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_setTrackObject(int)));
	connect(addbtn, SIGNAL(clicked()), earth, SLOT(slot_addInterval()));
	connect(subbtn, SIGNAL(clicked()), earth, SLOT(slot_subInterval()));
	//connect(playbtn, SIGNAL(clicked()), earth, SLOT(slot_startPlay()));
	connect(playbtn, SIGNAL(clicked()), this, SLOT(slot_test()));

	connect(addUAVBtn, SIGNAL(clicked()), SLOT(slot_addUAV()));
	connect(addShipBtn, SIGNAL(clicked()), SLOT(slot_addShip()));
	connect(addVehicleBtn, SIGNAL(clicked()), SLOT(slot_addVehicle()));
	connect(addTrailBtn, SIGNAL(clicked()), SLOT(slot_addTrail()));

	connect(xBtn, SIGNAL(clicked()), SLOT(slot_X()));
	connect(yBtn, SIGNAL(clicked()), SLOT(slot_Y()));
	connect(zBtn, SIGNAL(clicked()), SLOT(slot_Z()));
	connect(x1Btn, SIGNAL(clicked()), SLOT(slot_X1()));
	connect(y1Btn, SIGNAL(clicked()), SLOT(slot_Y1()));
	connect(z1Btn, SIGNAL(clicked()), SLOT(slot_Z1()));

	connect(ComboUAV, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_UAVComBoChanged(QString)));
	connect(ComboShip, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_ShipComBoChanged(QString)));
	connect(ComboVehicle, SIGNAL(currentIndexChanged(QString)), this, SLOT(slot_VehicleComBoChanged(QString)));

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_makeDataTest()));

}

void earthWnd::updateUAVPosition(string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed)
{
	earth->updateUAVPosition(name, osg::Vec3d(_long, _lat, _altitude), _heading, _pitch, _roll, _speed);
}

void earthWnd::addUAV(string name, QVector<STRPoint> _vecPoint)
{
	earth->addUAVWithoutTrail(name, _vecPoint);
	ComboUAV->addItem(QString::fromStdString(name));
}

void earthWnd::updateShipPosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	earth->updateShipPosition(name, _long, _lat, _altitude, _heading, _speed);
}

void earthWnd::addShip(string name, QVector<STRPoint> _vecPoint)
{
	earth->addShipByTrail(name, _vecPoint);
	ComboShip->addItem(QString::fromStdString(name));
}

void earthWnd::updateVehiclePosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	earth->updateVehiclePosition(name, _long, _lat, _altitude, _heading, _speed);
}

void earthWnd::addVehicle(string name, QVector<STRPoint> _vecPoint)
{
	earth->addVehicleByTrail(name, _vecPoint);
	ComboVehicle->addItem(QString::fromStdString(name));
}


void earthWnd::delUAV(string name)
{
	earth->delObject(1, name);
	delComboItem(ComboUAV, name);
}

void earthWnd::delShip(string name)
{
	earth->delObject(2, name);
	delComboItem(ComboShip, name);
}

void earthWnd::delVehicle(string name)
{
	earth->delObject(3, name);
	delComboItem(ComboVehicle, name);
}

void earthWnd::createTrail(int _target, QVector<STRPoint> _vecPoint)
{
	//earth->createTrail(_target + 1, _vecPoint);
}

void earthWnd::slot_test()
{
	if (timer->isActive())
	{
		timer->stop();
	}
	else
	{
		timer->start(10);
	}
}

static double heading = 0.0;

void earthWnd::slot_makeDataTest()
{
	if (heading < 55.0)
	{
		heading += 0.1;
	}
	UAV_longitude += 0.0001428;
	UAV_latitude += 0.000085166;
	UAV_altitude   = 6000;
	//G_STRPlane.heading += heading;
	updateUAVPosition("UAV0", UAV_longitude, UAV_latitude, UAV_altitude, heading, 0, 0, 80);
	updateUAVPosition("UAV1", UAV_longitude, UAV_latitude, UAV_altitude, heading, 0, 0, 0);

	Ship_longitude += 0.0001428;
	Ship_latitude += 0.000085166;
	//G_STRPlane.heading += heading;
	updateShipPosition("Ship0", Ship_longitude, Ship_latitude, Ship_altitude, heading, 60);

	Vehicle_longitude += 0.0001428;
	Vehicle_latitude += 0.000085166;

	updateVehiclePosition("Vehicle0", Vehicle_longitude, Vehicle_latitude, Vehicle_altitude, heading, 60);
}





void earthWnd::slot_addTrail()
{
	QVector<STRPoint> vecPoint;
	vecPoint.push_back(STRPoint(108.93, 34.27, 6000.0));
	//vecPoint.push_back(STRPoint(118.03, 34.27, 6000.0));
	vecPoint.push_back(STRPoint(117.5, 39.38, 6000.0));

	createTrail(1, vecPoint);
}

static double uavNum = 0;
static double ShipNum = 0;
static double VehicleNum = 0;

void earthWnd::slot_addUAV()
{
	QVector<STRPoint> vecPoint;
	vecPoint.push_back(STRPoint(108.93 + uavNum/5, 34.27 + uavNum/5, 0.0));
	vecPoint.push_back(STRPoint(109.5, 35.0, 2000.0));
	vecPoint.push_back(STRPoint(110.5, 35.38, 3000.0));
	vecPoint.push_back(STRPoint(114.5, 37.38, 5000.0));
	vecPoint.push_back(STRPoint(118.5, 39.38, 0.0));

	//vecPoint.push_back(STRPoint(122.08, 43.1331, 0.0));
	//vecPoint.push_back(STRPoint(122.09, 43.1431, 8000.0));
	//vecPoint.push_back(STRPoint(122.58, 43.7248, 1000.0));
	//vecPoint.push_back(STRPoint(122.8, 44.0244, 0.0));
	QString str = "UAV" + QString::number(uavNum);
	addUAV(str.toStdString(), vecPoint);
	uavNum += 1;
}

void earthWnd::slot_addShip()
{
	QVector<STRPoint> vecPoint;
	vecPoint.push_back(STRPoint(108.96 + ShipNum / 5, 34.27 + ShipNum / 5, 0.0));
	vecPoint.push_back(STRPoint(117.5, 39.38, 0.0));
	QString str = "Ship" + QString::number(ShipNum);
	addShip(str.toStdString(), vecPoint);
	ShipNum += 1;
}

void earthWnd::slot_addVehicle()
{
	QVector<STRPoint> vecPoint;
	vecPoint.push_back(STRPoint(108.99 + VehicleNum / 5, 34.27 + VehicleNum / 5, 0.0));
	vecPoint.push_back(STRPoint(117.5, 39.38, 0.0));
	QString str = "Vehicle" + QString::number(VehicleNum);
	addVehicle(str.toStdString(), vecPoint);
	VehicleNum += 1;
}

static double _x = 0;
static double _y = 0;
static double _z = 0;

void earthWnd::slot_X()
{
	delUAV("UAV0");
}

void earthWnd::slot_Y()
{
	delVehicle("Vehicle0");
}

void earthWnd::slot_Z()
{
	delAllShip();
}

void earthWnd::slot_X1()
{
	delShip("Ship0");
}

void earthWnd::slot_Y1()
{
	delAllUAV();
}

void earthWnd::slot_Z1()
{
	delAllVehicle();
	//G_STRVehicle.heading -= 5;
	//delUAV("UAV");
	
}

void earthWnd::delComboItem(QComboBox* combox, string name)
{
	int sizeCombox = combox->count();
	for (int i = 0; i < sizeCombox; ++i)
	{
		if (name == combox->itemText(i).toStdString())
		{
			combox->removeItem(i);
			return;
		}
	}
}

void earthWnd::setEarthTrack(int _type, string name)
{
	earth->trackNode(_type, name);
}

void earthWnd::slot_UAVComBoChanged(QString _name)
{
	setEarthTrack(1, _name.toStdString());
}

void earthWnd::slot_ShipComBoChanged(QString _name)
{
	setEarthTrack(2, _name.toStdString());
}

void earthWnd::slot_VehicleComBoChanged(QString _name)
{
	setEarthTrack(3, _name.toStdString());
}

void earthWnd::slot_setTrackObject(int _type)
{
	//int type = 0;
	string name = "";
	switch (_type)
	{
	case 1:
		name = ComboUAV->currentText().toStdString();
		break;
	case 2:
		name = ComboShip->currentText().toStdString();
		break;
	case 3:
		name = ComboVehicle->currentText().toStdString();
		break;
	default:
		break;
	}
	earth->trackNode(_type, name);
}

void earthWnd::delAllUAV()
{
	earth->delALLUAV();
}

void earthWnd::delAllShip()
{
	earth->delALLShip();
}

void earthWnd::delAllVehicle()
{
	earth->delALLVehicle();
}




