#pragma once
#include <osg/NodeCallback>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Matrixd>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/MatrixTransform>
#include <osgEarth/SpatialReference>
#include <osgEarthAnnotation/FeatureNode>
#include <osgSim/DOFTransform>

#include "AdapterWidget.h"

//更新模型的回调
class TrailBase : public osg::NodeCallback
{
public:
	inline virtual ~TrailBase(){};
	TrailBase(osg::MatrixTransform* _mtObjectRoot);

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	virtual void updateCommonData();
	virtual void updateSelfData();

public:
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

	STRDATA* strData;
	osg::MatrixTransform* mtObject;
	osg::MatrixTransform* mtRotate;
	const osgEarth::SpatialReference* latLong;

	QMutex G_mutex;

private:
	inline TrailBase(){};
};

class UAVUpdateCallback : public TrailBase
{
public:

	inline virtual ~UAVUpdateCallback (){};
	inline UAVUpdateCallback(osg::MatrixTransform* _mtObjectRoot, osg::ref_ptr<osgSim::DOFTransform> _propeller = NULL, osg::ref_ptr<osgSim::DOFTransform> _dRudder_L = NULL, double _degreePropeller = 0.0, double _degreeDRudder_L = 0.0)
		: TrailBase(_mtObjectRoot), propeller(_propeller), dRudder_L(_dRudder_L), degreePropeller(_degreePropeller), degreeDRudder_L(_degreeDRudder_L){};
	virtual void updateSelfData()
	{
		if (degreePropeller > 360){
			degreePropeller = 0;
		}
		degreePropeller += 20;   
		degreeDRudder_L = 0;

		
		if (propeller)
		{
			propeller->setCurrentHPR(osg::Vec3(0.0, osg::DegreesToRadians(degreePropeller), 0.0));
		}
		if (dRudder_L)
		{
			dRudder_L->setCurrentHPR(osg::Vec3(0.0, 0.0, osg::DegreesToRadians(degreeDRudder_L)));
		}
	}

private:
	double degreePropeller;
	double degreeDRudder_L;
	osg::ref_ptr<osgSim::DOFTransform> propeller;
	osg::ref_ptr<osgSim::DOFTransform> dRudder_L;
};

class UpdataWithTrail : public TrailBase
{

public:

	inline virtual ~UpdataWithTrail(){};

	UpdataWithTrail(osg::MatrixTransform* _mtObjectRoot) : TrailBase(_mtObjectRoot)
	{
		firstFrame = true;

		osg::ref_ptr<osg::MatrixTransform> mtLine = dynamic_cast<osg::MatrixTransform*>(mtObject->getChild(1));
		geode = dynamic_cast<osg::Geode*>(mtLine->getChild(0));
		osgEarth::GeoPoint point(latLong, _longitude, _latitude, _altitude, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
		osg::Matrix mt;
		point.createLocalToWorld(mt);
		preVec3 = mt.getTrans();
	};
	
	virtual void updateSelfData()
	{
		osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
		osg::Vec3Array* ver3Array = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());

		osgEarth::GeoPoint point(latLong, _longitude, _latitude, _altitude, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
		osg::Matrix mt;
		point.createLocalToWorld(mt);

		if (firstFrame)
		{
			ver3Array->clear();
			firstFrame = false;
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

private:
	bool firstFrame;
	osg::Geode* geode;
	osg::Vec3d preVec3;
};




#if 0
class TrailCallback : public osg::NodeCallback
{
public:
	inline TrailCallback(){};
	inline virtual ~TrailCallback(){};

	TrailCallback(osg::Geode* _geode, osgGA::NodeTrackerManipulator * _track, osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, STRPlane* _strData)
	{
		mtRotate = _mtRotate;
		strData = _strData;
		mytracker = _track;
		objectToFollow = _geode;
		FirstFrame = true;
		latLong = osgEarth::SpatialReference::get("wgs84");
		
		_latitude      = strData->latitude;
		_longitude     = strData->longitude;
		_altitude	   = strData->altitude;
		_speed  	   = strData->speed;
		_heading	   = 0.0;
		_pitch		   = 0.0;
		_roll		   = 0.0;
		_headingPre	   = 0.0;
		_pitchPre	   = 0.0;
		_rollPre	   = 0.0;
		_headingOffset = 0.0;

		osgEarth::GeoPoint point(latLong, _longitude, _latitude, _altitude, osgEarth::ALTMODE_ABSOLUTE);
		osg::Matrix matrix;
		point.createLocalToWorld(matrix);
		preVec3 = matrix.getTrans();
	}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		updataCommonData();
		updateSelfData();

		osg::MatrixTransform* mtTransform = static_cast<osg::MatrixTransform*>(node);
		osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(objectToFollow->getDrawable(0));
		osg::Vec3Array* ver3Array = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
		osgEarth::GeoPoint point(latLong, _longitude, _latitude, _altitude, osgEarth::AltitudeMode::ALTMODE_ABSOLUTE);
		osg::Matrix mt;
		point.createLocalToWorld(mt);
		mtTransform->setMatrix(mt);

		//无人机矩阵的转动
		mtRotate->setMatrix(osg::Matrixd::rotate(\
			osg::DegreesToRadians(-(_roll - _rollPre)), osg::X_AXIS, \
			osg::DegreesToRadians(-(_pitch - _pitchPre)), osg::Y_AXIS, \
			osg::DegreesToRadians(-(_heading - _headingPre) + _headingOffset), osg::Z_AXIS));
		_headingPre = _heading;
		_pitchPre = _pitch;
		_rollPre = _roll;

		if (nv && nv->getFrameStamp() && objectToFollow)
		{
			if (FirstFrame)
			{
				ver3Array->clear();
				FirstFrame = false;
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
	};
	virtual void updataCommonData()
	{
		G_mutex.lock();
		_longitude = strData->longitude;
		_latitude  = strData->latitude;
		_altitude  = strData->altitude;
		_heading   = strData->heading;
		_pitch	   = strData->pitching;
		_roll	   = strData->roll;
		_speed	   = strData->speed;
		G_mutex.unlock();
	};
	virtual void updateSelfData() = 0;

public:

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
	bool FirstFrame;

	STRPlane* strData;
	osg::MatrixTransform* mtSelf;
	osg::MatrixTransform* mtRotate;
	osgEarth::Annotation::FeatureNode* featureNode;

	osgGA::NodeTrackerManipulator *mytracker;
	const osgEarth::SpatialReference* latLong;
	osg::Vec3 preVec3;

	osg::Geode* objectToFollow;

	QMutex G_mutex;

};


class UAVTrail : public TrailCallback
{
public:
	UAVTrail(osg::Geode* _geode, osgGA::NodeTrackerManipulator * _track, osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, STRPlane* _strData)
		: TrailCallback(_geode, _track, _mtObject, _mtRotate, _strData){};
	inline virtual ~UAVTrail(){};
	virtual void updateSelfData(){};

private:
	UAVTrail(){};
};

class ShipTrail1 : public TrailCallback
{
public:
	ShipTrail1(osg::Geode* _geode, osgGA::NodeTrackerManipulator * _track, osg::MatrixTransform* _mtObject, osg::MatrixTransform* _mtRotate, STRPlane* _strData)
		: TrailCallback(_geode, _track, _mtObject, _mtRotate, _strData){};
	inline virtual ~ShipTrail1(){};
	virtual void updateSelfData(){};

private:

};

#endif

