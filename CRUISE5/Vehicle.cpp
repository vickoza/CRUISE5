#include "Vehicle.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//////////////////// Members of class 'Vehicle' ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Constructor of class 'Vehicle'
//allocating dynamic memory for the array of pointers of type 'Cadac'
//and returning the pointer to array
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Vehicle::Vehicle(int number)
{
	capacity = number;
	try { vehicle_ptr = new Cadac * [capacity]; }
	catch (bad_alloc xa) { cerr << "*** Error:'vehicle_ptr' allocation failed *** \n"; exit(1); }
	howmany = 0;
	//	cerr<<">>> inside constructor of 'Vehicle' <<<\n";
}

///////////////////////////////////////////////////////////////////////////////
//Destructor of class 'Vehicle'
//de-allocating dynamic memory of the pointer-array of type 'Cadac'
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Vehicle::~Vehicle()
{
	//	cerr<<">>> inside destructor of 'Vehicle' <<<\n";
	delete[] vehicle_ptr;
}

///////////////////////////////////////////////////////////////////////////////
//Adding a vehicle pointer to the vehicle list
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Vehicle::add_vehicle(Cadac& pt)
{
	if (howmany < capacity)
		vehicle_ptr[howmany++] = &pt;
}
///////////////////////////////////////////////////////////////////////////////
//Overloading operator [] so that it returns a 'Cadac' pointer
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Cadac* Vehicle::operator[](int position)
{
	if (position >= 0 && position < howmany)
		return vehicle_ptr[position];
	else
	{
		cout << "*** Bad value: " << position << '\n';
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Obtaining size of vehicle list (total number of vehicles)
//
//010626 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
int Vehicle::size()
{
	return howmany;
}
