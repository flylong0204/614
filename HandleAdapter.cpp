#include "HandleAdapter.h"

HandleAdapter::HandleAdapter(osgViewer::Viewer* view, osg::Group* group, osgEarth::SpatialReference* src, QObject* parent/* = NULL*/)
	: m_pViewer(view), m_pGroup(group), m_pSrc(src), QObject(parent)
{
	m_pViewer = view;
	m_pGroup = group;
	//m_pLayerGroup = new osg::Group;
	group->addChild(m_pLayerGroup);

	m_pEventHandle = new PickHandle(m_pSrc);
	benginHandle();
}

HandleAdapter::~HandleAdapter()
{

}

void HandleAdapter::benginHandle()
{
	m_pViewer->addEventHandler(m_pEventHandle);
	connect(m_pEventHandle, SIGNAL(signalPicked(osg::Vec3d)), this, SLOT(slotPicked(osg::Vec3d)));
	connect(m_pEventHandle, SIGNAL(signalMoving(osg::Vec3d)), this, SLOT(slotMoveing(osg::Vec3d)));
	connect(m_pEventHandle, SIGNAL(signalRightPicked()), this, SLOT(slotRightHandle()));
	connect(m_pEventHandle, SIGNAL(signalPickedXYZ(osg::Vec3d)), this, SLOT(slotPickedXYZ(osg::Vec3d)));
	connect(m_pEventHandle, SIGNAL(signalMovingXYZ(osg::Vec3d)), this, SLOT(slotMoveingXYZ(osg::Vec3d)));
}

void HandleAdapter::endHandle()
{
	disconnect(m_pEventHandle, SIGNAL(signalPicked(osg::Vec3d)), this, SLOT(slotPicked(osg::Vec3d)));
	disconnect(m_pEventHandle, SIGNAL(signalMoving(osg::Vec3d)), this, SLOT(slotMoveing(osg::Vec3d)));
	disconnect(m_pEventHandle, SIGNAL(signalRightPicked()), this, SLOT(slotRightHandle()));
	disconnect(m_pEventHandle, SIGNAL(signalPickedXYZ(osg::Vec3d)), this, SLOT(slotPickedXYZ(osg::Vec3d)));
	disconnect(m_pEventHandle, SIGNAL(signalMovingXYZ(osg::Vec3d)), this, SLOT(slotMoveingXYZ(osg::Vec3d)));
	m_pViewer->removeEventHandler(m_pEventHandle);
}



void mouseEvent::slotPicked(osg::Vec3d pos)
{
	emit signalmove(pos);
}

void mouseEvent::slotMoveing(osg::Vec3d pos)
{
	emit signalmove(pos);
}

void mouseEvent::slotPickedXYZ(osg::Vec3d pos)
{
	emit signalmove(pos);
}

void mouseEvent::slotMoveingXYZ(osg::Vec3d pos)
{
	emit signalmove(pos);
}
