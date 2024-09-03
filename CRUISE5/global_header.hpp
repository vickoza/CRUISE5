///////////////////////////////////////////////////////////////////////////////
//FILE: 'global_header.hpp'
//Declares the classes: 'Module', 'Variable', 'Document', 'Event', 'Packet'
// and the stuctures 'Module' and 'Targeting' 
//
//001206 Created by Peter Zipfel
//060510 Updated from F16C for CRUISE, PZi
///////////////////////////////////////////////////////////////////////////////

//preventing warnings in MS C++8 for not using security enhanced CRT functions 
#define _CRT_SECURE_NO_DEPRECATE

#ifndef global_header__HPP
#define global_header__HPP

//#include <fstream>
#include <string>		
#include "utility_header.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Structure 'Module'
//
//Provides the structure for the modules of the simulation
//
//010107 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
struct Module
{
	std::string name="";
	std::string definition="";
	std::string initialization="";
	std::string execution="";
	std::string termination="";
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Structure 'Targeting'
//
//Provides the structure for an array that identifies whether a satellite 
// is able to provide targeting data to the missiles or not
//
//010814 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
struct Targeting
{
	int vehicle_slot=0; //slot # in 'vehicle_list' (or 'combus')
	int tracking=0; //no=0; yes=1;
};

#endif
