//
// Sarah Kushner
// August/September 2018
// 
// a C++ program for reading in and parsing an SVG
// and outputting a flat SVG:
//	- visible strokes
//
// NanoSVG for parsing files
// CGAL for polygon clipping
// 

#include "read_svg.h"
#include "flatten.h"
#include "write_svg.h"

int main(int argc, char *argv[])
{

	const char* filename = (argc > 1) ? argv[1] : "C:/Users/sak/source/repos/wysiwyc/flattening/visible_strokes/files/from-the-wild/wedding-invitation-front.svg";
	//svg-polyline.svg";
	//svg-TEST1.svg";
	//svg-overlap-closed-shape-and-line-3.svg";
	//svg-shapes-two-rectangles.svg"; 
	//svg-overlap-edges-exact.svg";

	// Each NSVGshape in the Shape_set has a corresponding (index-wise) 
	Shape_set shapes;
	Shape_indices indices;
	Point_set points;
	Shape_segment_set segments;
	int width, height;

	// Reads in an svg file and fills the shape and Bezier sets
	read_svg(filename, shapes, segments, width, height);

	std::cout << "number of shapes total: " << shapes.size() << std::endl;
	std::cout << "inside" << "\t" << "0" << "\n"
		<< "on" << "\t" << "1" << "\n"
		<< "outside" << "\t" << "2" << "\n";

	Segment_set visible_segments;
	flatten(shapes, segments, visible_segments, indices);

	std::string new_filename = filename;
	std::string ending = "_flattened.svg";
	new_filename += ending;
	std::cout << new_filename << std::endl;

	//write_svg(new_filename.c_str(), width, height, shapes, segments);
	write_svg(new_filename.c_str(), width, height, shapes, visible_segments, indices);

	return 0;
}
