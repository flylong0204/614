#pragma once
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/MapNode>
#include <osgEarth/Utils>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarth/GeoMath>
#include <osgEarth/GeoTransform>
#include <osgManipulator/TrackballDragger>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#include <osgGA/NodeTrackerManipulator>
#include <osgEarth/Viewpoint>
#include <osgSim/DOFTransform>
#include <QtCore/qglobal.h>
#include <QGLWidget>
#include <osgViewer/GraphicsWindow>
#include <QKeyEvent>
#include <QEvent>
#include <QGraphicsView>
#include <QTimer>
#include <iostream>
#include <QObject>
#include <osg/NodeCallback>
#include <osgEarth/ElevationQuery>
#include <osgEarth/ElevationLayer>
#include <osgEarth/ElevationLOD>
#include <osgEarthUtil/Formatter>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/StringUtils>
#include <osgEarth/Terrain>
#include <osgEarthUtil/Formatter>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarth/ElevationQuery>
#include <osgUtil/LineSegmentIntersector>
#include <osgEarthAnnotation/FeatureNode>
#include <QMutex>

#include "Common.h"

using namespace std;

#define D2R (osg::PI/180.0)
#define R2D (180.0/osg::PI)

#pragma  pack(1)

/*无人机参数数据结构*/
typedef struct STRPLANE : public osg::Referenced
{
	std::string name;
	double longitude; //无人机经度 0~180
	double latitude;  //无人机纬度 -90~90
	double altitude;  //无人机飞行高度
	double heading;    //无人机航向角
	double pitching;   //无人机俯仰角
	double roll;       //无人机横滚
	double speed;
	STRPLANE(string _name, double _longitude, double _latitude, double _altitude, double _heading = 0.0, double _pitching = 0.0, double _roll = 0.0, double _speed = 0.0)
	{
		name = _name;
		longitude = _longitude;
		latitude = _latitude;
		altitude = _altitude;
		heading = _heading;
		pitching = _pitching;
		roll = _roll;
		speed = _speed;
	}
}STRPlane;


/*船参数数据结构*/
typedef struct STRSHIP : public osg::Referenced
{
	std::string name;
	double longitude; //经度 0~180
	double latitude;  //纬度 -90~90
	double altitude;  //高度
	double heading;   //航向角
	double speed;
	STRSHIP(string _name, double _longitude, double _latitude, double _altitude, double _heading = 0.0, double _speed = 0.0)
	{
		name = _name;
		longitude = _longitude;
		latitude = _latitude;
		altitude = _altitude;
		heading = _heading;
		speed = _speed;
	}
}STRShip;


/*车参数数据结构*/
typedef struct STRVEHICLE : public osg::Referenced
{
	std::string name;
	double longitude; //经度 0~180
	double latitude;  //纬度 -90~90
	double altitude;  //高度
	double heading;   //航向角
	double speed;
	STRVEHICLE(string _name, double _longitude, double _latitude, double _altitude, double _heading = 0.0, double _speed = 0.0)
	{
		name = _name;
		longitude = _longitude;
		latitude = _latitude;
		altitude = _altitude;
		heading = _heading;
		speed = _speed;
	}
}STRVehicle;

class AdapterWidget : public QGLWidget
{
	Q_OBJECT

public:
	AdapterWidget(QWidget *parent = NULL);
	~AdapterWidget();

	osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
	const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }
	osg::GraphicsContext* getGraphicsContext() { return _gc.get(); }
	const osg::GraphicsContext* getGraphicsContext() const { return _gc.get(); }

protected:
	void init();
	virtual void resizeGL(int width, int height);
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

private:

	void setKeyboardModifiers(QInputEvent* event);
	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
	osg::ref_ptr<osg::GraphicsContext>	_gc;	
signals:
	void quitFullScreen();
};


using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Annotation;

//更新飞机位置
//class updateEventHandler : public osgGA::NodeTrackerManipulator/*, public QObject*/
//{
//	//Q_OBJECT
//public:
//	
//	updateEventHandler(const osg::ref_ptr<GeoPositionNode> _geo, osg::MatrixTransform* mtSelf);
//	virtual ~updateEventHandler(){};
//	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
//	//GeoPoint p = GeoPoint(SpatialReference::create("wgs84"), osg::Vec3(0 + 0.001*a++, 0, 8000000.0));
//	//_geo->setPosition(p);
//private:
//	updateEventHandler();
//	osg::ref_ptr<GeoPositionNode>      _geo; \
//	osg::MatrixTransform*				_mtSelf;
//	//osg::Vec3d*				   _vec3d;
//
//};


struct MousePositionHandler : public osgGA::GUIEventHandler
{
public:
	//Q_OBJECT
	//构造函数
	MousePositionHandler(osg::ref_ptr<const osgEarth::SpatialReference>);

	//更新回调，具体的内容可以参考父类，传进来的参数是屏幕坐标xy，和osgViewer
	void update(float x, float y, osgViewer::View* view);

	//参数一个是事件的动作，另外一个是对应的操作
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	//Map对象
	const Map*       _map;
	//地形对象
	//const Terrain*   _terrain;
	osgEarth::MapNode* _mapNode;
	osg::ref_ptr<const osgEarth::SpatialReference> _srs;
	bool             _mouseDown;
	//查询高程使用的对象
	ElevationQuery   _query;
	osg::NodePath    _path;
	std::string _lat, _long;
public:
	//signals :
	//void signal_latlong(std::string, std::string);
};


//未画航线
//class  MyTrailCallBack : public osg::NodeCallback
//{
//public:
//	inline MyTrailCallBack(){};
//	MyTrailCallBack(osg::Geode* geode, osgGA::CameraManipulator* nodeTM);
//	virtual inline ~MyTrailCallBack(){};
//	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
//	virtual void updateData() = 0;
//	//void updateShowInfo();
//	void updateDrawLine();
//
//	//osg::Geode* geodeToFollow;
//	bool frisFrame;
//	double _latitude;
//	double _longitude;
//	double _height;
//	double _speed;
//	double _heading;		//航向角
//	double _pitch;
//	double _roll;
//	double _headingPre;
//	double _pitchPre;
//	double _rollPre;
//	double _headingOffset;
//	osgGA::CameraManipulator* nodeTrack;
//	const osgEarth::SpatialReference* SrclatLong;
//	osg::MatrixTransform* mtSelf;
//	osg::MatrixTransform* mtRotate;
//	QMutex G_mutex;
//	osg::Geode* lineGeo;
//
//
//	//double _latitude, _longitude, _height, _speed, _heading, _pitch, _roll;
//float _degreePropeller;
//float _degreeDRudder_L;
//short _crtPitch;			//当前俯仰
//short _crtHeading;          //惯导航向
////short _crtPitch;          //惯导俯仰
//short _crtRool;             //惯导横滚
//osgGA::CameraManipulator* nodeTrack;
//const osgEarth::SpatialReference* SrclatLong;
//	osg::Vec3 preVec3;
//};
//
//class UAVTrail : public MyTrailCallBack
//{
//public:
//	//UAVTrail(osg::Geode* geode, osgGA::CameraManipulator* nodeTM);
//	UAVTrail(osg::MatrixTransform* _mtObjectRoot, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L);
//	virtual inline ~UAVTrail(){};
//	virtual void updateData();
//
//private:
//	STRPlane* _UAVdata;
//
//	double _degreePropeller;
//	double _degreeDRudder_L;
//	osg::ref_ptr<osgSim::DOFTransform> propeller;
//	osg::ref_ptr<osgSim::DOFTransform> dRudder_L;
//};

//class ShipTrail : public MyTrailCallBack
//{
//public:
//	inline ShipTrail(){};
//	//ShipTrail(osg::Geode* geode, osgGA::NodeTrackerManipulator* nodeTM);
//	ShipTrail(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRShip* shipData);
//	virtual inline ~ShipTrail(){};
//	virtual void updateData();
//private:
//
//};
//
//class VehicleTrail : public MyTrailCallBack
//{
//public:
//	inline VehicleTrail(){};
//	//VehicleTrail(osg::Geode* geode, osgGA::NodeTrackerManipulator* nodeTM);
//	VehicleTrail(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRVehicle* VehicleData);
//	virtual inline ~VehicleTrail(){};
//	virtual void updateData();
//private:
//
//};

class callBackBase : public osg::NodeCallback
{
public:
	inline callBackBase(){};
	callBackBase(osg::MatrixTransform* _mtObjectRoot, osgEarth::Annotation::FeatureNode* _featureNode = nullptr)
	//callBackBase(osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, \
	//	STRPlane* UAVdata, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L, osgEarth::Annotation::FeatureNode* _featureNode)
	{
		
		if (1)
		{
			SrclatLong = osgEarth::SpatialReference::create("wgs84");
			osg::ref_ptr<osg::MatrixTransform> mtObject = dynamic_cast<osg::MatrixTransform*>(_mtObjectRoot->getChild(0));
			strData  = dynamic_cast<STRPlane*>(mtObject->getUserData());
			mtRotate = dynamic_cast<osg::MatrixTransform*>(mtObject->getChild(0));
			mtSelf   = dynamic_cast<osg::MatrixTransform*>(mtRotate->getChild(0));
			featureNode = _featureNode;

			//mtSelf = _mtObject;
			//mtRotate = _mtRotate;
			//strData = UAVdata;
			//featureNode = _featureNode;

			//_latitude      = strData->latitude;
			//_longitude     = strData->longitude;
			//_altitude	   = strData->altitude;
			//_speed  	   = strData->speed;
			_heading 	   = 0.0;
			_pitch 		   = 0.0;
			_roll		   = 0.0;
			_headingPre    = 0.0;
			_pitchPre	   = 0.0;
			_rollPre	   = 0.0;
			_headingOffset = 0.0;

		}
	};
	virtual inline ~callBackBase(){};
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	virtual void updataCommonData();
	virtual void updateSelfData() = 0;

	double _latitude;
	double _longitude;
	double _altitude;
	double _speed;
	double _heading;		
	double _pitch;
	double _roll;
	double _headingPre;
	double _pitchPre;
	double _rollPre;
	double _headingOffset;
	bool   _needDrawLine;

	STRPlane* strData;
	osg::MatrixTransform* mtSelf;
	osg::MatrixTransform* mtRotate;
	const osgEarth::SpatialReference* SrclatLong;
	osgEarth::Annotation::FeatureNode* featureNode;	

	QMutex G_mutex;
	//osgGA::CameraManipulator* nodeTrack;
};

class UAVCallBack : public callBackBase
{
public:
	UAVCallBack(osg::MatrixTransform* mtObjectRoot, osgEarth::Annotation::FeatureNode* _featureNode = nullptr) 
		:callBackBase(mtObjectRoot, _featureNode){};
	//UAVCallBack(osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, \
		//	STRPlane* UAVdata, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L, osgEarth::Annotation::FeatureNode* _featureNode)\
		//	: callBackBase(_mtObject, _mtRotate, nodeTM, UAVdata, _propeller, _dRudder_L, _featureNode){};
	virtual inline ~UAVCallBack(){};
	virtual void updateSelfData(){};

private:

};



//预画航线
class  TrailBaseCallBack : public osg::NodeCallback
{
public:
	inline TrailBaseCallBack(){};
	virtual inline ~TrailBaseCallBack(){};
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	virtual void updateData() = 0;

	osg::Geode* geodeToFollow;
	double _latitude;
	double _longitude;
	double _height;
	double _speed;
	double _heading;		//航向角
	double _pitch;
	double _roll;
	double _headingPre;				
	double _pitchPre;
	double _rollPre;
	double _headingOffset;
	osgGA::CameraManipulator* nodeTrack;
	const osgEarth::SpatialReference* SrclatLong;
	osg::MatrixTransform* mtSelf;
	osg::MatrixTransform* mtRotate;
	QMutex G_mutex;
};



class UVATrailCallBack : public TrailBaseCallBack
{
public:
	UVATrailCallBack(){};
	UVATrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRPlane* UAVdata, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L);
	virtual inline ~UVATrailCallBack(){};
	virtual void updateData();

	STRPlane* _UAVdata;
	
	double _degreePropeller;
	double _degreeDRudder_L;
	osg::ref_ptr<osgSim::DOFTransform> propeller;
	osg::ref_ptr<osgSim::DOFTransform> dRudder_L;

};

class UAVCALLBACKUpdateTrail : public UVATrailCallBack
{
public:
	UAVCALLBACKUpdateTrail(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRPlane* UAVdata, osgSim::DOFTransform* _propeller, \
		osgSim::DOFTransform* _dRudder_L, osgEarth::Annotation::FeatureNode* _featureNode);
	virtual inline ~UAVCALLBACKUpdateTrail(){};
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	osg::Vec3 preVec3;
	//osg::Geode* lineGeo;
	osgEarth::Annotation::FeatureNode* featureNode;
	//virtual void updateData();
	bool FirstFrame;
};

class ShipTrailCallBack : public TrailBaseCallBack
{
public:
	ShipTrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRShip* shipData);
	virtual inline ~ShipTrailCallBack(){};
	virtual void updateData();
private:
	STRShip* _shipData;
};

class VehicleTrailCallBack : public TrailBaseCallBack
{
public:
	VehicleTrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRVehicle* VehicleData);
	virtual inline ~VehicleTrailCallBack(){};
	virtual void updateData();
private:
	STRVehicle* _VehicleData;
};



