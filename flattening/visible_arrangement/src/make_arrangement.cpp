#include "make_arrangement.h"

void make_arrangement(Arrangement_2& arr, Face_index_observer& obs, Bezier_set& curves, Handle_set& handles)
{
	
	for (int i = 0; i < curves.size(); i++)
		handles.push_back(insert(arr, curves[i]));
	
	//insert(arr, curves.begin(), curves.end());
}