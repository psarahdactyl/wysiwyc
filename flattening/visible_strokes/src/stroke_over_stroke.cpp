#include "stroke_over_stroke.h"
#include "split_segments.h"
#include "decide_to_keep.h"

void stroke_over_stroke(
	const Segment_set& back,
	const Segment_set& front,
	const Shape_set& shapes,
	const int shape_number,
	Segment_set& visible_segments,
	Shape_indices& visible_indices)
{
	std::cout << "STROKE OVER STROKE" << std::endl;

	Shape shape = shapes[shape_number];
	bool intersected = false;

	// go through each segment of old shape
	for (int i = 0; i < back.size(); i++)
	{
		Segment old_seg = back[i];
		Intersection_set intersections;
		Segment new_seg;
		intersected = false;
		// go through each segment in the new shape
		for (int j = 0; j < front.size(); j++)
		{
			new_seg = front[j];
			std::cout << "NEW SEG " << new_seg << std::endl;
			if (CGAL::do_intersect(old_seg, new_seg))
			{
				intersected = true;
				auto result = intersection(old_seg, new_seg);
				if (const Segment* s = boost::get<Segment>(&*result))
				{
					//std::cout << "segment " << *s << std::endl;
					intersections.push_back(Intersection(s->source(), new_seg));
					intersections.push_back(Intersection(s->target(), new_seg));
				}
				else
				{
					const Point* p = boost::get<Point>(&*result);
					//std::cout << "point " << *p << std::endl;
					intersections.push_back(Intersection(*p, new_seg));
				}
			}
			// add NEW segments 
			if (!already_in_set(visible_segments, new_seg))
			{
				std::cout << "ADDING SEGMENT " << new_seg << std::endl;
				visible_segments.push_back(new_seg);
				visible_indices.push_back(shape_number);
			}
			

		}
		if (intersected)
		{
			//std::cout << "INTERSECTED " << intersections.size() << std::endl;
			Split_intersection_set splits;
			split_segments(old_seg, intersections, splits);

			std::vector<int> decisions;
			decide_to_keep(splits, intersections, front, decisions);

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
			if (!already_in_set(visible_segments, old_seg))
			{
				visible_segments.push_back(old_seg);
				visible_indices.push_back(shape_number - 1);
			}
		}
	}
}