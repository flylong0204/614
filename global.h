#pragma  once

//extern QMutex G_mutex;

#define DEFUAVOFFSET		420
#define DEFSHIPOFFSET		500
#define DEFVEHICLEOFFSET	360

#define DEFUAVSCALE		    100
#define DEFSHIPSCALE		20
#define DEFVEHICLESCALE		200



/*位置信息数据结构*/
typedef struct STRPOINT
{
	double longitude; //经度 0~180
	double latitude;  //纬度 -90~90
	double altitude;  //高度
	STRPOINT(double _long, double _lat, double _alt)
	{
		longitude = _long;
		latitude = _lat;
		altitude = _alt;
	}
	STRPOINT(){}
}STRPoint;

//extern STRPlane G_STRPlane;
//
//extern STRShip  G_STRShip;
//
//extern STRVehicle G_STRVehicle;

//osg::ref_ptr<osg::MatrixTransform>  g_mtUAVRotate;
//
//osg::ref_ptr<osg::MatrixTransform>  g_mtShipRotate;
//
//osg::ref_ptr<osg::MatrixTransform>  g_mtVehicleRotate;