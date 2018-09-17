#include "intersect_bezier.h"


void intersect_bezier(const Bezier_curve_2& c1, const Bezier_curve_2& c2, Bezier_set& subcurves)
{

	// make curves monotone
	Traits_2 tr;
	auto make_monotone = tr.make_x_monotone_2_object();

	Bezier_set curves = { c1, c2 };
	Monotone_set m_curves;
	Shape_indices m_inds;

	X_monotone_curve_2 m_curve;
	std::list<CGAL::Object> m;
	std::list<CGAL::Object>::const_iterator it;

	for (int i = 0; i < curves.size(); i++)
	{
		Bezier_curve_2 curve = curves[i];
		make_monotone(curve, std::back_inserter(m));

		for (it = m.begin(); it != m.end(); ++it)
		{
			if (CGAL::assign(m_curve, *it))
			{
				m_inds.push_back(i);
				m_curves.push_back(m_curve);
			}
			
		}
	}

	// functors
	auto intersect_curves = tr.intersect_2_object();
	auto split_curve = tr.split_2_object();
	auto merge_curves = tr.merge_2_object();

	std::vector<Traits_2::Point_2> intersections;
	Monotone_set m_subcurves;
	Shape_indices m_sub_inds;

	// now we can intersect them
	for (int i = 0; i < m_curves.size(); i++)
	{
		for (int j = i+1; j < m_curves.size(); j++)
		{
			X_monotone_curve_2 mc1 = m_curves[i];
			X_monotone_curve_2 mc2 = m_curves[j];

			std::list<CGAL::Object> ints;
			std::pair<Traits_2::Point_2, unsigned int> i_c;
			std::list<CGAL::Object>::const_iterator it;

			intersect_curves(mc1, mc2, std::back_inserter(ints));

			for (it = ints.begin(); it != ints.end(); ++it)
			{
				if (CGAL::assign(i_c, *it))
				{	
					Traits_2::Point_2 p = i_c.first;
					int multiplicity = i_c.second;
					if (multiplicity == 1)
					{
						intersections.push_back(p);
						X_monotone_curve_2 n1, n2;
						
						split_curve(mc1, p, n1, n2);
						m_subcurves.push_back(n1);
						m_sub_inds.push_back(i);
						m_subcurves.push_back(n2);
						m_sub_inds.push_back(i);

						split_curve(mc2, p, n1, n2);
						m_subcurves.push_back(n1);
						m_sub_inds.push_back(j);
						m_subcurves.push_back(n2);
						m_sub_inds.push_back(j);

						std::list<Algebraic> t_x;
						//Bezier_curve_2 b = mc1.supporting_curve();
						//const Rat_point_2& rat_p = (Rat_point_2)p;
						//b.get_t_at_x(rat_p.x(), std::back_inserter(t_x));
						
					}
				}
				// can we merge x-monotone curves back together?

			}
		}
	}

	for (int i = 0; i < m_subcurves.size(); i++)
	{
		m_subcurves[i].xid();
	}
	std::cout << "# of subcurves: " << m_subcurves.size() << std::endl;

	//std::cout << "# of intersections: " << intersections.size() << std::endl;
	//for (int i = 0; i < intersections.size(); i++)
	//	std::cout << intersections[i] << std::endl;
}