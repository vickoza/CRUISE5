///////////////////////////////////////////////////////////////////////////////
// FILE 'utility_functions.cpp'
//
//'Matrix' class member functions
//Module utility functions:
//	mat2tr
//	mat3tr
//	cadtge
//	cadtei
//	cadsph
//	cadtbv
//	cadine
//	sign
//	angle
//Table look-up,'Table' and 'Datadeck' class member functions
//Integration
//US76 Atmosphere
//
//010628 Created by Peter H Zipfel
//020829 Dynamically dimensioned utilities, PZi
//030319 Aded US76 atmosphere, PZi
//030424 General matrix integration, PZi
//030519 Overloaded operator [] for vector, PZi
//060510 Updated from F16C for CRUISE, PZi
//170114 Corrected 'row_vec(const int &row)', PZi
//191020 Corrected 'operator[]', PZi
///////////////////////////////////////////////////////////////////////////////

#include "utility_header.hpp"
#include "global_header.hpp"

using namespace std;



///////////////////////////////////////////////////////////////////////////////
////////////////////  Integration functions  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Integration of scalar state variable
//Modified Euler method
//Example first order lag:
//			phid_new=(phic-phi)/tphi;
//			phi=integrate(phid_new,phid,phi,int_step);
//			phid=phid_new;
//050203 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
double integrate(const double &dydx_new,const double &dydx,const double &y,const double &int_step)
{
	return y+(dydx_new+dydx)*int_step/2;
}

///////////////////////////////////////////////////////////////////////////////
// US Standard Atmosphere 1976
// Calculates the atmospheric properties density pressure and temperature 
//	up to 85 km.
// Extrapolation above 71 km and beyond 85 km is carried out from 71 km altitude 
// Ref: Public Domain Aeronautical Software (see Web) Fortran Code
//
// Argument Output:
//					rho=Air density - kg/m^3
//					press= Air static pressure - Pa
//					tempk= Air temperature - degKelvin
// Argument Input:
//					balt= Geometrical altitude above S.L. - m
//
// 030318 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

void atmosphere76(double &rho,double &press,double &tempk, const double balt)
{
	double rearth(6369.0); //radius of the earth - km
	double gmr(34.163195); //gas constant
	double rhosl(1.22500); //sea level density - kg/m^3
	double pressl(101325); //sea level pressure - Pa
	double tempksl(288.15); //sea level temperature - dK

	double htab[8]={0.0, 11.0, 20.0, 32.0, 47.0, 51.0, 71.0, 84.852}; //altitude
	double ttab[8]={288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65, 186.946}; //temperture
	double ptab[8]={1.0, 2.233611e-1, 5.403295e-2, 8.5666784e-3, 1.0945601e-3,
					6.6063531e-4, 3.9046834e-5, 3.68501e-6};  //pressure
	double gtab[8]={-6.5, 0.0, 1.0, 2.8, 0.0, -2.8, -2.0, 0.0};   //temperture gradient

	double delta(0);

	//convert geometric (m) to geopotential altitude (km)
	double alt=balt/1000; 
	double h=alt*rearth/(alt+rearth);

	//binary search determines altitude table entry i below actual altitude 
	int i(0); //offset of first value in table
	int j(7); //offset of last value in table
	for( ; ; ){
	  int k=(i+j)/2;     //integer division
	  if(h<htab[k])
	    j=k;
	  else
	    i=k;
	  if(j<=(i+1))break;
	}

	//normalized temperature 'theta' from table look-up and gradient interpolation
	double tgrad=gtab[i];
	double tbase=ttab[i];
	double deltah=h-htab[i];
	double tlocal=tbase+tgrad*deltah;
	double theta=tlocal/ttab[0]; 
	
	//normalized pressure from hydrostatic equations 
	if(tgrad==0)
	  delta=ptab[i]*exp(-gmr*deltah/tbase);
	else
	  delta=ptab[i]*pow((tbase/tlocal),(gmr/tgrad));

	//normalized density
	double sigma=delta/theta;

	//output
	rho=rhosl*sigma;
	press=pressl*delta;
	tempk=tempksl*theta;
}

