#pragma  once






//1.转obj，导到3dsmax，找到风扇，给个名字，再导ive，然后你懂得
//osgDB::writeNodeFile(xx.obj);
//2.或者写代码，遍历节点，一个个导出子节点ive，看哪个索引是螺旋桨

//class lineStyle : osgEarth::Symbology::Style
//{
//
//public:
//
//	inline lineStyle()
//	{
//		getOrCreate<LineSymbol>()->stroke()->color() = Color::White;
//		getOrCreate<LineSymbol>()->stroke()->width() = 1.0f;
//		getOrCreate<LineSymbol>()->tessellationSize() = 75000;
//		getOrCreate<PointSymbol>()->size() = 5;
//		getOrCreate<PointSymbol>()->fill()->color() = Color::Red;
//		getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
//		getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;
//	};
//	inline ~lineStyle();
//
//private:
//
//};
//
//class Common
//{
//public:
//	
//	static Common* instance()
//	{
//		if (common == NULL)
//		{
//			common = new Common;
//		}
//		return common;
//	}
//
//	inline ~Common(){};
//
//	lineStyle linestyle;
//
//private:
//	inline Common(){};
//
//	static Common* common;
//
//	
//};




