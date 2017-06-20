#pragma once

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osg/Node>  
#include <osg/Geode>  
#include <osg/Group>  
#include <osg/LineSegment>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/IntersectVisitor>
#include <osgGA/DriveManipulator>
#include <osg/CoordinateSystemNode>
#include "TravelManipulator.h"
#include <osgUtil/Optimizer>
#include <osgViewer/ViewerEventHandlers>  
#include <osgGA/StateSetManipulator> 
#include <osgGA/CameraManipulator>  
#include <osgEarth/MapNode>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgGA/NodeTrackerManipulator>
#include <osgEarth/MapNode>
#include <osgEarthSymbology/GeometryFactory>
#include <osgParticle/PrecipitationEffect>
#include <osg/Multisample>
#include <osgEarth/Utils>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthDrivers/model_simple/SimpleModelOptions>
#include <osgGA/KeySwitchMatrixManipulator>
#include <QTimer>
#include <QVector>
#include <list>

#include "NodeMatrix.h"
#include "AdapterWidget.h"
#include "ctime.h"
#include "PickHandle.h"
#include "global.h"
#include "showInfoUpdate.h"

//#include "Ocean/OSGProfile.h"



class earthWidget : public AdapterWidget, public osgViewer::Viewer
{
	Q_OBJECT
public:

	earthWidget(QWidget* parent = NULL);
	~earthWidget();

	void addLabel();

	void addModel(int _type, string _name, QVector<STRPoint> _vecPoint, int _trailModel = ETWHOLE);
	void addModel(int _type, osg::Group* _groupRoot, osg::Node* _node, string _name, int _scale, int _offset, double _long, double _lat, double _altitude, int _trailModel = ETWHOLE);
	void addUAV(string name, double _long, double _lat, double _altitude);
	void addShip(string name, double _long, double _lat, double _altitude);
	void addVehicle(string name, double _long, double _lat, double _altitude);

	void delObject(int _Object, string _name);
	//void delNodeAndTrail(int _Object, string _name, osg::Node* _node, osg::Group* _mtRoot, osgEarth::Annotation::FeatureNode* _fn, double _long, double _lat);
	//void delUAV(string name);
	//void delShip(string name);
	//void delVehicle(string name);

	void updateModelPos(int _type, string name, double _long, double _lat, double _altitude, double _heading, double _pitch, double _roll, double _speed);
	void updateModelPos(int _type, string name, string currentObjectName, osg::Group* _group, int offset, STRDATA* strData);
	//void updateUAVPosition(string name, osg::Vec3d vec3d, double _heading, double _pitch, double _roll, double _speed);
	//void updateShipPosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed);
	//void updateVehiclePosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed);

	//创建航线
	FeatureNode* createTrail(int _target, string _name, QVector<STRPoint> _vecPoint);

	//osg::Vec3dArray					   _vec3dArray;
	//void addUAVByTrail(string name, QVector<STRPoint> _vecPoint);
	//void addShipByTrail(string name, QVector<STRPoint> _vecPoint);
	//void addVehicleByTrail(string name, QVector<STRPoint> _vecPoint);
	void addUAVWithoutTrail(string name, QVector<STRPoint> _vecPoint);
	
	void delALLSameNode(osg::Group* group);
	void delALLUAV();
	void delALLShip();
	void delALLVehicle();

	void rotateX(double x);
	void rotateY(double y);
	void rotateZ(double z);

	void trackNode(int _type, string _name);

public slots:

    void slot_setOceanWndVisble(bool isShow);
	void slot_isFollowPlane(bool isFollow);
	void slot_FollowOne(int isFollow);
	void slot_addInterval();
	void slot_subInterval();
	//void slot_startPlay();
	//void slot_test();
	void slot_currentMousePos(osg::Vec3d);

signals:
	

protected:

	void setTrackObject(osg::Group* group, string name, int _distance);
	void readNodeFiles();
	void initViewer();
	void initEarthWidget();
	void earthRun();
	void setEarthManipulator();
	void connectWidgets();
	virtual void paintGL();
	void updateScene();
	void updateSceneTime();
	void createLabels();
	void creatCanvas();
	void test();
	void addEventHandlers();
	void addPlanePath(osg::Vec3d vecStart, osg::Vec3d vecEnd);
	osg::Node* createTrailLine();
	void setTrailStyle(osg::Node* node, osg::Vec4 vec4);
	void addViewPointLabel();
	osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);
	
	//Build尾迹
	void BuildTail(osg::Vec3 position, osg::MatrixTransform* scaler, osg::MatrixTransform* root);
	osg::ref_ptr<osg::MatrixTransform> findMatrixByName(osg::Group* _group, string _name);


	void delNodeByName(string _name, osg::Group* _group);
	void delTrailByName(string _name);

	enum ETRAILMODEL
	{
		ETWHOLE = 1,
		ETPART,
	}ETRAILMODEL;

	enum OBJECT
	{
		EOPLANE = 1,
		EOSHIP,
		EOVEHICLE,
	}EOBJECT;

private:

	static int mInterval, _count;
	osgSim::DOFTransform * propeller;
	osgSim::DOFTransform * dRudder_L;
	//OSGProfile* oceanDialog;
	osg::ref_ptr<osgViewer::Viewer> _view;
	osg::ref_ptr<osgEarth::MapNode> _pMapNode;
	osg::ref_ptr<osg::Group> _root, m_PlaneRoot, _planePath, m_ShipRoot, m_VehicleRoot;
	osg::ref_ptr<osg::Group> m_TrailRoot;
	osg::ref_ptr<const osgEarth::SpatialReference> m_pMapSRS;
	osg::ref_ptr<const osgEarth::SpatialReference> m_pGeoSRS;
	osg::ref_ptr<osgEarth::Util::SkyNode> _sky;

	osg::ref_ptr<osgEarth::Util::EarthManipulator>   _pEarthMan;
	/**节点跟随操作器*/
	osg::ref_ptr<osgGA::NodeTrackerManipulator>   m_pTrackNode;

	/**坐标系*/
	CoordinateSystem _coordinateSystem;
	LabelNode*                         _label;

	//飞机坐标
	osg::ref_ptr<osg::Node> nodeUAV;
	ControlCanvas* _canvas;

	//船坐标
	osg::ref_ptr<osg::Node> nodeShip;

	//车坐标
	osg::ref_ptr<osg::Node> nodeVehicle;

	QMutex   _osgLock;
	//QTimer _timer;
	PickHandle* _pickHandler;

	//画飞机航迹
	Style pathStyle; 
	const osgEarth::SpatialReference* m_pSrclatLong;
	osgEarth::GeoPoint m_pGPXian;
	osg::Matrix mtXian;

	//UAVTrail* UAVTrailCallBack;

	bool bIsFollow;
	int currentFollowObject;
	double _reveTime;
	string currentUAVName;
	string currentShipName;
	string currentVehicleName;

	showInfoUpdate* grid;
	
	QTimer* timer;
};


