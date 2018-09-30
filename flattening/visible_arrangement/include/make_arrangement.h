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
#include "face_index_observer.h"

void make_arrangement(Arrangement_2& arr, Face_index_observer& obs, Bezier_set& curves, Handle_set& handles);
