#include "NodeMatrix.h"


planeMatrix::planeMatrix(void)
{
	pat = new osg::MatrixTransform;
	addChild(pat.get());
	level = 1.0;
	position = osg::Vec3d(0, 0, 0);
}

planeMatrix::~planeMatrix(void)
{
}

/**设置当前模型转动方式*/
void planeMatrix::rotating(const osg::Vec3d &pivot, const osg::Vec3d &axis, float angularVelocity)
{
	setUpdateCallback(new osg::AnimationPathCallback(pivot, axis, angularVelocity));
}

/**旋转模型*/
void planeMatrix::toRotate(const osg::Matrix &mat)
{
	pat->setMatrix(mat);
}

/**旋转模型*/
void planeMatrix::toRotate(float angle, const osg::Vec3f &axis)
{
	pat->setMatrix(osg::Matrix::rotate(angle, axis));
}

/**缩放模型*/
void planeMatrix::toScale(const osg::Matrix &mat)
{
	pat->setMatrix(mat);
}

/**缩放模型*/
void planeMatrix::toScale(double &lel)
{
	pat->setMatrix(osg::Matrix::scale(lel, lel, lel));
}

/**addsChild方法*/
void planeMatrix::addsChild(osg::Node *node)
{
	pat->addChild(node);
	pnode = node;
	ps = node->getBound();
	osg::notify(osg::NOTICE) << ps.center().x() << "  " << ps.center().y() << "  " << ps.center().z() << std::endl;
}

/**将模型移到目的点*/
void planeMatrix::toPosition(osg::Vec3d &pos)
{
	osg::Vec3d cps;
	cps.set(-ps.center().x()*level, -ps.center().y()*level, -ps.center().z()*level);
	pat->setMatrix(osg::Matrix::translate(cps)*osg::Matrix::translate(pos));
	position = pos;
}

/**限制模型大小*/
void planeMatrix::adapt(osg::BoundingSphere &sps)
{
	float level = sps.radius() / ps.radius();
	pat->setMatrix(osg::Matrix::scale(level, level, level));
}

/**限制模型大小*/
void planeMatrix::adapt(osg::Node * node)
{
	osg::BoundingSphere sps = node->getBound();
	level = sps.radius() / ps.radius();
	pat->setMatrix(osg::Matrix::scale(level, level, level));
}

/**得到当前的位置*/
osg::Vec3d planeMatrix::getPosition()
{
	return position;
}