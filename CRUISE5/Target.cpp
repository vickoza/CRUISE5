#include "Target.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//Constructor initializing the modules and the module-variable arrays
//				  
//010205 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Target::Target(Module* module_list, int num_modules)
{
	//creating module-variable array
	try { target.resize(NTARGET); }
	catch (bad_alloc xa) { cerr << "*** Error: target[] allocation failed ***\n"; exit(1); }

	//zeroing module-variable array
	for (auto item : target) item.init("empty", 0, " ", " ", " ", " ");
	//calling initializer modules to build 'round3' and 'target' arrays
	//and make other initial calculations 

	//call the module definitions
	for (int j = 0; j < num_modules; j++)
	{
		if ((module_list[j].name == "environment") && (module_list[j].definition == "def"))
			def_environment();
		else if ((module_list[j].name == "forces") && (module_list[j].definition == "def"))
			def_forces();
		else if ((module_list[j].name == "newton") && (module_list[j].definition == "def"))
			def_newton();
		else if ((module_list[j].name == "intercept") && (module_list[j].definition == "def"))
			def_intercept();
	}
	//sizing module-variable array 'com_target3'
	sizing_arrays();

	// allocating memory for the com index arrays
	round3_com_ind = new int[round3_com_count];
	target_com_ind = new int[target_com_count];

	try { com_target3 = new Variable[ncom_target3]; }
	catch (bad_alloc xa) { cerr << "*** Error: com_cruise3[] allocation failed *** \n"; exit(1); }

	//building the index arrays of the data to be loaded into the packets of 'combus'
	com_index_arrays();
}
///////////////////////////////////////////////////////////////////////////////
//Destructor deallocating dynamic memory
//				  
//010205 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
Target::~Target()
{
	delete[] round3_com_ind;
	delete[] target_com_ind;
	delete[] com_target3;
}


///////////////////////////////////////////////////////////////////////////////
//Reading input data from input file 'input.asc' for each vehicle object
//Assigning initial values to module-variables of 'round3' and 'target' arrays
//Reading aero and propulsion decks
//
//The first vehicle 'TARGET3' of 'input.asc' reads until the first 'END' after
// 'TARGET3'. The second vehicle object reads untile the second 'END', etc until
// the data for all vehicle objects are read
//
//Output:	target3_name ('Target' data member)
//			round3[] data values (Round3 data member)
//			target[] data values ('Target' data member)
//
//Limitation: real and integer variables only (could be expanded to vectors)			 
//
//001230 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::vehicle_data(fstream& input)
{

	char line_clear[CHARL];
	char read[CHARN];	//name of variable read from input.asc
	const char* buff;			//name of module-variable
	double data;		//data of module-variable 
	const char* comment = "//";
	const char* integer;
	int int_data;
	int file_ptr = NULL;
	int e(0);
	const char* watchpoint = NULL;
	int i(0);

	input.getline(target3_name, CHARL, '\n');

	//reading data until END is encountered
	do
	{
		//reading variable data into module-variable arrays
		input >> read;
		if (strpbrk(comment, read))
		{
			input.getline(line_clear, CHARL, '\n');
		}
		else
		{
			for (i = 0; i < NROUND3; i++)
			{
				buff = round3[i].get_name();
				if (!strcmp(buff, read))
				{
					input >> data;
					//checking for integers
					integer = round3[i].get_type();
					if (!strcmp(integer, "int"))
					{
						//loading interger value
						int_data = (int)data;
						round3[i].gets(int_data);
						input.getline(line_clear, CHARL, '\n');
					}
					else
					{
						//loading real value
						round3[i].gets(data);
						input.getline(line_clear, CHARL, '\n');
					}
				}
			}
			for (i = 0; i < NTARGET; i++)
			{
				buff = target[i].get_name();
				if (!strcmp(buff, read))
				{
					input >> data;
					//checking for integers
					integer = target[i].get_type();
					if (!strcmp(integer, "int"))
					{
						//loading interger value
						int_data = (int)data;
						target[i].gets(int_data);
						input.getline(line_clear, CHARL, '\n');
					}
					else
					{
						//loading real value
						target[i].gets(data);
						input.getline(line_clear, CHARL, '\n');
					}
				}
			}
		} //end of reading non-comment lines
	} while (strcmp(read, "END")); //reached 'END' of first vehicle object

	//flushing the line after END and starting new line
	input.getline(line_clear, CHARL, '\n');

	//diagnostic: file pointer
	file_ptr = int(input.tellg());
}
///////////////////////////////////////////////////////////////////////////////
//Determining dimensions of arrays: 'com_target'
// 
//Out to Target:: round3_com_count, target_com_count, ncom_target3 		 ,
//
//010207 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
void Target::sizing_arrays()
{
	const char* key4 = "com";

	round3_com_count = 0;
	target_com_count = 0;
	int i(0);

	//counting in 'round3' array
	for (i = 0; i < NROUND3; i++)
	{
		if (strstr(round3[i].get_out(), key4))
			round3_com_count++;
	}
	//counting in 'target' array
	for (i = 0; i < NTARGET; i++)
	{
		if (strstr(target[i].get_out(), key4))
			target_com_count++;
	}
	//output to Target::protected
	ncom_target3 = round3_com_count + target_com_count;
}

///////////////////////////////////////////////////////////////////////////////
//Building index array of those 'round3[]' and target[] variables 
// that are output to 'combus' 'data'  
//
//Out to Target::round3_com_ind[], target_com_ind[] 
//
//010210 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::com_index_arrays()
{
	const char* test = "com";
	int k(0);
	int i(0);

	for (i = 0; i < NROUND3; i++)
	{
		if (strstr(round3[i].get_out(), test))
		{
			round3_com_ind[k] = i;
			k++;
		}
	}
	int l = 0;
	for (i = 0; i < NTARGET; i++)
	{
		if (strstr(target[i].get_out(), test))
		{
			target_com_ind[l] = i;
			l++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Initializing loading 'packet' of target
//
//Uses C-code 'sprintf' function to convert int to char
//Differs from 'loading_packet' only by initializing 'status=1'
//
//010401 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Packet Target::loading_packet_init(int num_cruise, int num_target, int num_satellite)
{
	string id;
	char object[4];
	static int c_count = 0;
	int index;
	int i(0);
	int j(0);

	c_count++;
	if (c_count == (num_target + 1))c_count = 1;
	sprintf(object, "%i", c_count);
	id = "t" + string(object);

	//building 'data' array of module-variables
	for (i = 0; i < round3_com_count; i++)
	{
		index = round3_com_ind[i];
		com_target3[i] = round3[index];
	}
	for (j = 0; j < target_com_count; j++)
	{
		index = target_com_ind[j];
		com_target3[i + j] = target[index];
	}
	//refreshing the packet
	packet.set_id(id);
	packet.set_status(1);
	packet.set_data(com_target3);
	packet.set_ndata(ncom_target3);

	return packet;
}
///////////////////////////////////////////////////////////////////////////////
//Loading 'packet' of target
//
//Uses C-code 'sprintf' function to convert int to char
//
//010207 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Packet Target::loading_packet(int num_cruise, int num_target, int num_satellite)
{
	int index(0);
	int i(0);
	int j(0);

	//building 'data' array of module-variables
	for (i = 0; i < round3_com_count; i++)
	{
		index = round3_com_ind[i];
		com_target3[i] = round3[index];
	}
	for (j = 0; j < target_com_count; j++)
	{
		index = target_com_ind[j];
		com_target3[i + j] = target[index];
	}
	//refreshing the packet
	packet.set_data(com_target3);
	packet.set_ndata(ncom_target3);

	return packet;
}

///////////////////////////////////////////////////////////////////////////////
//Composing documention on file 'doc.asc'
//Listing 'target' module-variable arrays
//
//010214 Created by Peter Zipfel
//020911 Added module-variable error flagging, PZi
//060929 Modification to accomodate C++8, PZi
///////////////////////////////////////////////////////////////////////////////

void Target::document(ostream& fdoc, const char* title, Document* doc_target3)
{
	int i(0);
	int j(0);

	fdoc << "\n\n                                       Target Module-Variable Array \n\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";
	fdoc << "|LOC|        NAME       |                   DEFINITION                        |   MODULE   | PURPOSE |    OUTPUT    |\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";

	char name_error_code[] = "A";
	for (i = 0; i < NTARGET; i++)
	{
		for (j = 0; j < i; j++) {
			if (!strcmp(target[i].get_name(), target[j].get_name()) && strcmp(target[i].get_name(), "empty"))
				target[i].put_error(name_error_code);
		}
		if (!strcmp(target[i].get_error(), "A")) cout << " *** Error code 'A': duplicate name in target[] array, see 'doc.asc' ***\n";
		if (!strcmp(target[i].get_error(), "*")) cout << " *** Error code '*': duplicate location in target[] array, see 'doc.asc' ***\n";

		fdoc << target[i].get_error();
		fdoc.setf(ios::left);
		fdoc.width(4); fdoc << i;
		if (!strcmp(target[i].get_type(), "int"))
		{
			fdoc.width(15); fdoc << target[i].get_name();
			fdoc.width(5); fdoc << " int ";
		}
		else
		{
			fdoc.width(20); fdoc << target[i].get_name();
		}
		fdoc.width(54); fdoc << target[i].get_def();
		fdoc.width(13); fdoc << target[i].get_mod();
		fdoc.width(10); fdoc << target[i].get_role();
		fdoc << target[i].get_out();
		fdoc << "\n";
		if (!((i + 1) % 10))fdoc << "----------------------------------------------------------------------------------------------------------------------\n";
	}
	//building doc_target3[] for documenting 'input.asc' and eliminating 'empty' slots
	int counter = 0;
	for (i = 0; i < NTARGET; i++) {
		if (strcmp(target[i].get_name(), "empty")) {
			doc_target3[counter].put_doc_offset(counter);
			doc_target3[counter].put_name(target[i].get_name());

			//z060929			doc_target3[counter].put_type(target[i].get_type());

			const char* dum;
			dum = target[i].get_type();
			if (*dum != -51)
				doc_target3[counter].put_type(dum);
			//z060929-end

			doc_target3[counter].put_def(target[i].get_def());
			doc_target3[counter].put_mod(target[i].get_mod());
			counter++;
		}
	}
	for (i = 0; i < NROUND3; i++) {
		if (strcmp(round3[i].get_name(), "empty")) {
			doc_target3[counter].put_doc_offset(counter);
			doc_target3[counter].put_name(round3[i].get_name());

			//z060929			doc_target3[counter].put_type(round3[i].get_type());

			const char* dum;
			dum = round3[i].get_type();
			if (*dum != -51)
				doc_target3[counter].put_type(dum);
			//z060929-end

			doc_target3[counter].put_def(round3[i].get_def());
			doc_target3[counter].put_mod(round3[i].get_mod());
			counter++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Definition of force module-variables
//Member function of class 'Target'
//Module-variable locations are assigned to target[10-14]
//
//Note that FSPV is entered into the round3[10] array because it is needed
//for the newton module, which is a member of the 'Round3' class
//		
//010205 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::def_forces()
{
	//Definition of module-variables
	round3[10].init("FSPV", 0, 0, 0, "Specific force in V-coord - m/s^2", "forces", "out", "");
	target[11].init("fwd_accel", 0, "Forward acceleration - m/s^2", "forces", "data", "");
	target[12].init("side_accel", 0, "Sideward acceleration - m/s^2", "forces", "data", "");
	target[13].init("CORIO_V", 0, "Coriolis acceleration - m/s^2", "forces", "diag", "");
	target[14].init("CENTR_V", 0, "Centrifugal acceleration - m/s^2", "forces", "diag", "");
}

///////////////////////////////////////////////////////////////////////////////
//Force Module 
//Member function of class 'Target' 
//Provides the centrifugal and Coriolis accelerations of the target in order for it
// to move on the (rotating) Earth
//
//010205 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::forces()
{
	//local variables
	double fspv1(0);
	double fspv2(0);
	double fspv3(0);
	Matrix GRAV_G(3, 1);
	Matrix GRAV_V(3, 1);
	Matrix ACC_V(3, 1);
	Matrix TVG(3, 3);
	Matrix TGI(3, 3);
	Matrix TEG(3, 3);
	Matrix WEIG(3, 3);

	//local module-variables
	Matrix CORIO_V(3, 1);
	Matrix CENTR_V(3, 1);
	Matrix FSPV(3, 1);

	//localizing module-variables
	//input data
	double fwd_accel = target[11].real();
	double side_accel = target[12].real();
	//input from other modules
	double grav = round3[11].real();
	double thtvgx = round3[29].real();
	Matrix TGV = round3[22].mat();
	Matrix TIG = round3[23].mat();
	Matrix TGE = round3[33].mat();
	Matrix WEII = round3[27].mat();
	Matrix VBEG = round3[32].vec();
	Matrix SBII = round3[35].vec();
	//-------------------------------------------------------------------------
	//transposing
	TVG = TGV.trans();
	TGI = TIG.trans();
	TEG = TGE.trans();

	//Coriolis acceleration in V-coordinates (note; WEIE=WEII)
	WEIG = TGE * WEII * TEG;
	CORIO_V = TVG * WEIG * VBEG * (2);

	//centrifugal acceleration in V-coordinates
	CENTR_V = TVG * WEIG * WEIG * TGI * SBII;

	//gravitational acceleration in V-coordiantes
	GRAV_G.assign_loc(2, 0, grav);
	GRAV_V = TVG * GRAV_G;

	//adding apparant and gravitational accelerations
	ACC_V = CORIO_V + CENTR_V - GRAV_V;

	//combining with commanded accelerations
	fspv1 = ACC_V.get_loc(0, 0) + fwd_accel;
	fspv2 = ACC_V.get_loc(1, 0) + side_accel;
	fspv3 = ACC_V.get_loc(2, 0);

	FSPV.assign_loc(0, 0, fspv1);
	FSPV.assign_loc(1, 0, fspv2);
	FSPV.assign_loc(2, 0, fspv3);
	//-------------------------------------------------------------------------
	//loading module-variables
	//ouput to other modules
	round3[10].gets_vec(FSPV);
	//diagnostics
	target[13].gets_vec(CORIO_V);
	target[14].gets_vec(CENTR_V);
}
///////////////////////////////////////////////////////////////////////////////
//Definition of intercept module-variables
//Member function of class 'Target'
//Module-variable locations are assigned to cruise[15]
//		
//010420 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::def_intercept()
{
	//definition of module-variables
	target[15].init("targ_health", "int", 0, "Target health status - ND", "intercept", "diag", "");
}
///////////////////////////////////////////////////////////////////////////////
//Intercept module
//Member function of class 'Target'
//Copies health of target from 'combus' to module-variable 'targ_health' so it can be used 
//in 'event' blocks of 'input.asc'
//
//status =	1: alive
//			0: dead
//		   -1: hit (but not dead)
//
//010420 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

void Target::intercept(Packet* combus, int vehicle_slot, double int_step, const char* title)
{
	//local module-variables
	int targ_health(0);
	//-------------------------------------------------------------------------
	//get target health from 'combus'
	targ_health = combus[vehicle_slot].get_status();
	//-------------------------------------------------------------------------
	//loading module-variables
	//diagnostics
	target[15].gets(targ_health);
}

