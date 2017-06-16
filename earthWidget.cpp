#include "earthWidget.h"
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarth/GeoData>
#include <osg/Vec3d>
#include <QDateTime>
#include <QString>
#include <sstream>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgParticle/FireEffect>
#include <osgEarthAnnotation/AnnotationEditing>
#include <osgEarthAnnotation/ImageOverlayEditor>
#include <osgEarth/Map>
#include <osgSim/DOFTransform>
#include <osg/Referenced>
#include <QMessageBox>

#include <osgEarthUtil/MouseCoordsTool>

osg::ref_ptr<osg::MatrixTransform>  g_mtRotateface;

double earthRotationDegree = -170;
int earthWidget::_count = 0;
int earthWidget::mInterval = 0;

osgEarth::Util::Controls::LabelControl* readout;
osgEarth::Util::Controls::LabelControl* rightText;
osgEarth::Util::Controls::LabelControl* leftText;

LabelControl*  LP_UAV_name = 0L;		    
LabelControl*  LP_UAV_Longitude = 0L;		//经度
LabelControl*  LP_UAV_Latitude = 0L;		//纬度
LabelControl*  LP_UAV_Elevation = 0L;		//高程
LabelControl*  LP_UAV_Velocity = 0L;		//速度
LabelControl*  LP_UAV_Heading = 0L;			//航向
LabelControl*  LP_UAV_Pitch = 0L;			//俯仰
LabelControl*  LP_UAV_Roll = 0L;			//滚转

LabelControl*  LP_Ship_name = 0L;
LabelControl*  LP_Ship_Longitude = 0L;		//经度
LabelControl*  LP_Ship_Latitude = 0L;		//纬度
LabelControl*  LP_Ship_Elevation = 0L;		//高程
LabelControl*  LP_Ship_Heading = 0L;		//航向
LabelControl*  LP_Ship_Velocity = 0L;		//速度

LabelControl*  LP_Vehicle_name = 0L;
LabelControl*  LP_Vehicle_Longitude = 0L;		//经度
LabelControl*  LP_Vehicle_Latitude = 0L;		//纬度
LabelControl*  LP_Vehicle_Elevation = 0L;		//高程
LabelControl*  LP_Vehicle_Heading = 0L;		    //航向
LabelControl*  LP_Vehicle_Velocity = 0L;		//速度

earthWidget::earthWidget(QWidget* parent) : AdapterWidget(parent)
{
	currentFollowObject = 0;
	m_pSrclatLong = osgEarth::SpatialReference::get("wgs84");
	//m_pTrackUAV = NULL;
	//UAVTrailCallBack = NULL;
	m_pTrackNode = NULL;
	currentUAVName = "";
	currentShipName = "";
	currentVehicleName = "";

	//setCursor(Qt::DragMoveCursor);
	initViewer();
	initEarthWidget();
	setEarthManipulator();
	creatCanvas();
	createLabels();
	connectWidgets();
	addEventHandlers();
	earthRun();
	//test();
	//oceanDialog = new OSGProfile(this);
	//oceanDialog->setFixedSize(600, 400);
	//oceanDialog->setVisible(false);
	
}

earthWidget::~earthWidget()
{
}

void earthWidget::initViewer()
{
	_view = this;
	osg::notify(osg::FATAL);
	
	osg::DisplaySettings::instance()->setNumMultiSamples(16);
	osg::ref_ptr<osg::Viewport> viewPort = new osg::Viewport(0, 0, width(), height());
	_view->getCamera()->setViewport(viewPort);

	//创建对称透视投影
	_view->getCamera()->setProjectionMatrixAsPerspective(30.0f,
		static_cast<double>(width()) / static_cast<double>(height()), 1.0f, 10000.0f);
	_view->getCamera()->setGraphicsContext(getGraphicsWindow());
										    setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
	_view->getCamera()->setNearFarRatio(0.0000001);
	_view->getCamera()->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);//);
}

void earthWidget::initEarthWidget()
{
	_root = new osg::Group();

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("./myearth.earth");
	if (NULL == node)
	{
		QMessageBox::about(NULL, "Error", "Can not open myearth.earth file");
		return;
	}
	_pMapNode = osgEarth::MapNode::get(node);
	_pMapNode->setName("earth");
	//
	_view->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(_pMapNode));

	//增加天气系统
	osgEarth::Util::SkyOptions kk;
	kk.ambient() = 0.20f;

	_sky = osgEarth::Util::SkyNode::create(kk, _pMapNode);
	if (_sky)
	{
		_sky->attach(_view, 1);
	}

	m_pMapSRS = _pMapNode->getMapSRS();			/**地固坐标系*/
	m_pGeoSRS = m_pMapSRS->getGeographicSRS();	/**大地坐标系*/

	osgEarth::DateTime dataTime;
	_sky->setLighting(true);
	osgEarth::Util::Ephemeris* eph = new osgEarth::Util::Ephemeris;
	_sky->setEphemeris(eph);
	_sky->setDateTime(dataTime);
	_sky->setMoonVisible(true);
	_sky->attach(_view);

	_sky->addChild(_pMapNode.get());
	_root->addChild(_sky.get());
	
	//抗锯齿
	osg::Multisample* pms=new osg::Multisample; 
	pms->setSampleCoverage(1,true);
	_root->getOrCreateStateSet()->setMode(GL_MULTISAMPLE_ARB, true);
	_root->getOrCreateStateSet()->setAttributeAndModes(pms, osg::StateAttribute::ON);


	//飞机节点
	m_PlaneRoot = new osg::Group();
	_root->addChild(m_PlaneRoot.get());

	//船节点
	m_ShipRoot = new osg::Group();
	_root->addChild(m_ShipRoot.get());

	//车节点
	m_VehicleRoot = new osg::Group();
	_root->addChild(m_VehicleRoot.get());

	//飞机航迹组节点
	_planePath = new osg::Group;
	_root->addChild(_planePath);

	if (NULL == m_pTrackNode)
	{
		m_pTrackNode = new osgGA::NodeTrackerManipulator;
		m_pTrackNode->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
		m_pTrackNode->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
	}
	

	addLabel();	
	//预先读入模型
	readNodeFiles();
}

void earthWidget::readNodeFiles()
{
	nodeUAV = osgDB::readNodeFile("./moudles/heron.FLT");
	if (NULL == nodeUAV)
	{
		QMessageBox::about(NULL, "Error", "Can not open heron.FLT file");
		return;
	}

	//查找DOF
	propeller = NULL;
	dRudder_L = NULL;
	osg::Node* foundNode = NULL;
	foundNode = findNamedNode("dRear", nodeUAV);
	if (foundNode){
		osgSim::DOFTransform * weiyi = dynamic_cast<osgSim::DOFTransform *>(foundNode);
		if (weiyi){
			propeller = weiyi;
		}
		foundNode = findNamedNode("dRudder_L", nodeUAV);
		if (foundNode){
			osgSim::DOFTransform * spyCamera = dynamic_cast<osgSim::DOFTransform *>(foundNode);
			if (spyCamera){
				dRudder_L = spyCamera;
			}
		}
	}

	nodeShip = osgDB::readNodeFile("./moudles/054Ship.ive");
	if (NULL == nodeShip)
	{
		QMessageBox::about(NULL, "Error", "Can not open 054Ship.ive file");
		return;
	}

	nodeVehicle = osgDB::readNodeFile("./moudles/Tank.ive"); 
	if (NULL == nodeVehicle)
	{
		QMessageBox::about(NULL, "Error", "Can not open Tank.ive file");
		return;
	}
}

void earthWidget::addEventHandlers()
{
	_pickHandler = new PickHandle(m_pMapSRS);
	_view->addEventHandler(_pickHandler);
	_view->addEventHandler(new osgViewer::StatsHandler);
	_view->addEventHandler(new osgViewer::WindowSizeHandler);
	_view->addEventHandler(new osgViewer::ThreadingHandler());
	_view->addEventHandler(new osgViewer::LODScaleHandler());

}

void earthWidget::earthRun()
{
	_view->setSceneData(_root.get());
}

void earthWidget::paintGL()
{
	_osgLock.lock();
	updateScene();
	frame();
	update();
	_osgLock.unlock();
}

void earthWidget::updateScene()
{
	updateSceneTime();
}

void earthWidget::updateSceneTime()
{
	osgEarth::DateTime tt(QDateTime::currentDateTime().toTime_t()/* + 8 * 60 * 60*/ + (_count++)*mInterval);
	_sky->setDateTime(tt);
	osgEarth::DateTime date = _sky->getDateTime();
	double deltaS = floor(float(tt.asTimeStamp() - _reveTime));
	earthRotationDegree = (deltaS)*   0.00416666666;

	//地球自转RsR2D
	//_pEarthMan->setByMatrix(_pEarthMan->getMatrix()*osg::Matrix::rotate(osg::inDegrees(-earthRotationDegree), osg::Z_AXIS));
	_reveTime = tt.asTimeStamp();

}


void earthWidget::setEarthManipulator()
{
	//切换视角
	_pEarthMan = new osgEarth::Util::EarthManipulator;
	if (NULL != _pMapNode)
	{
		_pEarthMan->setNode(_pMapNode);
	}
	
	_view->setCameraManipulator(_pEarthMan);
	_pEarthMan->getSettings()->setArcViewpointTransitions(true);

	//赤道半径
	double equatorRadius = _pMapNode->getMap()->getSRS()->getEllipsoid()->getRadiusEquator();//6378137.0
	//初始视点正对中国西安
	_pEarthMan->setHomeViewpoint(osgEarth::Util::Viewpoint("", 108.93, 34.27, 0, 0, -90, equatorRadius * 4), 5);
	_pEarthMan->setViewpoint(osgEarth::Viewpoint("", 108.93, 34.27, 0, 0, -90, equatorRadius * 4), 3);
	//_pEarthMan->getSettings()->setTetherMode(EarthManipulator::TETHER_CENTER_AND_ROTATION);
	_pEarthMan->getSettings()->getBreakTetherActions().push_back(EarthManipulator::ACTION_GOTO);
	//_pEarthMan->setTetherCallback(new osgEarth::Util::EarthManipulator::TetherCallback);

}

void earthWidget::creatCanvas()
{
	//控件绘制容器
	_canvas = new ControlCanvas();
	//将要显示的控件加入到root组节点中去
	_root->addChild(_canvas);

	//飞机和航线的起点矩阵  坐标西安  高度6000
	osgEarth::GeoPoint point(m_pSrclatLong, 108.93, 34.27, 6000.0, osgEarth::ALTMODE_ABSOLUTE);
	point.createLocalToWorld(mtXian);

}

void earthWidget::addLabel()
{
	// Style our labels:
	osgEarth::Symbology::Style labelStyle;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_CENTER;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Symbology::Color::Yellow;
	pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::White;
	pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 1.0f;
	pathStyle.getOrCreate<LineSymbol>()->tessellationSize() = 75000;
	pathStyle.getOrCreate<PointSymbol>()->size() = 2;
	pathStyle.getOrCreate<PointSymbol>()->fill()->color() = Color::White;
	pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;

	//添加北京和西安地标
	osgEarth::Symbology::Style pm;
	pm.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral("../data/placemark32.png");
	pm.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter() = true;
	pm.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo() = osgEarth::Symbology::Color("#5f5f5f");

	// bunch of pins:
	_root->addChild(new osgEarth::Annotation::PlaceNode(_pMapNode, osgEarth::GeoPoint(m_pGeoSRS, 117.5, 39.38), "Beijing", pm));
	_root->addChild(new osgEarth::Annotation::PlaceNode(_pMapNode, osgEarth::GeoPoint(m_pGeoSRS, 108.93, 34.27), "Xian", pm));
}

void earthWidget::createLabels()
{
	//下面是设置一个控件，grid的意思是用格网去布局里面的小控件
	// Make the readout:
	//grid = new showInfoUpdate();
	//_canvas->addChild(grid);

	/*
	//grid->getOrCreateStateSet()->
	//设置几个Label文字控件显示在场景中的第行
	osg::Vec4f vcolor(255.0,255.0,255.0,1.0);
	grid->setControl(0, 0, new LabelControl("UAV:"));
	grid->setControl(0, 1, new LabelControl("Longitude:"));
	grid->setControl(0, 2, new LabelControl("Latitude:"));
	grid->setControl(0, 3, new LabelControl("Elevation:"));
	grid->setControl(0, 4, new LabelControl("Heading:"));	//航向
	grid->setControl(0, 7, new LabelControl("Speed:"));	
	grid->setControl(0, 5, new LabelControl("Pitch:"));
	grid->setControl(0, 6, new LabelControl("Roll:"));
	//设置几个Label文字控件显示在场景中的第行
	LP_UAV_name = grid->setControl(1, 0, new LabelControl("", osg::Vec4(1.0, 1.0, 1.0, 1.0)));
	LP_UAV_Longitude = grid->setControl(1, 1, new LabelControl("", vcolor));
	LP_UAV_Latitude = grid->setControl(1, 2, new LabelControl("", osg::Vec4(1.0, 0.0, 0.0, 1.0)));
	LP_UAV_Elevation = grid->setControl(1, 3, new LabelControl(""));
	LP_UAV_Heading   = grid->setControl(1, 4, new LabelControl(""));
	LP_UAV_Velocity  = grid->setControl(1, 7, new LabelControl(""));
	LP_UAV_Pitch     = grid->setControl(1, 5, new LabelControl(""));
	LP_UAV_Roll      = grid->setControl(1, 6, new LabelControl(""));

	grid->setControl(2, 0, new LabelControl("Ship:"));
	grid->setControl(2, 1, new LabelControl("Longitude:"));
	grid->setControl(2, 2, new LabelControl("Latitude:"));
	grid->setControl(2, 3, new LabelControl("Elevation:"));
	grid->setControl(2, 4, new LabelControl("Heading:"));
	grid->setControl(2, 5, new LabelControl("Speed:"));
	LP_Ship_name      = grid->setControl(3, 0, new LabelControl(""));
	LP_Ship_Longitude = grid->setControl(3, 1, new LabelControl(""));		//经度
	LP_Ship_Latitude  = grid->setControl(3, 2, new LabelControl(""));		//纬度
	LP_Ship_Elevation = grid->setControl(3, 3, new LabelControl(""));		//高程
	LP_Ship_Heading   = grid->setControl(3, 4, new LabelControl(""));		//高程
	LP_Ship_Velocity  = grid->setControl(3, 5, new LabelControl(""));		//速度

	grid->setControl(4, 0, new LabelControl("Vehicle:"));
	grid->setControl(4, 1, new LabelControl("Longitude:"));
	grid->setControl(4, 2, new LabelControl("Latitude:"));
	grid->setControl(4, 3, new LabelControl("Elevation:"));
	grid->setControl(4, 4, new LabelControl("Heading:"));
	grid->setControl(4, 5, new LabelControl("Speed:"));
	LP_Vehicle_name      = grid->setControl(5, 0, new LabelControl(""));
	LP_Vehicle_Longitude = grid->setControl(5, 1, new LabelControl(""));		//经度
	LP_Vehicle_Latitude  = grid->setControl(5, 2, new LabelControl(""));		//纬度
	LP_Vehicle_Elevation = grid->setControl(5, 3, new LabelControl(""));		//高程
	LP_Vehicle_Heading   = grid->setControl(5, 4, new LabelControl(""));		//高程
	LP_Vehicle_Velocity  = grid->setControl(5, 5, new LabelControl(""));		//速度

	*/
}

void earthWidget::addUAVWithoutTrail(string _name, QVector<STRPoint> _vecPoint)
{

	STRPlane* strUAV = new STRPlane(_name, _vecPoint.at(0).longitude, _vecPoint.at(0).latitude, _vecPoint.at(0).altitude + DEFUAVOFFSET);

	osg::ref_ptr<osg::MatrixTransform> mtRootUAV = new osg::MatrixTransform;

	//mtRootUAV->setUserData(createTrailLine());
	mtRootUAV->setName(_name);
	//加入到飞机根节点
	m_PlaneRoot->addChild(mtRootUAV);

	osg::ref_ptr<osg::Group> uavGroup = new osg::Group;
	uavGroup->addChild(nodeUAV);

	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
	mtself->setMatrix(osg::Matrixd::scale(100, 100, 100)/**osg::Matrixd::translate(0.0, 0.0, )*/);
	mtself->addChild(uavGroup);
	//mtself->setUserData(strUAV);

	//转动矩阵
	osg::ref_ptr<osg::MatrixTransform>  mtRotateface = new osg::MatrixTransform;
	mtRotateface->addChild(mtself);
	mtRotateface->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(0.f), osg::X_AXIS,
		osg::DegreesToRadians(0.f), osg::Y_AXIS,
		osg::DegreesToRadians(0.0f), osg::Z_AXIS));
	//g_mtUAVRotate = mtRotateface;
	osg::ref_ptr<osg::MatrixTransform> mtUAV = new osg::MatrixTransform;
	mtUAV->addChild(mtRotateface);
	//mtUAV->setUserData(createTrailLine());

	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint point(latLong, strUAV->longitude, strUAV->latitude, strUAV->altitude, osgEarth::ALTMODE_ABSOLUTE);
	osg::Matrix matrix;
	point.createLocalToWorld(matrix);

	mtUAV->setMatrix(matrix);
	mtUAV->setUserData(strUAV);
	mtRootUAV->addChild(mtUAV);

	//UAVCALLBACKUpdateTrail* ut = new UAVCALLBACKUpdateTrail(mtUAV, mtRotateface, m_pTrackNode, strUAV, propeller, dRudder_L, createTrail(EOPLANE, _name, _vecPoint));
	UAVCallBack* ut = new UAVCallBack(mtRootUAV, createTrail(EOPLANE, _name, _vecPoint));
	//UAVCallBack* ut = new UAVCallBack(mtUAV, mtRotateface, m_pTrackNode, strUAV, propeller, dRudder_L, createTrail(EOPLANE, _name, _vecPoint));
	mtUAV->addUpdateCallback(ut);

}
//void earthWidget::addUAV(string name, double _long, double _lat, double _altitude)
//{
//	G_STRPlane.longitude = _long;
//	G_STRPlane.latitude = _lat;
//	G_STRPlane.altitude = _altitude;
//
//	osg::ref_ptr<osg::MatrixTransform> mtRootUAV = new osg::MatrixTransform;
//	m_PlaneRoot->addChild(mtRootUAV);
//
//	nodeUAV = osgDB::readNodeFile("./moudles/heron.FLT");
//	if (NULL == nodeUAV)
//	{
//		QMessageBox::about(NULL, "Error", "Can not open heron.FLT file");
//		return;
//	}
//	nodeUAV->setName(name);
//
//	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
//	mtself->setMatrix(osg::Matrixd::scale(300, 300, 300)*osg::Matrixd::rotate(osg::DegreesToRadians(-45.0), osg::Vec3(0.0, 0.0, 1.0))*\
//			osg::Matrix::translate(osg::Vec3(170,485,0)));
//	mtself->addChild(nodeUAV);
//
//	osg::ref_ptr<osg::MatrixTransform> mtUAV = new osg::MatrixTransform;
//	mtUAV->addChild(mtself);
//
//	osg::ref_ptr<osg::MatrixTransform> mtLine = new osg::MatrixTransform;
//	//mtLine->addChild(createTrailLine());
//	osg::Node* goede = createTrailLine();
//	mtLine->addChild(goede);
//
//	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
//	osgEarth::GeoPoint point(latLong, G_STRPlane.longitude, G_STRPlane.latitude, G_STRPlane.altitude, osgEarth::ALTMODE_ABSOLUTE);
//	osg::Matrix matrix;
//	point.createLocalToWorld(matrix);
//	osg::Vec3 c3 = matrix.getTrans();
//	mtLine->setMatrix(osg::Matrixd::translate(matrix.getTrans())*osg::Matrixd::translate(osg::Vec3(0, 100, 400)));
//	mtUAV->setMatrix(osg::Matrixd::translate(matrix.getTrans()));
//
//	mtRootUAV->addChild(mtUAV);
//	mtRootUAV->addChild(mtLine);
//
//	m_pTrackUAV = new osgGA::NodeTrackerManipulator;
//	m_pTrackUAV->setHomePosition(osg::Vec3(-10000, -5000, 3500), osg::Vec3(), osg::Z_AXIS);
//	m_pTrackUAV->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
//	m_pTrackUAV->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
//	m_pTrackUAV->setTrackNode(nodeUAV);
//	UAVTrailCallBack = new UAVTrail(/*positionesave*/dynamic_cast<osg::Geode*>(goede), m_pTrackUAV);
//	mtUAV->addUpdateCallback(UAVTrailCallBack);
//}

//void earthWidget::addShip(string name, double _long, double _lat, double _altitude)
//{
//	G_STRShip.longitude = _long;
//	G_STRShip.latitude = _lat;
//	G_STRShip.altitude = _altitude;
//
//	osg::ref_ptr<osg::MatrixTransform> mtRootShip = new osg::MatrixTransform;
//	m_ShipRoot->addChild(mtRootShip);
//
//	nodeShip = osgDB::readNodeFile("./moudles/054Ship.ive");
//	if (NULL == nodeShip)
//	{
//		QMessageBox::about(NULL, "Error", "Can not open cow.osg file");
//		return;
//	}
//	nodeShip->setName(name);
//
//	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
//	mtself->setMatrix(osg::Matrixd::scale(3, 3, 3) * osg::Matrixd::rotate(osg::DegreesToRadians(-50.0), osg::Vec3(0, 0, 1))*\
//		osg::Matrixd::translate(osg::Vec3(210, 650, 340)));
//	mtself->addChild(nodeShip);
//
//	osg::ref_ptr<osg::MatrixTransform> mtship = new osg::MatrixTransform;
//	mtship->addChild(mtself);
//
//	osg::ref_ptr<osg::MatrixTransform> mtLine = new osg::MatrixTransform;
//	//mtLine->addChild(createTrailLine());
//	osg::Node* goede = createTrailLine();
//	setTrailStyle(goede, osg::Vec4(129.0 / 255.0, 194.0 / 255.0, 194.0 / 255.0, 0.7));
//	mtLine->addChild(goede);
//
//	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
//	osgEarth::GeoPoint point(latLong, G_STRShip.longitude, G_STRShip.latitude, G_STRShip.altitude, osgEarth::ALTMODE_ABSOLUTE);
//	osg::Matrix matrix;
//	point.createLocalToWorld(matrix);
//
//	mtLine->setMatrix(osg::Matrixd::translate(matrix.getTrans())*osg::Matrixd::translate(osg::Vec3(0, 0, 550)));
//	mtship->setMatrix(osg::Matrixd::translate(matrix.getTrans()));
//
//	mtRootShip->addChild(mtship);
//	mtRootShip->addChild(mtLine);
//
//	m_pTrackShip = new osgGA::NodeTrackerManipulator;
//	m_pTrackShip->setHomePosition(osg::Vec3(-600, -500, 800), osg::Vec3(), osg::Z_AXIS);
//	m_pTrackShip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
//	m_pTrackShip->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
//	m_pTrackShip->setTrackNode(nodeShip);
//
//	ShipTrail* shipTrail = new ShipTrail(dynamic_cast<osg::Geode*>(goede), m_pTrackShip);
//	mtship->addUpdateCallback(shipTrail);
//
//}
//void earthWidget::addVehicle(string name, double _long, double _lat, double _altitude)
//{
//	G_STRVehicle.longitude = _long;
//	G_STRVehicle.latitude = _lat;
//	G_STRVehicle.altitude = _altitude;
//
//	osg::ref_ptr<osg::MatrixTransform> mtRootVehicle = new osg::MatrixTransform;
//	m_VehicleRoot->addChild(mtRootVehicle);
//
//	nodeVehicle = osgDB::readNodeFile("./moudles/Tank.ive");
//	if (NULL == nodeVehicle)
//	{
//		QMessageBox::about(NULL, "Error", "Can not open glider.osg file");
//		return;
//	}
//	nodeVehicle->setName(name);
//
//	//旋转，缩放矩阵
//	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
//	//mtself->setMatrix(osg::Matrixd::scale(30, 30, 30)*osg::Matrixd::rotate(osg::DegreesToRadians(120.0), osg::Vec3(0, 0, 1))*\
//			//	osg::Matrixd::translate(osg::Vec3(30.0, 480.0, 345.0))));
//	mtself->setMatrix(osg::Matrixd::scale(30, 30, 30)*osg::Matrixd::rotate(osg::DegreesToRadians(120.0), osg::Vec3(0, 0, 1))*  \
//		osg::Matrixd::translate(osg::Vec3(0.0, 0.0, 345.0)));
//	mtself->addChild(nodeVehicle);
//
//	//车矩阵
//	osg::ref_ptr<osg::MatrixTransform> mtvehicle = new  osg::MatrixTransform;
//	mtvehicle->addChild(mtself);
//
//	osg::ref_ptr<osg::MatrixTransform> mtLine = new osg::MatrixTransform;
//	osg::Node* goede = createTrailLine();
//	setTrailStyle(goede, osg::Vec4(53.0 / 255.0, 85.0 / 255.0, 20.0 / 255.0, 0.7));
//	mtLine->addChild(goede);
//
//	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
//	//osgEarth::GeoPoint point(latLong, G_STRVehicle.longitude, G_STRVehicle.latitude, G_STRVehicle.altitude, osgEarth::ALTMODE_ABSOLUTE);
//	osgEarth::GeoPoint point(latLong, G_STRVehicle.longitude, G_STRVehicle.latitude, G_STRVehicle.altitude, osgEarth::ALTMODE_ABSOLUTE);
//	osg::Matrix matrix;
//	point.createLocalToWorld(matrix);
//
//	mtLine->setMatrix(osg::Matrixd::translate(matrix.getTrans())*osg::Matrixd::translate(osg::Vec3(0, 0, 560)));
//	mtvehicle->setMatrix(osg::Matrixd::translate(matrix.getTrans()));
//
//	mtRootVehicle->addChild(mtvehicle);
//	mtRootVehicle->addChild(mtLine);
//
//	m_pTrackVehicle = new osgGA::NodeTrackerManipulator;
//	m_pTrackVehicle->setHomePosition(osg::Vec3(-600, -500, 800), osg::Vec3(), osg::Z_AXIS);
//	m_pTrackVehicle->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
//	m_pTrackVehicle->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);
//	m_pTrackVehicle->setTrackNode(nodeVehicle);
//
//	VehicleTrail* VehicleTrailCallBack = new VehicleTrail(dynamic_cast<osg::Geode*>(goede), m_pTrackVehicle);
//	mtvehicle->addUpdateCallback(VehicleTrailCallBack);
//
//	//BuildTail(osg::Vec3(0, -10, 550), mtvehicle, mtRootVehicle);
//	//BuildTail(osg::Vec3(0, 10, 500), mtvehicle, mtRootVehicle);
//}

void earthWidget::addUAVByTrail(string _name, QVector<STRPoint> _vecPoint)
{
	osg::ref_ptr<osg::Node> trailNode = createTrail(EOPLANE, _name, _vecPoint);

	STRPlane* strUAV = new STRPlane(_name, _vecPoint.at(0).longitude, _vecPoint.at(0).latitude, _vecPoint.at(0).altitude + DEFUAVOFFSET);

	osg::ref_ptr<osg::MatrixTransform> mtRootUAV = new osg::MatrixTransform;
	mtRootUAV->setUserData(trailNode);
	mtRootUAV->setName(_name);
	//加入到飞机根节点
	m_PlaneRoot->addChild(mtRootUAV);

	osg::ref_ptr<osg::Group> uavGroup = new osg::Group;
	uavGroup->addChild(nodeUAV);

	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
	mtself->setMatrix(osg::Matrixd::scale(100, 100, 100)/**osg::Matrixd::translate(0.0, 0.0, )*/);
	mtself->addChild(uavGroup);

	//转动矩阵
	osg::ref_ptr<osg::MatrixTransform>  mtRotateface = new osg::MatrixTransform;
	mtRotateface->addChild(mtself);
	mtRotateface->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(0.f), osg::X_AXIS,
		osg::DegreesToRadians(0.f), osg::Y_AXIS,
		osg::DegreesToRadians(0.0f), osg::Z_AXIS));
	//g_mtUAVRotate = mtRotateface;
	osg::ref_ptr<osg::MatrixTransform> mtUAV = new osg::MatrixTransform;
	mtUAV->addChild(mtRotateface);

	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint point(latLong, strUAV->longitude, strUAV->latitude, strUAV->altitude, osgEarth::ALTMODE_ABSOLUTE);
	osg::Matrix matrix;
	point.createLocalToWorld(matrix);

	mtUAV->setMatrix(matrix);
	mtUAV->setUserData(strUAV);
	mtRootUAV->addChild(mtUAV);

	

	//osg::ref_ptr<osgSim::DOFTransform> pro = new osgSim::DOFTransform;
	//pro->addChild(propeller);
	//
	//osg::ref_ptr<osgSim::DOFTransform> dru = new osgSim::DOFTransform;
	//dru->addChild(dRudder_L);

	UVATrailCallBack* ut = new UVATrailCallBack(mtUAV, mtRotateface, m_pTrackNode, strUAV, propeller, dRudder_L);
	mtUAV->addUpdateCallback(ut);

	//grid->updateInfo(EOPLANE, _name, strUAV->longitude, strUAV->latitude, strUAV->altitude, strUAV->heading, strUAV->pitching, strUAV->roll, strUAV->speed);
}


//void earthWidget::delUAV(string name)
//{
//	if (name == nodeUAV->getName())
//	{
//		G_mutex.lock();
//		m_TrailRoot->removeChild(pathUAVNode);
//
//		osg::ref_ptr<osg::MatrixTransform> mtroot = dynamic_cast<osg::MatrixTransform*>(m_PlaneRoot->getChild(0));
//		osg::ref_ptr<osg::MatrixTransform> mtObject = dynamic_cast<osg::MatrixTransform*>(mtroot->getChild(0));
//		osg::ref_ptr<osg::Callback> nc = mtObject->getUpdateCallback();
//		mtObject->removeUpdateCallback(nc);
//		m_PlaneRoot->removeChild(mtroot);
//		G_mutex.unlock();
//		if (EOPLANE == currentFollowObject)
//		{
//			_view->setCameraManipulator(_pEarthMan);
//			_pEarthMan->getSettings()->getBreakTetherActions().push_back(EarthManipulator::ACTION_GOTO);
//			_pEarthMan->setViewpoint(osgEarth::Util::Viewpoint("", G_STRPlane.longitude, G_STRPlane.latitude, 0, 0, -90, 100000), 2);
//			currentFollowObject = 0;
//		}
//		
//	}
//	
//	
//
//}

void earthWidget::addShipByTrail(string _name, QVector<STRPoint> _vecPoint)
{

	createTrail(EOSHIP, _name, _vecPoint);

	STRShip* strSHIP = new STRShip(_name, _vecPoint.at(0).longitude, _vecPoint.at(0).latitude, _vecPoint.at(0).altitude + DEFSHIPOFFSET);

	osg::ref_ptr<osg::MatrixTransform> mtRootShip = new osg::MatrixTransform;
	mtRootShip->setName(_name);
	mtRootShip->setUserData(strSHIP);
	m_ShipRoot->addChild(mtRootShip);

	osg::ref_ptr<osg::Group> shipGroup = new osg::Group;
	shipGroup->addChild(nodeShip);

	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
	mtself->setMatrix(osg::Matrixd::scale(20, 20, 20));
	mtself->addChild(shipGroup);

	//转动矩阵
	osg::ref_ptr<osg::MatrixTransform>  mtRotateface = new osg::MatrixTransform;
	mtRotateface->addChild(mtself);
	mtRotateface->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(0.f), osg::X_AXIS,
		osg::DegreesToRadians(0.f), osg::Y_AXIS,
		osg::DegreesToRadians(0.f), osg::Z_AXIS));
	//g_mtShipRotate = mtRotateface;
	osg::ref_ptr<osg::MatrixTransform> mtship = new osg::MatrixTransform;
	mtship->addChild(mtRotateface);


	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint point(latLong, strSHIP->longitude, strSHIP->latitude, strSHIP->altitude, osgEarth::ALTMODE_ABSOLUTE);
	osg::Matrix matrix;
	point.createLocalToWorld(matrix);

	mtship->setMatrix(matrix);
	mtRootShip->addChild(mtship);

	ShipTrailCallBack* st = new ShipTrailCallBack(mtship, mtRotateface, m_pTrackNode, strSHIP);
	mtship->addUpdateCallback(st);

	//oceanDialog->addShip(_name);
	//grid->updateInfo(EOSHIP, _name, strSHIP->longitude, strSHIP->latitude, strSHIP->altitude, strSHIP->heading, 0.0, 0.0, strSHIP->speed);
}

void earthWidget::addVehicleByTrail(string _name, QVector<STRPoint> _vecPoint)
{

	createTrail(EOVEHICLE, _name, _vecPoint);

	STRVehicle* strVehicle = new STRVehicle(_name, _vecPoint.at(0).longitude, _vecPoint.at(0).latitude, _vecPoint.at(0).altitude + DEFVEHICLEOFFSET);

	osg::ref_ptr<osg::MatrixTransform> mtRootVehicle = new osg::MatrixTransform;
	mtRootVehicle->setName(_name);
	mtRootVehicle->setUserData(strVehicle);
	m_VehicleRoot->addChild(mtRootVehicle);

	osg::ref_ptr<osg::Group> vehicleGroup = new osg::Group;
	vehicleGroup->addChild(nodeVehicle);

	//缩放矩阵
	osg::ref_ptr<osg::MatrixTransform> mtself = new osg::MatrixTransform;
	mtself->setMatrix(osg::Matrixd::scale(200, 200, 200));
	mtself->addChild(vehicleGroup);

	//转动矩阵
	osg::ref_ptr<osg::MatrixTransform> mtRotateface = new osg::MatrixTransform;
	mtRotateface->addChild(mtself);
	mtRotateface->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(0.f), osg::X_AXIS,
		osg::DegreesToRadians(0.f), osg::Y_AXIS,
		osg::DegreesToRadians(180.f), osg::Z_AXIS));
	//g_mtVehicleRotate = mtRotateface;
	//车矩阵
	osg::ref_ptr<osg::MatrixTransform> mtvehicle = new  osg::MatrixTransform;
	mtvehicle->addChild(mtRotateface);

	const osgEarth::SpatialReference* latLong = osgEarth::SpatialReference::get("wgs84");
	osgEarth::GeoPoint point(latLong, strVehicle->longitude, strVehicle->latitude, strVehicle->altitude, osgEarth::ALTMODE_ABSOLUTE);
	osg::Matrix matrix;
	point.createLocalToWorld(matrix);

	mtvehicle->setMatrix(matrix);
	mtRootVehicle->addChild(mtvehicle);
	
	VehicleTrailCallBack* vt = new VehicleTrailCallBack(mtvehicle, mtRotateface, m_pTrackNode, strVehicle);
	mtvehicle->addUpdateCallback(vt);

	//grid->updateInfo(EOVEHICLE, _name, strVehicle->longitude, strVehicle->latitude, strVehicle->altitude, strVehicle->heading, 0.0, 0.0, strVehicle->speed);
}

static double a = 0;
static double b = 0;
static double c = 0;

void earthWidget::rotateX(double x)
{
	a += x;

	//osg::Vec3d Veeye, Vecenter, Veup;
	//m_pTrackVehicle->getHomePosition(Veeye, Vecenter, Veup);
	//Veeye.x() += x;
	//m_pTrackVehicle->setHomePosition(Veeye, Vecenter, Veup);
	//cout << "x: " << Veeye.x() << endl;
}

void earthWidget::rotateY(double y)
{
	b += y;
	//osg::Vec3d Veeye, Vecenter, Veup;
	//m_pTrackVehicle->getHomePosition(Veeye, Vecenter, Veup);
	//Veeye.y() += y;
	//m_pTrackVehicle->setHomePosition(Veeye, Vecenter, Veup);
	//cout << "y: " << Veeye.y() << endl;
}

void earthWidget::rotateZ(double z)
{
	c += z;
	//osg::Vec3d Veeye, Vecenter, Veup;
	//m_pTrackVehicle->getHomePosition(Veeye, Vecenter, Veup);
	//Veeye.z() += z;
	//m_pTrackVehicle->setHomePosition(Veeye, Vecenter, Veup);
	//cout << "z: " << Veeye.z() << endl;
}

void earthWidget::slot_isFollowPlane(bool isFollow)
{
	bIsFollow = isFollow;
	if (isFollow)
	{
		_view->setCameraManipulator(m_pTrackNode);
	} 
	else
	{
		_view->setCameraManipulator(_pEarthMan);
	}
}

void earthWidget::slot_addInterval()
{
	if (mInterval < 100)
	{
		mInterval += 10;
	}
}

void earthWidget::slot_subInterval()
{
	if (mInterval > 0)
	{
		mInterval -= 10;
	}
}

void earthWidget::test()
{
	
}




//不用
void earthWidget::slot_currentMousePos(osg::Vec3d mousePos)
{
	QString str;
	str = QString("%1 , + %2").arg(mousePos.x()).arg(mousePos.y());
	//s_posLabel->setText(str.toStdString());
}

void earthWidget::connectWidgets()
{
	//timer = new QTimer;
	//connect(timer, SIGNAL(timeout()), this, SLOT(slot_test()));
}


void earthWidget::updateUAVPosition(string name, osg::Vec3d v3d, double _heading, double _pitch, double _roll, double _speed)
{
	if (NULL != nodeUAV)
	{
		int size = m_PlaneRoot->getNumChildren();
		for (int i = 0; i < size; ++i)
		{
			if (name == m_PlaneRoot->getChild(i)->getName())
			{
				osg::MatrixTransform* mtUAVRoot = dynamic_cast<osg::MatrixTransform*>(m_PlaneRoot->getChild(i));
				STRPlane* sp  = dynamic_cast<STRPlane*>(mtUAVRoot->getChild(0)->getUserData());
				sp->longitude = v3d.x();
				sp->latitude  = v3d.y();
				sp->altitude  = v3d.z();
				sp->speed     = _speed;
				sp->heading   = _heading;
				sp->pitching  = _pitch;
				sp->roll      = _roll;

				//if (name == currentUAVName)
				//{
				//	grid->updateInfo(EOPLANE, name, sp->longitude, sp->latitude, v3d.z(), sp->heading, sp->pitching, sp->roll, sp->speed);
				//}

				return;
			}
		}
	}
	
}

void earthWidget::updateShipPosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	if (NULL != nodeShip)
	{
		int size = m_ShipRoot->getNumChildren();
		for (int i = 0; i < size; ++i)
		{
			if (name == m_ShipRoot->getChild(i)->getName())
			{
				osg::MatrixTransform* mtShipRoot = dynamic_cast<osg::MatrixTransform*>(m_ShipRoot->getChild(i));
				STRShip* ss = dynamic_cast<STRShip*>(mtShipRoot->getUserData());
				ss->longitude = _long;
				ss->latitude  = _lat;
				ss->speed     = _speed;
				ss->heading   = _heading;

				//if (name == currentShipName)
				//{
				//	grid->updateInfo(EOSHIP, name, ss->longitude, ss->latitude, _altitude, ss->heading, 0.0, 0.0, ss->speed);
				//}

				return;
			}
		}
	}
}

void earthWidget::updateVehiclePosition(string name, double _long, double _lat, double _altitude, double _heading, double _speed)
{
	if (NULL != nodeVehicle)
	{
		int size = m_VehicleRoot->getNumChildren();
		for (int i = 0; i < size; ++i)
		{
			if (name == m_VehicleRoot->getChild(i)->getName())
			{
				osg::MatrixTransform* mtVehicleRoot = dynamic_cast<osg::MatrixTransform*>(m_VehicleRoot->getChild(i));
				STRVehicle* sv = dynamic_cast<STRVehicle*>(mtVehicleRoot->getUserData());
				sv->longitude = _long;
				sv->latitude  = _lat;
				sv->speed     = _speed;
				sv->heading   = _heading;

				//if (name == currentVehicleName)
				//{
				//	grid->updateInfo(EOVEHICLE, name, sv->longitude, sv->latitude, _altitude, sv->heading, 0.0, 0.0, sv->speed);
				//}

				return;
			}
		}
	}
}

//void earthWidget::addPlanePath(osg::Vec3d vecStart, osg::Vec3d vecEnd)
//{
//	if (path == NULL)
//	{
//		path = new LineString();
//		path->push_back(vecStart);
//		path->push_back(vecEnd);
//		osg::ref_ptr<osgEarth::SpatialReference> str = SpatialReference::create("wgs84");
//
//		pathFeature = new Feature(path, str);
//		pathFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;
//		pathNode = new FeatureNode(_pMapNode, pathFeature, pathStyle);
//		//_planePath->addChild(pathNode);
//
//		osgEarth::GeoPoint point(str, 108, 34, 0, osgEarth::ALTMODE_ABSOLUTE);
//		osg::Matrix m;
//		point.createLocalToWorld(m);
//		osg::MatrixTransform* mt = new osg::MatrixTransform(m);
//		mt->addChild(pathNode);
//		_planePath->addChild(mt);
//	}
//	else
//	{
//		//pathFeature->getGeometry()->clear();
//		//pathNode->setStyle(pathStyle);
//		//for (int i = 0; i < m_Vector3D.size(); ++i)
//		//{
//		pathFeature->getGeometry()->push_back(m_Vector3D[m_Vector3D.size() - 1]);
//		//}
//	}
//
//	pathNode->init();	
//}

osg::Node* earthWidget::createTrailLine()
{
	// create the Geode (Geometry Node) to contain all our osg::Geometry objects.
	osg::Geode* geode = new osg::Geode();

	osg::Geometry* linesGeom = new osg::Geometry();
	linesGeom->setDataVariance(osg::Object::DYNAMIC);
	linesGeom->setUseDisplayList(false);
	linesGeom->setUseVertexBufferObjects(true);
	osg::Vec3Array* vertices = new osg::Vec3Array;
	linesGeom->setVertexArray(vertices);
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 0.7f));
	linesGeom->setColorArray(colors);
	linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	normals->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
	linesGeom->setNormalArray(normals);
	linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	// This time we simply use primitive, and hardwire the number of coords to use 
	// since we know up front,
	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 0));
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.0f));
	geode->addDrawable(linesGeom);
	geode->setCullingActive(false);

	return geode;
}

void earthWidget::setTrailStyle(osg::Node* node, osg::Vec4 vec4)
{
	osg::Geode* geode = dynamic_cast<osg::Geode*>(node);
	osg::Geometry* linesGeom = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
	osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(linesGeom->getColorArray());
	if (color != NULL &&color->size() > 0)
	{
		int a = color->size();
		osg::Vec4& _vec4 = color->at(0);
		_vec4 = vec4;
	}
	else
	{
		color->push_back(vec4);
	}
	
}

void earthWidget::slot_FollowOne(int whichOne)
{
	//currentFollowObject = whichOne;
	//if (bIsFollow)
	//{
	//	switch (currentFollowObject)
	//	{
	//	case EOPLANE:
	//		if (nullptr != nodeUAV)
	//		{
	//			_view->setCameraManipulator(m_pTrackNode);
	//			m_pTrackNode->setTrackNode(findMatrixByName(m_PlaneRoot, _name));
	//			m_pTrackNode->setDistance(15000);
	//			//m_pTrackUAV->setElevation(2000);
	//		}
	//		break;
	//	case EOSHIP:
	//		if (nullptr != nodeShip)
	//		{
	//			_view->setCameraManipulator(m_pTrackNode);
	//			m_pTrackShip->setDistance(15000);
	//			//m_pTrackShip->setHeading(30.0);
	//		}
	//		break;
	//	case EOVEHICLE:
	//		if (nullptr != nodeVehicle)
	//		{
	//			_view->setCameraManipulator(m_pTrackNode);
	//			m_pTrackVehicle->setDistance(10000);
	//			//_view->setCameraManipulator(_pEarthMan);		
	//			//_pEarthMan->setViewpoint(osgEarth::Util::Viewpoint("", G_STRVehicle.longitude, G_STRVehicle.latitude, 0, 0, -90, 3500), 2);
	//			//_pEarthMan->setTetherNode(nodeVehicle);
	//		}
	//	default:
	//		break;
	//	}
	//}
}

void earthWidget::BuildTail(osg::Vec3 position, osg::MatrixTransform* scaler, osg::MatrixTransform* root)
{
	osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(position, 10);
	fire->setUseLocalParticleSystem(false);
	fire->getEmitter()->setEndless(true);
	fire->getEmitter()->setLifeTime(1);

	fire->getParticleSystem()->getDefaultParticleTemplate().setLifeTime(fire->getParticleSystem()->getDefaultParticleTemplate().getLifeTime() * 5);
	scaler->addChild(fire);


	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(fire->getParticleSystem());
	root->addChild(geode);
}


void earthWidget::addViewPointLabel()
{

}

FeatureNode* earthWidget::createTrail(int _target, string _name, QVector<STRPoint> _vecPoint)
{
	int offset = 0;
	if (EOPLANE == _target)
	{
		offset = DEFUAVOFFSET + 250;
	}
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> pathNode = 0;	
	osg::ref_ptr<osgEarth::Symbology::Geometry> path = new LineString();
	for (auto& i : _vecPoint)
	{	
		path->push_back(osg::Vec3d(i.longitude, i.latitude, i.altitude + offset));
	}	

	osg::ref_ptr<osgEarth::Features::Feature> pathFeature = new Feature(path, m_pSrclatLong);
	pathFeature->geoInterp() = GEOINTERP_RHUMB_LINE;

	Style pathStyle;
	pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Red;
	pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 2.0f;
	//pathStyle.getOrCreate<LineSymbol>()->tessellationSize() = 10;

	switch (_target)
	{
	case EOPLANE:
		//pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_RELATIVE_TO_TERRAIN;
		pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_MAP;
		break;

	case EOSHIP:
	case EOVEHICLE:
		pathStyle.getOrCreate<LineSymbol>()->tessellationSize() = 10;
		pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;
		break;

	default:
		break;
	}
	
	//pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;

	pathNode = new FeatureNode(_pMapNode, pathFeature, pathStyle);
	pathNode->setName(_name);

	if (NULL  == m_TrailRoot)
	{
		m_TrailRoot = new osg::Group;
		_root->addChild(m_TrailRoot);
	}
	m_TrailRoot->addChild(pathNode);
	return pathNode;
}


void earthWidget::delObject(int _Object, string _name)
{
	delTrailByName(_name);
	switch (_Object)
		{
		case EOPLANE:
			delNodeByName(_name, m_PlaneRoot);		
			break;
		case EOSHIP:
			delNodeByName(_name, m_ShipRoot);
			break;
		case EOVEHICLE:
			delNodeByName(_name, m_VehicleRoot);
			break;
		default:
			break;
		}
}


void earthWidget::slot_setOceanWndVisble(bool isShow)
{
	//if (oceanDialog)
	//{
	//	oceanDialog->setVisible(isShow);
	//}
}

void earthWidget::delTrailByName(string _name)
{
	for (int i = 0; i < m_TrailRoot->getNumChildren(); ++i)
	{
		if (_name == m_TrailRoot->getChild(i)->getName())
		{
			m_TrailRoot->removeChild(i);
			return;
		}
	}
}

void earthWidget::delNodeByName(string _name, osg::Group* _group)
{
	for (int i = 0; i < _group->getNumChildren(); ++i)
	{
		if (_name == _group->getChild(i)->getName())
		{
			_osgLock.lock();
			//删除模型
			//osg::ref_ptr<osg::MatrixTransform> mtrootObject = dynamic_cast<osg::MatrixTransform*>(_group->getChild(0));
			//osg::ref_ptr<osg::MatrixTransform> mtObject = dynamic_cast<osg::MatrixTransform*>(mtrootObject->getChild(0));
			//osg::ref_ptr<osg::MatrixTransform> mtRotateface = dynamic_cast<osg::MatrixTransform*>(mtObject->getChild(0));
			//osg::ref_ptr<osg::MatrixTransform> mtself = dynamic_cast<osg::MatrixTransform*>(mtRotateface->getChild(0));
			_group->removeChild(i);
			_osgLock.unlock();
			return;
		}
	}
}

osg::ref_ptr<osg::MatrixTransform> earthWidget::findMatrixByName(osg::Group* _group, string _name)
{
	for (int i = 0; i < _group->getNumChildren(); ++i)
	{
		if (_name == _group->getChild(i)->getName())
		{
			osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(_group->getChild(i));
			return mt;
		}
	}

	return NULL;
}

void earthWidget::trackNode(int _type, string _name)
{
	switch (_type)
	{
	case EOPLANE:
		setTrackObject(m_PlaneRoot, _name, 4000);
		currentUAVName = _name;
		break;
	case EOSHIP:
		setTrackObject(m_ShipRoot, _name, 4000);
		currentShipName = _name;
		break;
	case EOVEHICLE:
		setTrackObject(m_VehicleRoot, _name, 4000);
		currentVehicleName = _name;
		break;
	default:
		break;
	}
}

void earthWidget::setTrackObject(osg::Group* _group, string _name, int _distance)
{
	osg::MatrixTransform* mt = findMatrixByName(_group, _name);
	if (NULL != mt)
	{
		osg::ref_ptr<osg::MatrixTransform> mtObject = dynamic_cast<osg::MatrixTransform*>(mt->getChild(0));
		osg::ref_ptr<osg::MatrixTransform> mtRotateface = dynamic_cast<osg::MatrixTransform*>(mtObject->getChild(0));
		osg::ref_ptr<osg::MatrixTransform> mtself = dynamic_cast<osg::MatrixTransform*>(mtRotateface->getChild(0));
		osg::ref_ptr<osg::Group> node = dynamic_cast<osg::Group*>(mtself->getChild(0));
		m_pTrackNode->setTrackNode(node);
		m_pTrackNode->setDistance(_distance);
	}
}

void earthWidget::delALLUAV()
{
	delALLSameNode(m_PlaneRoot);
}

void earthWidget::delALLShip()
{
	delALLSameNode(m_ShipRoot);
}

void earthWidget::delALLVehicle()
{
	delALLSameNode(m_VehicleRoot);
}

void earthWidget::delALLSameNode(osg::Group* group)
{
	for (int i = group->getNumChildren() - 1; i >= 0; --i)
	{
		string name = group->getChild(i)->getName();
		group->removeChild(group->getChild(i));
		delTrailByName(name);
	}
}

osg::Node* earthWidget::findNamedNode(const std::string& searchName, osg::Node* currNode)
{
	osg::Group* currGroup;
	osg::Node* foundNode;

	if (!currNode)
	{
		return NULL;
	}

	if (currNode->getName() == searchName)
	{
		return currNode;
	}

	currGroup = currNode->asGroup(); // returns NULL if not a group.
	if (currGroup)
	{
		for (unsigned int i = 0; i <
			currGroup->getNumChildren(); i++)
		{
			foundNode = findNamedNode(searchName,
				currGroup->getChild(i));
			if (foundNode)
				return foundNode; 
		}
		return NULL; 
	}
	else
	{
		return NULL; 
	}
}











