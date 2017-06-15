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

//��ӷɻ�����ʼ������
GlobalCtrlApi void addUAV(string name, QVector<STRPoint> _vecPoint);
//��Ӵ�����ʼ������
GlobalCtrlApi void addShip(string name, QVector<STRPoint> _vecPoint);
//��ӳ�����ʼ������
GlobalCtrlApi void addVehicle(string name, QVector<STRPoint> _vecPoint);

//��������ɾ��ָ���ɻ�
GlobalCtrlApi void delUAV(string name);
//��������ɾ��ָ����
GlobalCtrlApi void delShip(string name);
//��������ɾ��ָ����
GlobalCtrlApi void delVehicle(string name);

//ɾ�����зɻ�
GlobalCtrlApi void delAllUAV();
//ɾ�����д�			  
GlobalCtrlApi void delAllShip();
//ɾ�����г�			  
GlobalCtrlApi void delAllVehicle();

//����ָ�����Ƶķɻ�λ��
GlobalCtrlApi void updateUAVPos(string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed);
//����ָ�����ƵĴ�λ��
GlobalCtrlApi void updateShipPos(string name, double _long, double _lat, double _altitude, double _heading, double _speed);
//����ָ�����Ƶĳ�λ��
GlobalCtrlApi void updateVehiclePos(string name, double _long, double _lat, double _altitude, double _heading, double _speed);

//����һ�����ߣ���Ҫ����
GlobalCtrlApi void createTrail(int _target, QVector<STRPoint> _vecPoint);

GlobalCtrlApi bool checkptr(void* ptr);