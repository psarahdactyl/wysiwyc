#include "flatten.h"
#include "fill_over_stroke.h"
#include "stroke_over_stroke.h"

#include "write_svg.h"

void compare_2_shapes(
	const Segment_set& background_shape, 
	const Segment_set& shape_to_add,
	const Shape_indices& background_indices,
	const Shape_set& shapes, 
	const int shape_number,
	Segment_set& visible_segments,
	Shape_indices& visible_indices)
{
	Shape shape = shapes[shape_number];
	bool is_fill = shape->fill.color != 0;

	// if new shape is a fill
	if (is_fill)
	{
		fill_over_stroke(background_shape, shape_to_add, background_indices, shapes, shape_number, visible_segments, visible_indices);
	}
	// if new shape is a stroke
	else
	{
		stroke_over_stroke(background_shape, shape_to_add, background_indices, shapes, shape_number, visible_segments, visible_indices);
	}
}

void flatten(
	const Shape_set& shapes,
	const Shape_segment_set& segments,
	Segment_set& visible_segments,
	Shape_indices& visible_indices)
{
	Segment_set build;
	Shape_indices build_indices;
	int shape_number = 0;
	for (Segment_set shape : segments)
	{
		if (shape_number == 0)
		{
			bool is_fill = shapes[shape_number]->fill.color != 0;
			if (is_fill)
			{
				Segment origin(Point(0.0, 0.0), Point(0.0, 0.0));
				build.push_back(origin);
				build_indices.push_back(0);
			}
			else
			{
				boost::range::push_back(build, shape);
				Shape_indices first_shape_indices(shape.size(), 0);
				boost::range::push_back(build_indices, first_shape_indices);
			}

		}
		else
		{
			if (shape_number < segments.size())
			{
				visible_segments.clear();
				visible_indices.clear();
			}

			compare_2_shapes(build, shape, build_indices, shapes, shape_number, visible_segments, visible_indices);
			build.clear();
			build_indices.clear();

			std::string filename = "C:/Users/sak/Documents/wysiwyc/flattening/visible_strokes/files/intermediate";
			filename += std::to_string(shape_number);
			std::string ending = ".svg";
			filename += ending;
			write_svg(filename.c_str(), 620, 620, shapes, visible_segments, visible_indices);

			boost::range::push_back(build, visible_segments);
			boost::range::push_back(build_indices, visible_indices);

			std::reverse(visible_segments.begin(), visible_segments.end());
			std::reverse(visible_indices.begin(), visible_indices.end());
		}

		shape_number++;
	}
}

/*
float max = std::numeric_limits<float>::max();
Point min_proj(max, max);
for (int i = 0; i < intersecting_segs.size(); i++)
{
Segment i_seg = intersecting_segs[i];
K::Construct_projected_point_2 project;
Point proj = project(i_seg.supporting_line(), centroid);

if (!seg.has_on(proj))
{
proj = CGAL::compare_distance_to_point(proj, i_seg.source(), i_seg.target()) ==
CGAL::SMALLER ? i_seg.source() : i_seg.target();
}

// the closest to the centroid will be the min projection
min_proj = CGAL::compare_distance_to_point(centroid, proj, min_proj) ==
CGAL::SMALLER ? proj : min_proj;
}
*/