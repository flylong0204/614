#include "showInfoUpdate.h"



showInfoUpdate::showInfoUpdate()
{
	setControl(0, 0, new infoLable(ordeString11("Name:")));
	setControl(0, 1, new infoLable(ordeString11("Longitude:")));
	setControl(0, 2, new infoLable(ordeString11("Latitude:")));
	setControl(0, 3, new infoLable(ordeString11("Elevation:")));
	setControl(0, 4, new infoLable(ordeString11("Heading:")));	
	setControl(0, 5, new infoLable(ordeString11("Pitch:")));
	setControl(0, 6, new infoLable(ordeString11("Roll:")));
	setControl(0, 7, new infoLable(ordeString11("Speed:")));

	//UAV
	UAV_name      = setControl(1, 0, new infoLable(ordeString7("")));
	UAV_Longitude = setControl(1, 1, new infoLable(ordeString7("")));
	UAV_Latitude  = setControl(1, 2, new infoLable(ordeString7("")));
	UAV_Elevation = setControl(1, 3, new infoLable(ordeString7("")));
	UAV_Heading   = setControl(1, 4, new infoLable(ordeString7("")));
	UAV_Pitch     = setControl(1, 5, new infoLable(ordeString7("")));
	UAV_Roll      = setControl(1, 6, new infoLable(ordeString7("")));
	UAV_Speed     = setControl(1, 7, new infoLable(ordeString7("")));

	//Ship
	Ship_name	   = setControl(2, 0, new infoLable(ordeString7("")));
	Ship_Longitude = setControl(2, 1, new infoLable(ordeString7("")));
	Ship_Latitude  = setControl(2, 2, new infoLable(ordeString7("")));
	Ship_Elevation = setControl(2, 3, new infoLable(ordeString7("")));
	Ship_Heading   = setControl(2, 4, new infoLable(ordeString7("")));
	Ship_Pitch     = setControl(2, 5, new infoLable(ordeString7("")));
	Ship_Roll      = setControl(2, 6, new infoLable(ordeString7("")));
	Ship_Speed     = setControl(2, 7, new infoLable(ordeString7("")));

	//Vehicle
	Vehicle_name      = setControl(3, 0, new infoLable(ordeString7("")));
	Vehicle_Longitude = setControl(3, 1, new infoLable(ordeString7("")));
	Vehicle_Latitude  = setControl(3, 2, new infoLable(ordeString7("")));
	Vehicle_Elevation = setControl(3, 3, new infoLable(ordeString7("")));
	Vehicle_Heading   = setControl(3, 4, new infoLable(ordeString7("")));
	Vehicle_Pitch     = setControl(3, 5, new infoLable(ordeString7("")));
	Vehicle_Roll      = setControl(3, 6, new infoLable(ordeString7("")));
	Vehicle_Speed     = setControl(3, 7, new infoLable(ordeString7("")));
}


void showInfoUpdate::updateInfo(int _type, string _name, double _long, double _lat, double _altitude, double _heading /*= 0.0*/, double _pitch /*= 0.0*/, double _roll /*= 0.0*/, double _speed /*= 0.0*/)
{
	string name = ordeString7(_name);

	switch (_type)
	{
	case 1:
		UAV_name->setText(name);
		UAV_Longitude->setText(stringNumber(_long));
		UAV_Latitude->setText(stringNumber(_lat));
		UAV_Elevation->setText(stringNumber(_altitude));
		UAV_Heading->setText(stringNumber(_heading));
		UAV_Pitch->setText(stringNumber(_pitch));
		UAV_Roll->setText(stringNumber(_roll));
		UAV_Speed->setText(stringNumber(_speed));
		break;
	case 2:
		Ship_name->setText(name);
		Ship_Longitude->setText(stringNumber(_long));
		Ship_Latitude->setText(stringNumber(_lat));
		Ship_Elevation->setText(stringNumber(_altitude));
		Ship_Heading->setText(stringNumber(_heading));
		Ship_Pitch->setText(stringNumber(_pitch));
		Ship_Roll->setText(stringNumber(_roll));
		Ship_Speed->setText(stringNumber(_speed));
		break;
	case 3:
		Vehicle_name->setText(name);
		Vehicle_Longitude->setText(stringNumber(_long));
		Vehicle_Latitude->setText(stringNumber(_lat));
		Vehicle_Elevation->setText(stringNumber(_altitude));
		Vehicle_Heading->setText(stringNumber(_heading));
		Vehicle_Pitch->setText(stringNumber(_pitch));
		Vehicle_Roll->setText(stringNumber(_roll));
		Vehicle_Speed->setText(stringNumber(_speed));
		break;
	default:
		break;
	}
	bool b = UAV_name->visible();
	//for (int Column = 0; Column < getNumColumns(); ++Column)
	//{
	//	infoLable* label = dynamic_cast<infoLable*>(getControl(Column, 0));
	//	if (_name == label->text())
	//	{
			//if (_type > 3)
			//{
			//	return;
			//}
			//(dynamic_cast<infoLable*>(getControl(_type, 0)))->setText(_name);
			//(dynamic_cast<infoLable*>(getControl(_type, 1)))->setText(stringNumber(_long));
			//(dynamic_cast<infoLable*>(getControl(_type, 2)))->setText(stringNumber(_lat));
			//(dynamic_cast<infoLable*>(getControl(_type, 3)))->setText(stringNumber(_altitude));
			//(dynamic_cast<infoLable*>(getControl(_type, 4)))->setText(stringNumber(_heading));
			//(dynamic_cast<infoLable*>(getControl(_type, 5)))->setText(stringNumber(_pitch));
			//(dynamic_cast<infoLable*>(getControl(_type, 6)))->setText(stringNumber(_roll));
			//(dynamic_cast<infoLable*>(getControl(_type, 7)))->setText(stringNumber(_speed));
	//	}
	//}
}

void showInfoUpdate::insertInfo(string _name, double _long, double _lat, double _altitude, double _heading /*= 0.0*/, double _pitch /*= 0.0*/, double _roll /*= 0.0*/, double _speed /*= 0.0*/)
{
	int size = getNumColumns();
	setControl(size, 0, new infoLable(_name));
	setControl(size, 1, new infoLable(stringNumber(_long)));
	setControl(size, 2, new infoLable(stringNumber(_lat)));
	setControl(size, 3, new infoLable(stringNumber(_altitude)));
	setControl(size, 4, new infoLable(stringNumber(_heading)));
	setControl(size, 5, new infoLable(stringNumber(_pitch)));
	setControl(size, 6, new infoLable(stringNumber(_roll)));
	setControl(size, 7, new infoLable(stringNumber(_speed)));
}

void showInfoUpdate::removeInfo(string _name)
{
	for (int Column = 0; Column < getNumColumns(); ++Column)
	{
		infoLable* label = dynamic_cast<infoLable*>(getControl(Column, 0));
		if (_name == label->text())
		{
			
		}
	}
}

showInfoUpdate::~showInfoUpdate()
{
}
