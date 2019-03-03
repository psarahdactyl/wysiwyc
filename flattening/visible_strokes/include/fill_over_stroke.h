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

void fill_over_stroke(
	const Segment_set& back,
	const Segment_set& front,
	const Shape_indices& background_indices,
	const Shape_set& shapes,
	const int shape_number,
	Segment_set& visible_segments,
	Shape_indices& visible_indices);