#pragma once

#include <iostream>
#include <string>
#include <QString>
#include <osgEarthUtil/Controls>

using namespace std;
using namespace osgEarth::Util::Controls;

class infoLable : public LabelControl
{
public:

	infoLable(string _name){
		setText(_name);
		setFontSize(16.0);
		setForeColor(osg::Vec4(1.0, 1.0, 1.0, 0.5));
		setWidth(30);
	};
	inline ~infoLable(){};
	

private:

};



class showInfoUpdate : public Grid
{
public:
	showInfoUpdate();
	//showInfoUpdate(const Alignment& halign, const Alignment& valign, const Gutter& padding, float spacing);
	virtual ~showInfoUpdate();
	void updateInfo(int _type, string _name, double _long, double _lat, double _altitude, double _heading = 0.0, double _pitch = 0.0, double _roll = 0.0, double _speed = 0.0);

private:
	void insertInfo(string _name, double _long, double _lat, double _altitude, double _heading = 0.0, double _pitch = 0.0, double _roll = 0.0, double _speed = 0.0);
	void removeInfo(string _name);
	string stringNumber(double _number){
		return QString::number(_number, 'f', 3).leftJustified(7, ' ').toStdString();
	}
	string ordeString11(string str){
		return QString::fromStdString(str).leftJustified(11, ' ').toStdString();
	}
	string ordeString7(string str){
		return QString::fromStdString(str).leftJustified(7, ' ').toStdString();
	}

	infoLable* UAV_name;
	infoLable* UAV_Longitude;
	infoLable* UAV_Latitude;
	infoLable* UAV_Elevation;
	infoLable* UAV_Heading;
	infoLable* UAV_Pitch;
	infoLable* UAV_Roll;
	infoLable* UAV_Speed;

	infoLable* Ship_name;
	infoLable* Ship_Longitude;
	infoLable* Ship_Latitude;
	infoLable* Ship_Elevation;
	infoLable* Ship_Heading;
	infoLable* Ship_Pitch;
	infoLable* Ship_Roll;
	infoLable* Ship_Speed;

	infoLable* Vehicle_name;
	infoLable* Vehicle_Longitude;
	infoLable* Vehicle_Latitude;
	infoLable* Vehicle_Elevation;
	infoLable* Vehicle_Heading;
	infoLable* Vehicle_Pitch;
	infoLable* Vehicle_Roll;
	infoLable* Vehicle_Speed;
};

