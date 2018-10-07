#include "fill_over_stroke.h"
#include "split_segments.h"
#include "decide_to_keep.h"

void fill_over_stroke(
	const Segment_set& back,
	const Segment_set& front,
	const Shape_set& shapes,
	const int shape_number,
	Segment_set& visible_segments,
	Shape_indices& visible_indices)
{
	std::cout << "FILL OVER STROKE" << std::endl << "----" << std::endl;

	Shape shape = shapes[shape_number];
	bool intersected = false;

	// go through each segment of background
	for (int i = 0; i < back.size(); i++)
	{
		Segment seg_behind = back[i];
		Intersection_set intersections;
		Segment new_seg;
		intersected = false;
		// go through each segment of shape to add
		for (int j = 0; j < front.size(); j++)
		{
			Segment seg_to_add = front[j];

			if (CGAL::do_intersect(seg_to_add, seg_behind))
			{
				intersected = true;
				auto result = intersection(seg_to_add, seg_behind);
				if (const Segment* s = boost::get<Segment>(&*result))
				{
					//std::cout << "segment " << *s << std::endl;
					intersections.push_back(Intersection(s->source(), seg_to_add));
					intersections.push_back(Intersection(s->target(), seg_to_add));
				}
				else
				{
					const Point* p = boost::get<Point>(&*result);
					//std::cout << "point " << *p << std::endl;
					if (!already_in_set(intersections, Intersection(*p, seg_to_add)))
					{
						intersections.push_back(Intersection(*p, seg_to_add));
					}
				}
			}
		}
		if (intersected)
		{
			//std::cout << "INTERSECTED " << intersections.size() << std::endl;
			Split_intersection_set splits;
			split_segments(seg_behind, intersections, splits);

			std::vector<int> decisions;
			decide_to_keep(splits, intersections, front, decisions, FILL);

			intersections.clear();

			for (int k = 0; k < decisions.size(); k++)
			{
				int d = decisions[k];
				//std::cout << d << std::endl;
				if (d != 0)
				{
					if (!already_in_set(visible_segments, splits[k].first))
					{
						visible_segments.push_back(splits[k].first);
						visible_indices.push_back(shape_number - 1);
					}
				}
			}
		}
		else
		{
			if (!already_in_set(visible_segments, seg_behind))
			{
				visible_segments.push_back(seg_behind);
				visible_indices.push_back(shape_number - 1);
			}
		}
	}
}