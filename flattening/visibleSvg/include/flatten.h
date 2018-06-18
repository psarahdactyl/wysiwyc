//
// Sarah Kushner
// May 2018
// 
// a C++ program for reading in and parsing an SVG
// and outputting two SVGs:
//	- visible strokes
//	- visible fills
//
// NanoSVG for parsing files
// CGAL for polygon clipping


#include "helper.h"


void flatten(Arrangement_2& arr, Shape_set& shapes, Handle_set& handles, Shape_indices& indices);
