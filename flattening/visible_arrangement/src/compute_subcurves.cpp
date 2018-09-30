#include "compute_subcurves.h"
#include "intersect_bezier.h"


void compute_subcurves(Bezier_set& curves, Monotone_set& subcurves)
{
	Traits_2 tr;

	std::vector<Traits_2::Point_2> inters;
	CGAL::compute_intersection_points(curves.begin(), curves.end(), std::back_inserter(inters), false, tr);
	std::cout << "correct intersections:    " << inters.size() << std::endl;

	/*
	Monotone_set m_subcurves;
	CGAL::compute_subcurves(curves.begin(), curves.end(), std::back_inserter(m_subcurves), false, tr);
	std::cout << "correct subcurves:    " << m_subcurves.size() << std::endl;

	std::vector<Bezier_set> sets;
	for (Bezier_curve_2 c1 : curves)
	{
		for (Bezier_curve_2 c2 : curves)
		{
			Bezier_set set;
			intersect_bezier(c1, c2, set);
			sets.push_back(set);
		}
	}
	
	subcurves = flatten(sets);

	std::cout << "# of subcurves: " << subcurves.size() << std::endl;
	*/
	
}
