#include "mySVG.h"


mySVG::mySVG()
{
}

Polygon_set mySVG::getCgalPolygons()
{
	Polygon_set cgalPolygons;
	for (int i = 0; i < polygons.size(); i++)
	{
		cgalPolygons.push_back(polygons[i].cgalPolygon);
	}

	return cgalPolygons;
}

Polygon_set mySVG::getCgalPolygons(std::vector<myPolygon> myPolygons)
{
	Polygon_set cgalPolygons;
	for (int i = 0; i < myPolygons.size(); i++)
	{
		cgalPolygons.push_back(myPolygons[i].cgalPolygon);
	}

	return cgalPolygons;
}

mySVG::~mySVG()
{
}
