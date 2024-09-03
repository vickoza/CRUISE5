#pragma once
#include "utility_header.hpp"
#include "Matrix.hpp"

///////////////////////////////////////////////////////////////////////////////
//Class 'Variable'
//Establishing module-variables as type Variable
//Provides the class for the variables used in modules
//
//001125 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

class Variable
{
private:
	char name[CHARN] = {}; //label of variable
	char type[CHARN] = {}; //type of variable 'int'; default is real
	double rval = 0;	  //real value
	int ival = 0;         //integer value
	Matrix VEC;       //3x1 vector 
	Matrix MAT;       //3x3 matrix 
	char def[CHARL] = {};  //definition and units
	char mod[CHARN] = {};  //module name where variable is calculated
	char role[CHARN] = {}; //role that variable plays: 'data', 'state', 'diag', 'out'
	char out[CHARN] = {};  //output for: 'scrn', 'plot', 'com'
	char error[2];	  //error code '*' = SAME LOCATION multiple, overwritten definitions
	//           'A' = SAME NAME assigned to multiple locations 
public:
	Variable()
	{
		VEC.dimension(3, 1); MAT.dimension(3, 3);
		strcpy_s(name, "empty");
		error[0] = ' '; error[1] = '\0';
		int dum = 1;
	};
	~Variable() {};

	//////////////////////////// Protopypes ///////////////////////////////////////
	void init(const char* na, double rv, const char* de, const char* mo, const char* ro, const char* ou);
	void init(const char* na, const char* ty, int iv, const char* de, const char* mo, const char* ro, const char* ou);
	void init(const char* na, double v1, double v2, double v3, const char* de, const char* mo, const char* ro, const char* ou);
	void init(const char* na, double v11, double v12, double v13, double v21, double v22, double v23,
		double v31, double v32, double v33, const char* de, const char* mo, const char* ro, const char* ou);

	////////////////////////// Inline Functions ///////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'name' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_name() { return name; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'type' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_type() { return type; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining value (of type 'double') from module-variable array to local variable
	//Example: thrust_com=Variable::cruise[13].real();
	//
	//001128 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	double real() { return rval; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining value (of type 'int') from module-variable array to local variable
	//Example: mprop=Variable::cruise[10].integer();
	//
	//001128 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	int integer() { return ival; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining vector value (of type 'Matrix') from module-variable array to local variable
	//Example: FSPV=Variable::round3[200].vec();
	//
	//001128 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	Matrix vec() { return VEC; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining matrix value (of type 'Matrix') from module-variable array to local variable
	//Example: TGV=Variable::round3[22].mat();
	//
	//001226 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	Matrix mat() { return MAT; }

	///////////////////////////////////////////////////////////////////////////
	//Four-times overloaded function gets()
	//Loads module-variable onto module-variable array
	//Overloaded for real, integer and vector variables
	//Example: cruise[10].gets(mprop);
	//
	//001128 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	double gets(double rv)
	{
		rval = rv;
		return rval;
	}

	double gets(int iv)
	{
		ival = iv;
		return ival;
	}
	Matrix gets_vec(Matrix VE)
	{
		VEC = VE;
		return VEC;
	}

	Matrix gets_mat(Matrix MA)
	{
		MAT = MA;
		return MAT;
	}

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'def' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_def() { return def; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'mod' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_mod() { return mod; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'role' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_role() { return role; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'out' from module-variable array 
	//
	//001213 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_out() { return out; }

	///////////////////////////////////////////////////////////////////////////
	//Obtaining 'error' code from module-variable array 
	//
	//020909 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_error() { return error; }

	///////////////////////////////////////////////////////////////////////////
	//Putting 'error' code into module-variable  
	//Error code must be single caracter
	//
	//020911 Created by Peter Zipfel
	///////////////////////////////////////////////////////////////////////////
	void put_error(const char* error_code) { strcpy_s(error, error_code); }
};

