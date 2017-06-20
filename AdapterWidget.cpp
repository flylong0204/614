#include "AdapterWidget.h"
#include <osgViewer/GraphicsWindow>
#include <QByteArray>
#include <QString>
#include <osgViewer/Viewer>
#include <osgEarth/Viewpoint>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/GeoData>
#include "global.h"

AdapterWidget::AdapterWidget(QWidget *parent)
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = width();
	traits->height = height();

	traits->doubleBuffer = true;
	traits->depth = 32;
	traits->samples = 16;
	_gw = new osgViewer::GraphicsWindowEmbedded(traits.get());
	setFocusPolicy(Qt::ClickFocus);
}

AdapterWidget::~AdapterWidget()
{

}

void AdapterWidget::resizeGL(int width, int height)
{
	_gw->getEventQueue()->windowResize(0, 0, width, height);
	_gw->resized(0, 0, width, height);
}

void AdapterWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		emit quitFullScreen(); //for operation 
		break;
	default:
		_gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toLatin1().data()));
	}
}

void AdapterWidget::keyReleaseEvent(QKeyEvent* event)
{
	_gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol) *(event->text()./*toAscii()*/toLatin1().data()));
}

void AdapterWidget::mouseMoveEvent(QMouseEvent* event)
{
	_gw->getEventQueue()->mouseMotion(event->x(), event->y());
	//QGLWidget::mouseMoveEvent(event);
}

void AdapterWidget::mousePressEvent(QMouseEvent* event)
{
	int button = 0;
	switch (event->button())
	{
	case(Qt::LeftButton) : button = 1; break;
	case(Qt::MidButton) : button = 2; break;
	case(Qt::RightButton) : button = 3; break;
	case(Qt::NoButton) : button = 0; break;
	default: button = 0; break;
	}
	_gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void AdapterWidget::mouseReleaseEvent(QMouseEvent* event)
{
	int button = 0;
	switch (event->button())
	{
	case(Qt::LeftButton) : button = 1; break;
	case(Qt::MidButton) : button = 2; break;
	case(Qt::RightButton) : button = 3; break;
	case(Qt::NoButton) : button = 0; break;
	default: button = 0; break;
	}
	_gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
	
}

void AdapterWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	QPoint pos = QCursor::pos();
	// 	QList<QGraphicsItem*> listItems = items(mapToScene(pos.x(), pos.y()).toPoint());
	// 	if (listItems.size() > 0)
	// 	{
	// 		return;
	// 	}
	int button = 0;
	switch (event->button())
	{
	case Qt::LeftButton: button = 1; break;
	case Qt::MidButton: button = 2; break;
	case Qt::RightButton: button = 3; break;
	case Qt::NoButton: button = 0; break;
	default: button = 0; break;
	}
	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseDoubleButtonPress(event->x(), event->y(), button);

	QGLWidget::mouseDoubleClickEvent(event);
}

void AdapterWidget::setKeyboardModifiers(QInputEvent* event)
{
	int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
	unsigned int mask = 0;
	if (modkey & Qt::ShiftModifier) mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;
	if (modkey & Qt::ControlModifier) mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;
	if (modkey & Qt::AltModifier) mask |= osgGA::GUIEventAdapter::MODKEY_ALT;

	_gw->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

void AdapterWidget::wheelEvent(QWheelEvent* event)
{
	setKeyboardModifiers(event);
	_gw->getEventQueue()->mouseScroll(
		event->orientation() == Qt::Vertical ?
		(event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN) :
		(event->delta() > 0 ? osgGA::GUIEventAdapter::SCROLL_LEFT : osgGA::GUIEventAdapter::SCROLL_RIGHT));
	QGLWidget::wheelEvent(event);
}

//updateEventHandler::updateEventHandler(osg::ref_ptr<GeoPositionNode> geo, osg::MatrixTransform* mtSelf)
//{
//	_geo = geo;
//	_mtSelf = mtSelf;
//}


//bool updateEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
//{
//	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
//	if (ea.getEventType() == ea.FRAME)
//	{
//		osgEarth::Util::EarthManipulator* em = dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
//		//if (nullptr != em)
//		//{
//		//	osgEarth::Viewpoint vp = em->getViewpoint();
//		//	osgEarth::GeoPoint gp = vp.focalPoint().get();
//		//	//std::cout << "vp x:" << gp.x() << ", y:" << gp.y() << ", z:" << gp.z() << ", Heading:" << vp.getHeading() << ", Pitch:" << vp.getPitch() << ", Range:" << vp.getRange() << endl;
//		//}
//		_geo->setPosition(GeoPoint(SpatialReference::create("wgs84"), osg::Vec3(G_STRVehicle.longitude, G_STRVehicle.latitude, G_STRVehicle.altitude)));
//	}
//
//	/*if (ea.getEventType() == ea.KEYDOWN)
//	{
//	if (ea.getButton() == )
//	{
//	}
//	}*/
//	return false;
//}




//构造函数 
MousePositionHandler::MousePositionHandler(osg::ref_ptr<const osgEarth::SpatialReference> srs)
{
	_srs = srs;
}

void MousePositionHandler::update(float x, float y, osgViewer::View* view)
{
	bool yes = false;

	// look under the mouse:
	//采用线去对地球做碰撞检测，根据鼠标点击点去检测，得到交点，就是当前点的xyz
	osg::Vec3d world;
	osgUtil::LineSegmentIntersector::Intersections hits;
	//判断求交结果是否为空
	if (view->computeIntersections(x, y, hits))
	{
		//得到世界坐标系下面的坐标，就是osg的xyz坐标
		world = hits.begin()->getWorldIntersectPoint();

		// convert to map coords:
		//将其转换为地球的地理坐标，转换方法都照抄即可
		GeoPoint mapPoint;
		mapPoint.fromWorld(_srs, world);

		// do an elevation query:
		double query_resolution = 0; // 1/10th of a degree
		double out_hamsl = 0.0;
		double out_resolution = 0.0;

		//根据输入参数查询当前点位置的高程，需要设置分辨率，就是查询精度
		//bool ok = _query.getElevation(
		//	mapPoint,
		//	out_hamsl,
		//	query_resolution,
		//	&out_resolution);

		//如果查询成功
		//if (ok)
		//{
		// convert to geodetic to get the HAE:
		mapPoint.z() = out_hamsl;
		GeoPoint mapPointGeodetic(_srs->getGeodeticSRS(), mapPoint);

		//经纬度坐标的格式化工具，也可以自己用字符串去拼接xyz数字
		//static LatLongFormatter s_f;

		//更新显示的xyz值，label是传入的控件
		_lat = mapPointGeodetic.y();
		_long = mapPointGeodetic.x();
		/*s_posLabel->setText(Stringify()
		<< std::fixed << std::setprecision(2)
		<< mapPointGeodetic.y()
		<< ", "
		<< mapPointGeodetic.x());*/

		cout << "lat long : " << _lat << _long << endl;

		//emit signal_latlong(_lat, _long);
		//还可以输出分辨率，椭球体信息等
		//s_mslLabel->setText(Stringify() << out_hamsl);
		//s_haeLabel->setText(Stringify() << mapPointGeodetic.z());
		//s_resLabel->setText(Stringify() << out_resolution);

		yes = true;
		//}

		// finally, get a normal ISECT HAE point.
		GeoPoint isectPoint;
		isectPoint.fromWorld(_srs->getGeodeticSRS(), world);
		//s_mapLabel->setText(Stringify() << isectPoint.alt());
	}

	//如果查询不到高程的话
	if (!yes)
	{
		/*s_posLabel->setText("-");
		s_mslLabel->setText("-");
		s_haeLabel->setText("-");
		s_resLabel->setText("-");*/
	}
}

bool MousePositionHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	//判断如果是移动鼠标事件才进行更新当前的坐标显示
	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE /*&&
														  aa.asView()->getFrameStamp()->getFrameNumber() % 10 == 0*/)
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		update(ea.getX(), ea.getY(), view);
	}

	return false;
}


MyTrailCallBack::MyTrailCallBack(osg::Geode* geode, osgGA::CameraManipulator* nodeTM)
{
}

void MyTrailCallBack::updateShowInfo()
{

}


void MyTrailCallBack::updateDrawLine()
{

}

void MyTrailCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	updateData();
	osg::MatrixTransform* mtTransform = static_cast<osg::MatrixTransform*>(node);
	osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geodeToFollow->getDrawable(0));
	osg::Vec3Array* ver3Array = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _height, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	mtTransform->setMatrix(mt);


	//无人机矩阵的转动
	//...
	updateShowInfo();
	if (nv && nv->getFrameStamp() && geodeToFollow)
	{
		if (frisFrame)
		{
			ver3Array->clear();
			frisFrame = false;
		}

		//osg::Matrix matrix = mtTransform->getMatrix();
		osg::Vec3 crtVec3 = mt.getTrans();
		osg::Vec3 deltaVec3 = crtVec3 - preVec3;
		ver3Array->push_back(deltaVec3);
		ver3Array->dirty();

		osg::DrawArrays* da = dynamic_cast<osg::DrawArrays*>(geometry->getPrimitiveSet(0));
		da->setFirst(0);
		da->setCount(ver3Array->size());
		da->dirty();
	}

	traverse(node, nv);
}

UAVTrail1::UAVTrail1(osg::Geode* geode, osgGA::CameraManipulator* nodeTM, STRDATA* _strData)
{
	geodeToFollow = geode;
	nodeTrack = nodeTM;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
	frisFrame = true;
	strData = _strData;
	_latitude = _strData->latitude;
	_longitude = _strData->longitude;
	_height = _strData->altitude;
	_speed = _strData->speed;
	_heading = _strData->heading;;
	_pitch = _strData->pitching;
	_roll = _strData->roll;
	_degreePropeller = 0.0;
	_degreeDRudder_L = 0.0;
	_crtPitch = 7.0;
	_crtHeading = 10;
	_crtRool = 10;

	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _height, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	preVec3 = mt.getTrans();
}

void UAVTrail1::updateData()
{
	G_mutex.lock();
	_longitude = strData->longitude;
	_latitude = strData->latitude;
	_height = strData->altitude;
	_heading = strData->heading;
	_pitch = strData->pitching;
	_roll = strData->roll;
	_speed = strData->speed;
	G_mutex.unlock();
}

VehicleTrail::VehicleTrail(osg::Geode* geode, osgGA::NodeTrackerManipulator* nodeTM, STRDATA* _strData)
{
	geodeToFollow = geode;
	nodeTrack = nodeTM;
	strData = _strData;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
	frisFrame = true;
	_latitude  = strData->latitude;
	_longitude = strData->longitude;
	_height    = strData->altitude;
	_speed = 100;
	_heading = 0.0;
	_pitch = 0.0;
	_roll = 0.0;
	_degreePropeller = 0.0;
	_degreeDRudder_L = 0.0;
	_crtPitch = 7.0;
	_crtHeading = 10;
	_crtRool = 10;

	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _height, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	preVec3 = mt.getTrans();
}

void VehicleTrail::updateData()
{
	G_mutex.lock();
	_longitude = strData->longitude;
	_latitude  = strData->latitude;
	_height    = strData->altitude;
	_heading   = strData->heading;
	_speed     = strData->speed;
	_heading = 0.0;
	_pitch = 0.0;
	_roll = 0.0;
	_degreePropeller = 0.0;
	_degreeDRudder_L = 0.0;
	_crtPitch = 7.0;
	_crtHeading = 10;
	_crtRool = 10;
	G_mutex.unlock();
	if (_degreePropeller >= 360)
	{
		_degreePropeller = 0;
	}
}

ShipTrail::ShipTrail(osg::Geode* geode, osgGA::NodeTrackerManipulator* nodeTM, STRDATA* _strData)
{
	geodeToFollow = geode;
	nodeTrack = nodeTM;
	strData = _strData;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
	frisFrame = true;
	_latitude  = strData->latitude;
	_longitude = strData->longitude;
	_height    = strData->altitude;
	_height = 100;
	_speed = 100;
	_heading = 0.0;
	_pitch = 0.0;
	_roll = 0.0;
	_degreePropeller = 0.0;
	_degreeDRudder_L = 0.0;
	_crtPitch = 7.0;
	_crtHeading = 10;
	_crtRool = 10;

	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, 0.0, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	preVec3 = mt.getTrans();
}

void ShipTrail::updateData()
{
	G_mutex.lock();
	_longitude = strData->longitude;
	_latitude  = strData->latitude;
	_height	   = strData->altitude;
	_heading = strData->heading;
	_speed = strData->speed;
	G_mutex.unlock();
	if (_degreePropeller >= 360)
	{
		_degreePropeller = 0;
	}
}


#if 0

void TrailBaseCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	updateData();
	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _height, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	mtSelf->setMatrix(mt);
	//mtRotate->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(-(_heading - _headingPre)), osg::Z_AXIS));
	mtRotate->setMatrix(osg::Matrixd::rotate(\
		osg::DegreesToRadians(-(_roll - _rollPre)),       osg::X_AXIS, \
		osg::DegreesToRadians(-(_pitch - _pitchPre)),     osg::Y_AXIS, \
		osg::DegreesToRadians(-(_heading - _headingPre) + _headingOffset), osg::Z_AXIS));
	_headingPre  = _heading;
	_pitchPre    = _pitch;
	_rollPre     = _roll;
	traverse(node, nv);
}

UVATrailCallBack::UVATrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, \
	osgGA::CameraManipulator* nodeTM, STRDATA* UAVdata, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L)
{
	mtSelf = _mtSelf;
	mtRotate = _mtRotate;
	nodeTrack = nodeTM;
	_speed = 0.0;
	_heading = 0.0;
	_headingPre = 0.0;
	_pitch = 0.0;
	_pitchPre = 0.0;
	_roll = 0.0;
	_rollPre = 0.0;
	_degreePropeller = 0;
	_degreeDRudder_L = 0;
	_headingOffset = 0.0;
	_UAVdata = UAVdata;
	propeller = _propeller;
	dRudder_L = _dRudder_L;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
}

void UVATrailCallBack::updateData()
{
	G_mutex.lock();
	_longitude = _UAVdata->longitude;
	_latitude  = _UAVdata->latitude;
	_height    = _UAVdata->altitude + DEFUAVOFFSET;
	_heading   = _UAVdata->heading;
	_pitch     = _UAVdata->pitching;
	_roll      = _UAVdata->roll;
	_speed     = _UAVdata->speed;
	G_mutex.unlock();
	if (_degreePropeller > 360){
		_degreePropeller = 0;
	}
	_degreePropeller += 20;   ///控制尾翼转动
	_degreeDRudder_L = 0;

	//无人机DOF
	if (propeller){
		propeller->setCurrentHPR(osg::Vec3(0.0, osg::DegreesToRadians(_degreePropeller), 0.0));
	}
	if (dRudder_L){
		dRudder_L->setCurrentHPR(osg::Vec3(0.0, 0.0, osg::DegreesToRadians(_degreeDRudder_L)));
	}
	
}

ShipTrailCallBack::ShipTrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRDATA* shipData)
{
	mtSelf = _mtSelf;
	mtRotate = _mtRotate;
	nodeTrack = nodeTM;
	_speed = 0.0;
	_heading = 0.0;
	_headingPre = 0.0;
	_pitch = 0.0;
	_pitchPre = 0.0;
	_roll = 0.0;
	_rollPre = 0.0;
	_headingOffset = 0.0;
	_shipData = shipData;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
}

void ShipTrailCallBack::updateData()
{
	G_mutex.lock();
	_longitude = _shipData->longitude;
	_latitude  = _shipData->latitude;
	_height    = _shipData->altitude;
	_heading   = _shipData->heading;
	_speed     = _shipData->speed;
	_pitch	   = 0.0;
	_roll      = 0.0;
	G_mutex.unlock();

}

VehicleTrailCallBack::VehicleTrailCallBack(osg::MatrixTransform* _mtSelf, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, STRDATA* VehicleData)
{
	mtSelf     = _mtSelf;
	mtRotate   = _mtRotate;
	nodeTrack  = nodeTM;
	_speed = 0.0;
	_heading = 0.0;
	_headingPre = 0.0;
	_pitch = 0.0;
	_pitchPre = 0.0;
	_roll = 0.0;
	_rollPre = 0.0;
	_headingOffset = 180.0;
	_VehicleData = VehicleData;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");

}

void VehicleTrailCallBack::updateData()
{
	G_mutex.lock();
	_longitude = _VehicleData->longitude;
	_latitude  = _VehicleData->latitude;
	_height    = _VehicleData->altitude;
	_heading   = _VehicleData->heading;
	_speed     = _VehicleData->speed;
	_pitch	   = 0.0;
	_roll	   = 0.0;
	G_mutex.unlock();

	mtRotate->setMatrix(osg::Matrixd::rotate(osg::DegreesToRadians(180.0), osg::Z_AXIS));
	//osgGA::NodeTrackerManipulator* nt = dynamic_cast<osgGA::NodeTrackerManipulator*> (nodeTrack);
	//osg::Vec3d Veeye, Vecenter, Veup;
	//nt->getHomePosition(Veeye, Vecenter, Veup);
	//cout << "Veeye: " << Veeye.x() << ", " << Veeye.y() << ", " << Veeye.z() << " --- center: " << Vecenter.x() << ", " << Vecenter.y() << ", " << Vecenter.z() << endl;
	//cout << "nt center:" << nt->getCenter().x() << "  " << nt->getCenter().y() << "  " << nt->getCenter().z() << endl;
	//cout << "heading:" << nt->getHeading()<< " " << nt->getDistance() << endl;

	//osgEarth::Util::EarthManipulator* em = dynamic_cast<osgEarth::Util::EarthManipulator*>(nodeTrack);
	//
	//osgEarth::Viewpoint vp = em->getViewpoint();
	//
	//cout << vp.focalPoint().get().x() << "---" << vp.focalPoint().get().y() << "---" << vp.focalPoint().get().z() \
	//	 << "---" << vp.getHeading() << "---" << vp.getPitch() << "---" << vp.getRange() << endl;

	//if (NULL != mtRotate)
	//{
	//	mtRotate->setMatrix(mtRotate->getMatrix()*osg::Matrixd::rotate(osg::DegreesToRadians(0.f), osg::X_AXIS,
	//		osg::DegreesToRadians(0.f), osg::Y_AXIS,
	//		osg::DegreesToRadians(-_heading), osg::Z_AXIS));
	//}
}





void UAVCALLBACKUpdateTrail::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	updateData();
	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _height, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	mtSelf->setMatrix(mt);

	mtRotate->setMatrix(osg::Matrixd::rotate(\
		osg::DegreesToRadians(-(_roll - _rollPre)), osg::X_AXIS, \
		osg::DegreesToRadians(-(_pitch - _pitchPre)), osg::Y_AXIS, \
		osg::DegreesToRadians(-(_heading - _headingPre) + _headingOffset), osg::Z_AXIS));
	_headingPre = _heading;
	_pitchPre = _pitch;
	_rollPre = _roll;

	osg::ref_ptr<osgEarth::Symbology::Geometry> _trail = featureNode->getFeature()->getGeometry();
	_trail->push_back(_longitude, _latitude, _height);
	featureNode->init();
	//osgEarth::Annotation::fea FeatureNode* geom = dynamic_cast<osg::Geometry*>(lineGeo->getDrawable(0));
	//osg::Vec3Array *vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

	//	if (nv && nv->getFrameStamp() && lineGeo)
	//	{
	//		if (frisFrame)
	//		{
	//			ver3Array->clear();
	//			frisFrame = false;
	//		}
	//
	//		//osg::Matrix matrix = mtTransform->getMatrix();
	//		osg::Vec3 crtVec3 = mt.getTrans();
	//		osg::Vec3 deltaVec3 = crtVec3 - preVec3;
	//		ver3Array->push_back(deltaVec3);
	//		ver3Array->dirty();
	//
	//		osg::DrawArrays* da = dynamic_cast<osg::DrawArrays*>(geometry->getPrimitiveSet(0));
	//		da->setFirst(0);
	//		da->setCount(ver3Array->size());
	//		da->dirty();

	//if (nv && lineGeo && nv->getFrameStamp())
	//{
	//	if (lineGeo){
	//		if (FirstFrame){
	//			vertices->clear();
	//			FirstFrame = false;
	//		}
	//
	//		osg::Vec3 crtVec3 = mt.getTrans();
	//		osg::Vec3 deltaVec3 = crtVec3 - preVec3;
	//		vertices->push_back(deltaVec3);
	//		vertices->dirty();
	//
	//		vertices->dirty();
	//		osg::DrawArrays * ff = dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0));
	//		ff->setFirst(0);
	//		ff->setCount(vertices->size());
	//		ff->dirty();
	//	}
	//}
	traverse(node, nv);
}

UAVCALLBACKUpdateTrail::UAVCALLBACKUpdateTrail(osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, osgGA::CameraManipulator* nodeTM, \
	STRDATA* UAVdata, osgSim::DOFTransform* _propeller, osgSim::DOFTransform* _dRudder_L, osgEarth::Annotation::FeatureNode* _featureNode)
{
	mtSelf = _mtObject;
	mtRotate = _mtRotate;
	nodeTrack = nodeTM;
	_speed = 0.0;
	_heading = 0.0;
	_headingPre = 0.0;
	_pitch = 0.0;
	_pitchPre = 0.0;
	_roll = 0.0;
	_rollPre = 0.0;
	_degreePropeller = 0;
	_degreeDRudder_L = 0;
	_headingOffset = 0.0;
	_UAVdata = UAVdata;
	propeller = _propeller;
	dRudder_L = _dRudder_L;
	FirstFrame = true;
	SrclatLong = osgEarth::SpatialReference::create("wgs84");
	//lineGeo = dynamic_cast<osg::Geode*>(_lineGeo);
	featureNode = _featureNode;

	//osgEarth::GeoPoint point(SrclatLong, UAVdata->longitude, UAVdata->latitude, UAVdata->altitude, osgEarth::ALTMODE_ABSOLUTE);
	//osg::Matrix matrix;
	//point.createLocalToWorld(matrix);
	//preVec3 = matrix.getTrans();
}

void callBackBase::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	updataCommonData();
	updateSelfData();

	osgEarth::GeoPoint point(SrclatLong, _longitude, _latitude, _altitude, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	mtSelf->setMatrix(mt);

	mtRotate->setMatrix(osg::Matrixd::rotate(\
		osg::DegreesToRadians(-(_roll - _rollPre)), osg::X_AXIS, \
		osg::DegreesToRadians(-(_pitch - _pitchPre)), osg::Y_AXIS, \
		osg::DegreesToRadians(-(_heading - _headingPre) + _headingOffset), osg::Z_AXIS));

	_headingPre = _heading;
	_pitchPre = _pitch;
	_rollPre = _roll;

	if (_needDrawLine)
	{
		osg::ref_ptr<osgEarth::Symbology::Geometry> _trail = featureNode->getFeature()->getGeometry();
		_trail->push_back(_longitude, _latitude, _altitude);
		featureNode->init();
	}
}

void callBackBase::updataCommonData()
{
	G_mutex.lock();
	_longitude = strData->longitude;
	_latitude  = strData->latitude;
	_altitude  = strData->altitude;
	_heading   = strData->heading;
	_pitch     = strData->pitching;
	_roll      = strData->roll;
	_speed     = strData->speed;
	G_mutex.unlock();
}

#endif
