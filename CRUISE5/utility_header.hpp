///////////////////////////////////////////////////////////////////////////////
//FILE: 'utility_header.hpp'
// Declares the classes 'Matrix', 'Table','Datadeck',    
//
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
//Table look-up, classes 'Table' and 'Datadeck'
//Integration
//US76 Atmosphere
//
//010628 Created by Peter H Zipfel
//020829 Dynamically dimensioned utilities, PZi
//030319 Added US76 atmosphere, PZi
//030424 General matrix integration, PZi
//030519 Overloaded operator [] for vector, PZi
//030725 Removed all reference to table look-up structure, PZi
//060510 Updated from F16C for CRUISE, PZi
//170114 Corrected 'row_vec(const int &row)', PZi
//191020 Corrected 'operator[]', PZi
///////////////////////////////////////////////////////////////////////////////

//preventing warnings in MS C++8 for not using security enhanced CRT functions 
#define _CRT_SECURE_NO_DEPRECATE

#ifndef utility_header__HPP
#define utility_header__HPP

#include <iostream>
#include <cmath>
#include "global_constants.hpp"

using namespace std;


///////////////////////////////////////////////////////////////////////////////
////////////////////  Integration functions  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//Integration of scalar state variable
//Modified Midpoint method 
//Example first order lag:
//			phid_new=(phic-phi)/tphi;
//			phi=integrate(phid_new,phid,phi,int_step);
//			phid=phid_new;
double integrate(const double &dydx_new,const double &dydx,const double &y,const double &int_step);


///////////////////////////////////////////////////////////////////////////////
////////////////////  US Standard Atmosphere 1976 /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
//Calculates the atmospheric properties density pressure and temperature 
// up to 85 km.
void atmosphere76(double &rho,double &press,double &tempk, const double balt);

#endif