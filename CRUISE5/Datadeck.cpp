#include "Datadeck.hpp"
#include "global_header.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//Single independent variable look-up
//Constant extrapolation at the upper end, slope extrapolation at the lower end
//
//030717 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Datadeck::look_up(string name, double value1)
{
	//finding slot of table in table pointer array (Table **table_ptr) 
	int slot(-1);
	string tbl_name;
	do {
		slot++;
		tbl_name = get_tbl(slot)->get_name();
	} while (name != tbl_name);

	//getting table index locater of discrete value just below of variable value
	int var1_dim = get_tbl(slot)->get_var1_dim();
	int loc1 = find_index(var1_dim - 1, value1, get_tbl(slot)->var1_values);

	//using max discrete value if value is outside table
	if (loc1 == (var1_dim - 1)) return get_tbl(slot)->data[loc1];

	return interpolate(loc1, loc1 + 1, slot, value1);
}

///////////////////////////////////////////////////////////////////////////////
//Two independent variables look-up
//constant extrapolation at the upper end, slope extrapolation at the lower end
//
//030717 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
double Datadeck::look_up(string name, double value1, double value2)
{
	//finding slot of table in table pointer array (Table **table_ptr) 
	int slot(-1);
	string tbl_name;
	do {
		slot++;
		tbl_name = get_tbl(slot)->get_name();
	} while (name != tbl_name);


	//getting table index (off-set) locater of discrete value just below or equal of the variable value
	int var1_dim = get_tbl(slot)->get_var1_dim();
	int loc1 = find_index(var1_dim - 1, value1, get_tbl(slot)->var1_values);

	int var2_dim = get_tbl(slot)->get_var2_dim();
	int loc2 = find_index(var2_dim - 1, value2, get_tbl(slot)->var2_values);

	return interpolate(loc1, loc1 + 1, loc2, loc2 + 1, slot, value1, value2);
}

///////////////////////////////////////////////////////////////////////////////
//Three independent variables look-up
//constant extrapolation at the upper end, slope extrapolation at the lower end
//
//030723 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
double Datadeck::look_up(string name, double value1, double value2, double value3)
{
	//finding slot of table in table pointer array (Table **table_ptr) 
	int slot(-1);
	string tbl_name;
	do {
		slot++;
		tbl_name = get_tbl(slot)->get_name();
	} while (name != tbl_name);


	//getting table index locater of discrete value just below of variable value
	int var1_dim = get_tbl(slot)->get_var1_dim();
	int loc1 = find_index(var1_dim - 1, value1, get_tbl(slot)->var1_values);

	int var2_dim = get_tbl(slot)->get_var2_dim();
	int loc2 = find_index(var2_dim - 1, value2, get_tbl(slot)->var2_values);

	int var3_dim = get_tbl(slot)->get_var3_dim();
	int loc3 = find_index(var3_dim - 1, value3, get_tbl(slot)->var3_values);

	return interpolate(loc1, loc1 + 1, loc2, loc2 + 1, loc3, loc3 + 1, slot, value1, value2, value3);
}
///////////////////////////////////////////////////////////////////////////////
//Table index finder
//This is a binary search method it is O(lgN)
// Returns array locater (offset index) of the discrete_variable just below variable
// Keeps max or min array locater if variable is outside those max or min  
//
//030717 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

int Datadeck::find_index(int max, double value, double* list)
{
	if (value >= list[max])
		return max;
	else if (value <= list[0]) {
		return 0;
	}
	else {
		int index = 0;
		int mid;
		while (index <= max) {
			mid = (index + max) / 2;		//integer division
			if (value < list[mid])
				max = mid - 1;
			else if (value > list[mid])
				index = mid + 1;
			else
				return mid;
		}
		return max;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Linear one-dimensional interpolation
//Data deck must contain table in the following format:
//
// X1       Table Values(X1)
//
// X11		Y11
// X12		Y12
// X13		Y13
//           
// Constant extrapolation beyond max values of X1
// Slope extrapolation beyond min values of X1
//
//030717 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Datadeck::interpolate(int ind1, int ind2, int slot, double val)
{
	double dx(0), dy(0);
	double dumx(0);

	double diff = val - get_tbl(slot)->var1_values[ind1];
	dx = get_tbl(slot)->var1_values[ind2] - get_tbl(slot)->var1_values[ind1];
	dy = get_tbl(slot)->data[ind2] - get_tbl(slot)->data[ind1];

	if (dx > EPS) dumx = diff / dx;
	dy = dumx * dy;

	return get_tbl(slot)->data[ind1] + dy;
}
///////////////////////////////////////////////////////////////////////////////
//Linear, two-dimensional interpolation
//File must contain table in the following form:
//
//  X1  X2  //Table Values(X1-row, X2-column)
//            ---------------
//  X11 X21   |Y11  Y12  Y13| 
//  X12 X22   |Y21  Y22  Y23|    <- data
//  X13 X23   |Y31  Y32  Y33| 
//  X14       |Y41  Y42  Y43| 
//            ---------------
//Constant extrapolation beyond max values of X1 and X2
//Slope extrapolation beyond min values of X1 and X2
//
//030718 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Datadeck::interpolate(int ind10, int ind11, int ind20, int ind21, int slot, double value1,
	double value2)
{
	double dx1(0), dx2(0);
	double dumx1(0), dumx2(0);

	int var1_dim = get_tbl(slot)->get_var1_dim();
	int var2_dim = get_tbl(slot)->get_var2_dim();

	double diff1 = value1 - get_tbl(slot)->var1_values[ind10];;
	double diff2 = value2 - get_tbl(slot)->var2_values[ind20];

	if (ind10 == (var1_dim - 1)) //Assures constant upper extrapolation of first variable
		ind11 = ind10;
	else
		dx1 = get_tbl(slot)->var1_values[ind11] - get_tbl(slot)->var1_values[ind10];

	if (ind20 == (var2_dim - 1)) //Assures constant upper extrapolation of second variable
		ind21 = ind20;
	else
		dx2 = get_tbl(slot)->var2_values[ind21] - get_tbl(slot)->var2_values[ind20];

	if (dx1 > EPS) dumx1 = diff1 / dx1;
	if (dx2 > EPS) dumx2 = diff2 / dx2;

	double y11 = get_tbl(slot)->data[ind10 * var2_dim + ind20];
	double y12 = get_tbl(slot)->data[ind10 * var2_dim + ind21];
	double y21 = get_tbl(slot)->data[ind11 * var2_dim + ind20];
	double y22 = get_tbl(slot)->data[ind11 * var2_dim + ind21];
	double y1 = dumx1 * (y21 - y11) + y11;
	double y2 = dumx1 * (y22 - y12) + y12;

	return dumx2 * (y2 - y1) + y1;
}
///////////////////////////////////////////////////////////////////////////////
//Linear, three-dimensional interpolation
//File must contain table in the following form:
//
//  X1  X2  X3    Table Values(X1-row, X2-block, X3-column) <- don't type (illustration only)
//
//                (X1 x X3) (X1 x X3) (X1 x X3) (X1 x X3)	<- don't type 
//				   for X21   for X22   for X23   for X24	<- don't type 
//               -----------------------------------------
//  X11 X21 X31  |Y111 Y112|Y121 Y122|Y131 Y132|Y141 Y142|  
//  X12 X22 X32  |Y211 Y212|Y221 Y222|Y231 Y232|Y241 Y242|  <- data; don't type: '|'
//  X13 X23      |Y311 Y312|Y321 Y322|Y331 Y332|Y341 Y342| 
//      X24      ----------------------------------------- 
//               
//Constant extrapolation beyond max values of X1, X2 and X3
//Slope extrapolation beyond min values of X1, X2 and X3
//
//030723 Created and corrected by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
double Datadeck::interpolate(int ind10, int ind11, int ind20, int ind21, int ind30, int ind31,
	int slot, double value1, double value2, double value3)
{
	double dx1(0), dx2(0), dx3(0);
	double dumx1(0), dumx2(0), dumx3(0);

	int var1_dim = get_tbl(slot)->get_var1_dim();
	int var2_dim = get_tbl(slot)->get_var2_dim();
	int var3_dim = get_tbl(slot)->get_var3_dim();

	double diff1 = value1 - get_tbl(slot)->var1_values[ind10];;
	double diff2 = value2 - get_tbl(slot)->var2_values[ind20];
	double diff3 = value3 - get_tbl(slot)->var3_values[ind30];

	if (ind10 == (var1_dim - 1)) //Assures constant upper extrapolation of first variable
		ind11 = ind10;
	else
		dx1 = get_tbl(slot)->var1_values[ind11] - get_tbl(slot)->var1_values[ind10];

	if (ind20 == (var2_dim - 1)) //Assures constant upper extrapolation of second variable
		ind21 = ind20;
	else
		dx2 = get_tbl(slot)->var2_values[ind21] - get_tbl(slot)->var2_values[ind20];

	if (ind30 == (var3_dim - 1)) //Assures constant upper extrapolation of third variable
		ind31 = ind30;
	else
		dx3 = get_tbl(slot)->var3_values[ind31] - get_tbl(slot)->var3_values[ind30];

	if (dx1 > EPS) dumx1 = diff1 / dx1;
	if (dx2 > EPS) dumx2 = diff2 / dx2;
	if (dx3 > EPS) dumx3 = diff3 / dx3;
	//int ind10,int ind11,int ind20,int ind21,int ind30,int ind31
	//      i        i+1        j         j+1       k        k+1
	// Use innner x1 and outer variable x3 for 2DIM interpolation, middle variable x2 is parameter
	// For parameter ind20
	double y11 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind20 * var3_dim + ind30];
	double y12 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind20 * var3_dim + ind30 + var2_dim * var3_dim];
	double y31 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind20 * var3_dim + ind31];
	double y32 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind20 * var3_dim + ind31 + var2_dim * var3_dim];
	//2DIM interpolation
	double y1 = dumx1 * (y12 - y11) + y11;
	double y3 = dumx1 * (y32 - y31) + y31;
	double y21 = dumx3 * (y3 - y1) + y1;

	// For parameter ind21
	y11 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind21 * var3_dim + ind30];
	y12 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind21 * var3_dim + ind30 + var2_dim * var3_dim];
	y31 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind21 * var3_dim + ind31];
	y32 = get_tbl(slot)->data[ind10 * var2_dim * var3_dim + ind21 * var3_dim + ind31 + var2_dim * var3_dim];
	//2DIM interpolation
	y1 = dumx1 * (y12 - y11) + y11;
	y3 = dumx1 * (y32 - y31) + y31;
	double y22 = dumx3 * (y3 - y1) + y1;

	//1DIM interpolation between the middle variable 
	return dumx2 * (y22 - y21) + y21;
}

