#pragma once

#include <osg/Vec3d>
#include <osgEarthSymbology/Style>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgGA/GUIEventAdapter>


class DrawLine : public osgGA::GUIEventAdapter
{
public:
	DrawLine(osgEarth::MapNode* mapNode = NULL);
	~DrawLine();

protected:
	//virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	// ֻ��Ҫ��γ��
	virtual void slotPicked(osg::Vec3d pos);
	virtual void slotMoveing(osg::Vec3d pos);
	virtual void slotRightHandle();

private:
	// ʵ�ߵ�style��Node
	osgEarth::Symbology::Style m_lineStyle;
	osgEarth::Features::Feature* m_pFeature;
	osgEarth::Annotation::FeatureNode* m_pFeatureNode;

	// ���ߵ�style��Node
	osgEarth::Symbology::Style m_stippleLineStyle;
	osgEarth::Features::Feature* m_pStippleFeature;
	osgEarth::Annotation::FeatureNode* m_pStippleFeatureNode;

	std::vector<osg::Vec3d> m_vecPoint;
	osgEarth::MapNode* _pMapNode;
};
