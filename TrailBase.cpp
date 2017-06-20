#include "TrailBase.h"

TrailBase::TrailBase(osg::MatrixTransform* _mtObjectRoot)
{
	latLong = osgEarth::SpatialReference::create("wgs84");
	strData = dynamic_cast<STRDATA*>(_mtObjectRoot->getUserData());
	mtObject = dynamic_cast<osg::MatrixTransform*>(_mtObjectRoot->getChild(0));
	if (NULL != mtObject)
	{
		mtRotate = dynamic_cast<osg::MatrixTransform*>(mtObject->getChild(0));	
	}
	_longitude  = strData->longitude;
	_latitude   = strData->latitude;
	_altitude   = strData->altitude;
	_speed      = 0.0;
	_heading    = 0.0;
	_headingPre = 0.0;
	_pitch      = 0.0;
	_pitchPre   = 0.0;
	_roll       = 0.0;
	_rollPre    = 0.0;
	
}

void TrailBase::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	updateCommonData();
	updateSelfData();

	osgEarth::GeoPoint point(latLong, _longitude, _latitude, _altitude, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
	osg::Matrix mt;
	point.createLocalToWorld(mt);
	mtObject->setMatrix(mt);
	
	mtRotate->setMatrix(osg::Matrixd::rotate(\
		osg::DegreesToRadians(-(_roll - _rollPre)), osg::X_AXIS, \
		osg::DegreesToRadians(-(_pitch - _pitchPre)), osg::Y_AXIS, \
		osg::DegreesToRadians(-(_heading - _headingPre) + _headingOffset), osg::Z_AXIS));
	_headingPre = _heading;
	_pitchPre = _pitch;
	_rollPre = _roll;

	traverse(node, nv);
}

void TrailBase::updateCommonData()
{
	G_mutex.lock();
	_longitude = strData->longitude;
	_latitude = strData->latitude;
	_altitude = strData->altitude;
	_heading = strData->heading;
	_pitch = strData->pitching;
	_roll = strData->roll;
	_speed = strData->speed;
	G_mutex.unlock();
}

void TrailBase::updateSelfData()
{
	//to do...
}
