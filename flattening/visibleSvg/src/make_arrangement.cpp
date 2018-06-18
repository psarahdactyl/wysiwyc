#include "make_arrangement.h"

void make_arrangement(Arrangement_2& arr, Bezier_set& curves, Handle_set& handles)
{
	for (int i = 0; i < curves.size(); i++)
		handles.push_back(insert(arr, curves[i]));
}