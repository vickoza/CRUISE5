#pragma once
#include "Cadac.hpp"

///////////////////////////////////////////////////////////////////////////////
////////////////////////// Global class 'Vehicle'//////////////////////////////
///////////// must be located after 'Cadac' hierarchy in this file ////////////
///////////////////////////////////////////////////////////////////////////////
//Class 'Vehicle'
//
//Global class for typifying the array of vehicle pointers
//
//010629 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
class Vehicle
{
private:
	int capacity = 0;	//max number of vehicles permitted in vehicle list
	int howmany = 0;	//actual number of vehicles in vehicle list 
	Cadac** vehicle_ptr = nullptr; //'vehicle_ptr' is the pointer to an array of pointers of type 'Cadac'
public:
	Vehicle(int number = 1);	//constructor, setting capacity, allocating dynamic memory
	virtual ~Vehicle();	//destructor, de-allocating dynamic memory
	void add_vehicle(Cadac& ptr);	//adding vehicle to list
	Cadac* operator[](int position);	//[] operator returns vehicle pointer
	int size();	//returning 'howmany' vehicles are stored in vehicle list
};
