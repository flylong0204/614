#ifndef HANDLEADAPTER_H
#define HANDLEADAPTER_H

#include <QObject>
#include <osgViewer/Viewer>
#include <osgEarth/SpatialReference>

#include "PickHandle.h"

class HandleAdapter : public QObject
{
	Q_OBJECT

public:
	HandleAdapter(osgViewer::Viewer* view, osg::Group* group, osgEarth::SpatialReference* src, QObject* parent = NULL);
	~HandleAdapter();

	void benginHandle();
	void endHandle();

	private slots:
	// 虚函数的实现，有其子类来
	virtual void slotPicked(osg::Vec3d pos){}
	virtual void slotMoveing(osg::Vec3d pos){}

	virtual void slotPickedXYZ(osg::Vec3d pos){}
	virtual void slotMoveingXYZ(osg::Vec3d pos){}

	virtual void slotRightHandle(){}

public:
	osgEarth::SpatialReference* m_pSrc;
	osgViewer::Viewer* m_pViewer;
	osg::Group* m_pLayerGroup;
	PickHandle* m_pEventHandle;
	osg::Group* m_pGroup;
};


class mouseEvent : public HandleAdapter
{
	Q_OBJECT
public:
	inline mouseEvent();
	inline ~mouseEvent();

public slots:

	// 虚函数的实现，有其子类来
	virtual void slotPicked(osg::Vec3d pos);
	virtual void slotMoveing(osg::Vec3d pos);

	virtual void slotPickedXYZ(osg::Vec3d pos);
	virtual void slotMoveingXYZ(osg::Vec3d pos);
signals:
	void signalmove(osg::Vec3d pos);

private:

};

#endif // HANDLEADAPTER_H
