#pragma once
#ifndef MYSVG_H
#define MYSVG_H

#include "../polygons/myPolygon.h"

class mySVG
{
public:
	mySVG();
	std::vector<myPolygon> polygons;
	Polygon_set fills;
	Polygon_set getCgalPolygons();
	Polygon_set getCgalPolygons(std::vector<myPolygon> myPolygons);
	~mySVG();
};


#endif
