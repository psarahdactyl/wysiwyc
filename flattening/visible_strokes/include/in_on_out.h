//
// Sarah Kushner
// October 2018
// 
// a C++ program for reading in and parsing an SVG
// and outputting two SVGs:
//	- visible strokes
//	- visible fills
//
// NanoSVG for parsing files
// CGAL for polygon clipping


#include "helper.h"

int in_on_out(
	const Segment& seg,
	const Segment_set& intersecting_segs,
	const Point& intersection,
	const Segment_set& shape);
