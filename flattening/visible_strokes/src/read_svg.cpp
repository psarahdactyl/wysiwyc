#include "read_svg.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

bool make_geometry(float* p, Segment& s, bool& closed)
{
	//std::cout << "making geometry" << std::endl;

	// Read the current segment.
	Point p0(p[0], p[1]);
	//Rat_point_2(p[2], p[3]), 
	//Rat_point_2(p[4], p[5]), 
	Point p1(p[6], p[7]);

	s = Segment(p0, p1);

	return true;
}

void make_curves(NSVGimage* image, Shape_set& shapes, Shape_segment_set& segments)
{

	Shape shape;
	NSVGpath* path;
	Segment_set shape_segments;

	int shape_number = 0;
	for (shape = image->shapes; shape != NULL; shape = shape->next)
	{
		//std::cout << "STROKE color: " << shape->stroke.color << std::endl;
		//std::cout << "FILL color: " << shape->fill.color << std::endl;
		for (NSVGpath* path = shape->paths; path != NULL; path = path->next)
		{
			bool success;
			Segment S;
			for (int i = 0; i < path->npts - 1; i += 3)
			{
				float* p = &(path->pts)[i * 2];
				bool closed = false;
				success = make_geometry(p, S, closed);
				if (success)
				{
					shape_segments.push_back(S);
					//std::cout << "pushing segment " << S << std::endl;
					//indices.push_back(shape_number);
				}
			}

			std::cout << "made path" << std::endl;
		}
		segments.push_back(shape_segments);
		shape_segments.clear();
		shapes.push_back(shape);
		shape_number++;
	}
}

//void read_svg(const char* filename, Shape_set& shapes, Segment_set& segments, Shape_indices& indices, int& w, int& h)
void read_svg(const char* filename, Shape_set& shapes, Shape_segment_set& segments, int& w, int& h)
{
	NSVGimage* g_image = NULL;

	g_image = nsvgParseFromFile(filename, "px", 96.0f);
	w = (int)g_image->width;
	h = (int)g_image->height;
	std::cout << "parsed original svg" << std::endl;
	
	make_curves(g_image, shapes, segments);
	std::cout << "made curves\n" << "-------" << std::endl << std::endl;
}
