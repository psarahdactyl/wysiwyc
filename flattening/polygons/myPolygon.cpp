#include "myPolygon.h"



myPolygon::myPolygon()
{
	closed = false;
	fill = 0;
	stroke = 0;
}

myPolygon::myPolygon(bool closed, int fill, int stroke, Polygon_with_holes_2 cgalPoly)
{
	this->closed = closed;
	this->fill = fill;
	this->stroke = stroke;
	this->cgalPolygon = cgalPoly;
}

myPolygon::~myPolygon()
{
}
