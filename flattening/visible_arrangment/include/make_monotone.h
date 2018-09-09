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

void make_monotone(const Bezier_set& curves, Monotone_set& monotone_curves, const Shape_indices& indices, Shape_indices& monotone_indices);
