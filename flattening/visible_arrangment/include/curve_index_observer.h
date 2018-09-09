#include "helper.h"

// An arrangement observer, used to receive notifications of curve insertions and
// to update the indices of the newly added curves.
class Curve_index_observer : public CGAL::Arr_observer<Arrangement_2>
{
private:
	int n_curves;          // The current number of curves.
public:
	Curve_index_observer(Arrangement_2& arr) :
		CGAL::Arr_observer<Arrangement_2>(arr),
		n_curves(0)
	{
		CGAL_precondition(arr.is_empty());
		n_curves++;
	}
	
	virtual void after_global_change();

};
