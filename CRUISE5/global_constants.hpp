///////////////////////////////////////////////////////////////////////////////
//FILE: 'global_constants.hpp'
//Defines all global constant parameters
//
//011128 Created by Peter H Zipfel
//060510 Updated from F16C for CRUISE, PZi
///////////////////////////////////////////////////////////////////////////////

#ifndef global_constants__HPP
#define global_constants__HPP
#include <numbers>
#ifdef DIAGNOSTICS
constexpr auto enableDiagostics{ true };
#else
constexpr auto enableDiagostics{ false };
#endif // DIAGNOSTICS


//global constants to be used in the simulation
constexpr double const REARTH=6370987.308;		//mean earth radius - m
constexpr double const WEII3=7.292115e-5;			//angular rotation of earth - rad/s
constexpr double const RAD=0.0174532925199432;	//conversion factor deg->rad
constexpr double const DEG=57.2957795130823;		//conversion factor rad->deg
constexpr double const AGRAV=9.80675445;			//standard value of gravity acceleration - m/s^2 
constexpr double const G=6.673e-11;				//universal gravitational constant - Nm^2/kg^2 
constexpr double const EARTH_MASS=5.973e24;		//mass of the earth - kg 
constexpr double const R=287.053;					//ideal gas constant - m^2/(K*s^2) 
constexpr double const PI=std::numbers::pi;		//circumference of unit diameter circle
constexpr double const EPS=1e-10;					//machine precision error
constexpr double const SMALL=1.e-7;				//small real number
constexpr int const ILARGE=9999;					//large integer number
constexpr double const BIG=1e10;					//big number
//sizing of arrays
constexpr int const CHARN=40;						//character numbers in variable names
constexpr int const CHARL=150;					//character numbers in a line
//verify the following array sizes. If too small, dynamic memory allocations may fail!
constexpr int const NROUND3=40;					//size of 'round3' module-variable array
constexpr int const NCRUISE=160;					//size of 'cruise' module-variable array 
constexpr int const NTARGET=20;					//size of 'target' module-variable array
constexpr int const NSATELLITE=20;				//size of 'satellite' module-variable array
constexpr int const NEVENT=25;					//max number of events
constexpr int const NVAR=15;						//max number of variables to be input at every event 

#endif