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


void flatten(const Shape_set& shapes,
	const Shape_segment_set& segments,
	Segment_set& visible_segments,
	Shape_indices& indices);
