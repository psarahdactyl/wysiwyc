#include "split_segments.h"

void split_segments(const Segment& old_seg, Intersection_set& intersections, Split_intersection_set& splits)
{
	std::sort(intersections.begin(),
		intersections.end(),
		[](auto &a, auto &b) { return a.first < b.first; });

	std::cout << "SPLIT SEGMENTS" << std::endl;
	for (int i = 0; i <= intersections.size(); i++)
	{
		Segment_set segs;
		if (i == 0)
		{
			std::cout << intersections[i].first << std::endl;
			segs.push_back(intersections[i].second);
			Segment s = Segment(old_seg.source(), intersections[i].first);
			std::cout << s << std::endl;
			//if (!(s.is_degenerate()) && !(old_seg == s))
			//if (!s.is_degenerate())
			splits.push_back(Split_intersection(s, segs));
		}
		else if (i == intersections.size())
		{
			segs.push_back(intersections[i - 1].second);
			Segment s = Segment(intersections[i - 1].first, old_seg.target());
			std::cout << s << std::endl;
			//if (!s.is_degenerate())
			splits.push_back(Split_intersection(s, segs));
		}
		else
		{
			std::cout << intersections[i].first << std::endl;
			// TODO: check this. which seg to push back?
			segs.push_back(intersections[i].second);
			segs.push_back(intersections[i - 1].second);
			Segment s = Segment(intersections[i - 1].first, intersections[i].first);
			std::cout << s << std::endl;
			//if (!s.is_degenerate())
			splits.push_back(Split_intersection(s, segs));
		}
		segs.clear();
	}
}