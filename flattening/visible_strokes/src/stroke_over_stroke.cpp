#include "stroke_over_stroke.h"
#include "split_segments.h"
#include "decide_to_keep.h"

void stroke_over_stroke(
	const Segment_set& back,
	const Segment_set& front,
	const Shape_indices& background_indices,
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
		Segment seg_behind = back[i];
		Intersection_set intersections;
		intersected = false;
		// go through each segment in the new shape
		for (int j = 0; j < front.size(); j++)
		{
			Segment seg_to_add = front[j];
			//std::cout << "NEW SEG " << seg_to_add << std::endl;
			if (CGAL::do_intersect(seg_behind, seg_to_add))
			{
				intersected = true;
				auto result = intersection(seg_behind, seg_to_add);
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
					intersections.push_back(Intersection(*p, seg_to_add));
				}
			}
			// add NEW segments 
			if (!already_in_set(visible_segments, seg_to_add))
			{
				std::cout << "ADDING SEGMENT " << seg_to_add << std::endl;
				visible_segments.push_back(seg_to_add);
				visible_indices.push_back(shape_number);
			}
			

		}
		if (intersected)
		{
			//std::cout << "INTERSECTED " << intersections.size() << std::endl;
			Split_intersection_set splits;
			split_segments(seg_behind, intersections, splits);

			std::vector<int> decisions;
			decide_to_keep(splits, intersections, front, decisions, STROKE);

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
						visible_indices.push_back(background_indices[i]);
					}
				}
			}
		}
		else
		{
			if (!already_in_set(visible_segments, seg_behind))
			{
				visible_segments.push_back(seg_behind);
				visible_indices.push_back(background_indices[i]);
			}
		}
	}
}