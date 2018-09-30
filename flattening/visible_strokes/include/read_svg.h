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

//void read_svg(const char* filename, Shape_set& shapes, Segment_set& segments, Shape_indices& indices, int& w, int& h);
void read_svg(const char* filename, Shape_set& shapes, Shape_segment_set& segments, int& w, int& h);