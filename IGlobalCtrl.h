#pragma once
#include <iostream>
#include <string>
#include <QVector>
#include "global.h"

#ifdef GlobalCtrlDll
#define GlobalCtrlApi extern "C" __declspec(dllexport)
#else 
#define GlobalCtrlApi extern "C" __declspec(dllimport)
#endif

using namespace std;

class earthWnd;
//#define GlobalCtrlApi

GlobalCtrlApi earthWnd* GetEarthWidget(void* ptr = 0);

//添加飞机并初始化航线
GlobalCtrlApi void addUAV(string name, QVector<STRPoint> _vecPoint);
//添加船并初始化航线
GlobalCtrlApi void addShip(string name, QVector<STRPoint> _vecPoint);
//添加车并初始化航线
GlobalCtrlApi void addVehicle(string name, QVector<STRPoint> _vecPoint);

//根据名称删除指定飞机
GlobalCtrlApi void delUAV(string name);
//根据名称删除指定船
GlobalCtrlApi void delShip(string name);
//根据名称删除指定车
GlobalCtrlApi void delVehicle(string name);

//删除所有飞机
GlobalCtrlApi void delAllUAV();
//删除所有船			  
GlobalCtrlApi void delAllShip();
//删除所有车			  
GlobalCtrlApi void delAllVehicle();

//更新指定名称的飞机位置
GlobalCtrlApi void updateUAVPos(string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed);
//更新指定名称的船位置
GlobalCtrlApi void updateShipPos(string name, double _long, double _lat, double _altitude, double _heading, double _speed);
//更新指定名称的车位置
GlobalCtrlApi void updateVehiclePos(string name, double _long, double _lat, double _altitude, double _heading, double _speed);

//创建一条航线，需要类型
GlobalCtrlApi void createTrail(int _target, QVector<STRPoint> _vecPoint);

GlobalCtrlApi bool checkptr(void* ptr);