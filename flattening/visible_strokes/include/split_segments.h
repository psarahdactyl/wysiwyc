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

void split_segments(const Segment& old_seg, Intersection_set& intersections, Split_intersection_set& splits);