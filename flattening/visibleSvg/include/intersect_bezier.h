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

void intersect_bezier(const Bezier_curve_2& c1, const Bezier_curve_2& c2, Bezier_set& subcurves);
