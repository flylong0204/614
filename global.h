#pragma  once

//extern QMutex G_mutex;

#define DEFUAVOFFSET		420
#define DEFSHIPOFFSET		500
#define DEFVEHICLEOFFSET	360

#define DEFUAVSCALE		    100
#define DEFSHIPSCALE		20
#define DEFVEHICLESCALE		200



/*λ����Ϣ���ݽṹ*/
typedef struct STRPOINT
{
	double longitude; //���� 0~180
	double latitude;  //γ�� -90~90
	double altitude;  //�߶�
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