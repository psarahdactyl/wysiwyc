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

void decide_to_keep(const Split_intersection_set& splits,
	const Intersection_set& intersections,
	const Segment_set& shape,
	std::vector<int>& decisions,
	front_shape_type type);