#include "face_index_observer.h"

void Face_index_observer::after_split_face(Face_handle /* old_face */,
	Face_handle new_face, bool)
{
	// Assign index to the new face.
	new_face->set_data(n_faces);
	n_faces++;
}