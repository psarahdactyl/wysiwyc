#include "decide_to_keep.h"
#include "in_on_out.h"

void decide_to_keep(const Split_intersection_set& splits,
	const Intersection_set& intersections,
	const Segment_set& shape,
	std::vector<int>& decisions)
{
	for (int i = 0; i < splits.size(); i++)
	{
		if (i >= intersections.size())
		{
			decisions.push_back(in_on_out(splits[i].first, splits[i].second, intersections[i - 1].first, shape));
		}
		else
		{
			decisions.push_back(in_on_out(splits[i].first, splits[i].second, intersections[i].first, shape));
		}
		std::cout << splits[i].first << std::endl;
		std::cout << "DECISION (0 means it will be removed) " << decisions[i] << std::endl;
	}
}