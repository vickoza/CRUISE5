#include "Variable.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////// Definition of Member functions of class 'Variable' //////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'real'
//
// private class member output: name, rval, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////
void Variable::init(const char* na, double rv, const char* de, const char* mo, const char* ro, const char* ou)
{

	if (strcmp(name, "empty") != 0) error[0] = '*';
	strcpy(name, na);
	rval = rv;
	strcpy(def, de);
	strcpy(mod, mo);
	strcpy(role, ro);
	strcpy(out, ou);
}
///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'int'
//
// privat class member output: name, type, ival, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////
void Variable::init(const char* na, const char* ty, int iv, const char* de, const char* mo, const char* ro, const char* ou)
{
	if (strcmp(name, "empty") != 0) error[0] = '*';
	strcpy(name, na);
	strcpy(type, ty);
	ival = iv;
	strcpy(def, de);
	strcpy(mod, mo);
	strcpy(role, ro);
	strcpy(out, ou);
}
///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'Matrix' for 3x1 vectors
//
//private class member output: name, VEC, def, mod, role, out
//
//001128 Created by Peter Zipfel
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////

void Variable::init(const char* na, double v1, double v2, double v3, const char* de, const char* mo, const char* ro, const char* ou)
{
	double* pbody;
	pbody = VEC.get_pbody();
	*pbody = v1;
	*(pbody + 1) = v2;
	*(pbody + 2) = v3;

	if (strcmp(name, "empty") != 0) error[0] = '*';
	strcpy(name, na);
	strcpy(def, de);
	strcpy(mod, mo);
	strcpy(role, ro);
	strcpy(out, ou);
}

///////////////////////////////////////////////////////////////////////////////
//Initialization of module-variables of type 'Matrix' for 3x3 matrices
//
//private class member output: name, MAT, def, mod, role, out
//
//001226 Created by Peter Zipfel
//020104 Corrected element assigment errors, PZi
//020909 Added error code handling, PZi
///////////////////////////////////////////////////////////////////////////////

void Variable::init(const char* na, double v11, double v12, double v13, double v21, double v22, double v23,
	double v31, double v32, double v33, const char* de, const char* mo, const char* ro, const char* ou)
{
	double* pbody;
	pbody = MAT.get_pbody();
	*pbody = v11;
	*(pbody + 1) = v12;
	*(pbody + 2) = v13;
	*(pbody + 3) = v21;
	*(pbody + 4) = v22;
	*(pbody + 5) = v23;
	*(pbody + 6) = v31;
	*(pbody + 7) = v32;
	*(pbody + 8) = v33;

	if (strcmp(name, "empty") != 0) error[0] = '*';
	strcpy(name, na);
	strcpy(def, de);
	strcpy(mod, mo);
	strcpy(role, ro);
	strcpy(out, ou);
}
