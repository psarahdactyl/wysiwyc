#include "helper.h"

// An arrangement observer, used to receive notifications of face splits and
// to update the indices of the newly created faces.
class Face_index_observer : public CGAL::Arr_observer<Arrangement_2>
{
private:
	int n_faces;          // The current number of faces.
public:
	Face_index_observer(Arrangement_2& arr) :
		CGAL::Arr_observer<Arrangement_2>(arr),
		n_faces(0)
	{
		CGAL_precondition(arr.is_empty());
		arr.unbounded_face()->set_data(0);
		n_faces++;
	}
	virtual void after_split_face(Face_handle /* old_face */,
		Face_handle new_face, bool);
};
