#include "make_monotone.h"


void make_monotone(const Bezier_set& curves, Monotone_set& monotone_curves, const Shape_indices& indices, Shape_indices& monotone_indices)
{

	Traits_2 tr;
	auto make_monotone = tr.make_x_monotone_2_object();

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
				monotone_indices.push_back(indices[i]);
				monotone_curves.push_back(m_curve);
			}			
		}
	}

	// --------------- PRINTING ---------------
	std::cout << "# of bezier curves: " << curves.size() << std::endl;
	std::cout << "# of monotone curves: " << monotone_curves.size() << std::endl;
}
