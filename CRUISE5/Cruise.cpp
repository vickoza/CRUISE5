#include "Cruise.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
//////////////// Member functions of 'Cadac' class hierarchy //////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//Constructor initializing the modules and the module-variable arrays
//Reading input data from 'input.asc' and putting into 'round3' and 'cruise' arrays 
//Writing banners to screen, 'tabout.asc' and to 'traj.asc' files  
//
//Module-variable arrays:
//	cruise[NCRUISE]		contains variables of modules under class 'Cruise'
//	cruise3[ncruise3]	contains variables of all modules with empty slots removed
//	scrn_cruise3[nscrn_cruise3]	contains variables to be displayed at screen and 'tabout.asc'
//	plot_cruise3[nplot_cruise3] contains variables to be plotted, i.e., written to 'traj.asc'
//	com_cruise3[ncom_cruise3] contains variables for communication among vehicles
//  event_ptr_list[NEVENT] event pointer list
//
//Index pointer arrays:	
//	round3_scrn_ind[round3_scrn_count];
//	cruise_scrn_ind[cruise_scrn_count];
//	round3_plot_ind[round3_plot_count];
//	cruise_plot_ind[cruise_plot_count];
//	round3_com_ind[round3_com_count];
//	cruise_com_ind[cruise_com_count];
//
//001220 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Cruise::Cruise(Module* module_list, int num_modules, int num_target, int num_satellite)
{
	//initializing coutners
	int i(0);
	int j(0);

	//creating module-variable array
	try { cruise.resize(NCRUISE); }
	catch (bad_alloc xa) { cerr << "*** Error: cruise[] allocation failed ***\n"; exit(1); }

	//zeroing module-variable array
	for (i = 0; i < NCRUISE; i++)cruise[i].init("empty", 0, " ", " ", " ", " ");

	//calling initializer modules to build 'round3' and 'cruise' arrays
	// and make other initial calculations in the following sequence

	//call the module definitions -MOD
	for (j = 0; j < num_modules; j++)
	{
		if ((module_list[j].name == "environment") && (module_list[j].definition == "def"))
			def_environment();
		else if ((module_list[j].name == "newton") && (module_list[j].definition == "def"))
			def_newton();
		else if ((module_list[j].name == "aerodynamics") && (module_list[j].definition == "def"))
			def_aerodynamics();
		else if ((module_list[j].name == "propulsion") && (module_list[j].definition == "def"))
			def_propulsion();
		else if ((module_list[j].name == "forces") && (module_list[j].definition == "def"))
			def_forces();
		else if ((module_list[j].name == "control") && (module_list[j].definition == "def"))
			def_control();
		else if ((module_list[j].name == "targeting") && (module_list[j].definition == "def"))
			def_targeting();
		else if ((module_list[j].name == "guidance") && (module_list[j].definition == "def"))
			def_guidance();
		else if ((module_list[j].name == "seeker") && (module_list[j].definition == "def"))
			def_seeker();
		else if ((module_list[j].name == "intercept") && (module_list[j].definition == "def"))
			def_intercept();
	}

	//sizing module-variable arrays 'cruise3','scrn_cruise3','plot_cruise3' arrays
	//their dimensions are the protected data:'ncruise3','nscrn_cruise3','nplot_cruise3'
	sizing_arrays();

	//allocating dynamic memory to the module-variable arrays
	try { cruise3 = new Variable[ncruise3]; }
	catch (bad_alloc xa) { cerr << "*** Error: cruise3[] allocation failed *** \n"; exit(1); }

	try { scrn_cruise3 = new Variable[nscrn_cruise3]; }
	catch (bad_alloc xa) { cerr << "*** Error: scrn_cruise3[] allocation failed *** \n"; exit(1); }

	try { plot_cruise3 = new Variable[nplot_cruise3]; }
	catch (bad_alloc xa) { cerr << "*** Error: plot_cruise3[] allocation failed *** \n"; exit(1); }

	try { com_cruise3 = new Variable[ncom_cruise3]; }
	catch (bad_alloc xa) { cerr << "*** Error: com_cruise3[] allocation failed *** \n"; exit(1); }

	// allocating memory for the screen index arrays
	round3_scrn_ind = new int[round3_scrn_count];
	cruise_scrn_ind = new int[cruise_scrn_count];

	// allocating memory for the plot index arrays
	round3_plot_ind = new int[round3_plot_count];
	cruise_plot_ind = new int[cruise_plot_count];

	// allocating memory for the com index arrays
	round3_com_ind = new int[round3_com_count];
	cruise_com_ind = new int[cruise_com_count];

	//allocating memory for the 'grnd_range' array
	grnd_range = new double[num_target];

	//allocating memory to each event object in event object list
	for (i = 0; i < NEVENT; i++)
		try { event_ptr_list[i] = new Event; }
	catch (bad_alloc xa) { cerr << "*** Error: 'event' object allocation failed *** \n"; exit(1); }

	//allocating memory for satellite visibility array
	visibility = new Targeting[num_satellite];

	//initializing the event array index
	nevent = 0;
	event_total = 0;
	event_time = 0;

	//building 'cruise3' array (compacting and merging 'round3' and 'cruise' arrays)
	vehicle_array();

	//building 'scrn_cruise3' array from 'cruise3' array
	scrn_array();

	//building 'plot_cruise3' array from 'cruise3' array
	plot_array();

	//building the index arrays of the data to be written to the screen
	scrn_index_arrays();

	//building the index arrays of the data to be written to the 'ploti.asc' files
	plot_index_arrays();

	//building the index arrays of the data to be loaded into the packets of 'combus'
	com_index_arrays();
}
///////////////////////////////////////////////////////////////////////////////
//Destructor deallocating dynamic memory
//				  
//010115 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Cruise::~Cruise()
{
	delete[] cruise3;
	delete[] scrn_cruise3;
	delete[] plot_cruise3;
	delete[] com_cruise3;
	delete[] round3_scrn_ind;
	delete[] cruise_scrn_ind;
	delete[] cruise_plot_ind;
	delete[] round3_com_ind;
	delete[] cruise_com_ind;
	delete[] grnd_range;
	delete[] & event_ptr_list;
}


///////////////////////////////////////////////////////////////////////////////
//Determining dimensions of arrays: 'cruise3', 'scrn_cruise3', 'plot_cruise3'
//and 'com_cruise3'
// 
//Out to Cruise:: ncruise3, nscrn_cruise3, nplot_cruise3, ncom_cruise3,
//	round3_scrn_count, cruise_scrn_count, round3_plot_count, cruise_plot_count 		 ,
//
//001212 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::sizing_arrays()
{
	const char* key1 = "empty";
	const char* key2 = "scrn";
	const char* key3 = "plot";
	const char* key4 = "com";
	int round3_full_count = 0;
	int cruise_full_count = 0;
	int i(0);

	//initialize 'Round3' and 'Cruise' member variables
	round3_scrn_count = 0;
	cruise_scrn_count = 0;
	round3_plot_count = 0;
	cruise_plot_count = 0;
	round3_com_count = 0;
	cruise_com_count = 0;

	//counting in 'round3' array
	for (i = 0; i < NROUND3; i++)
	{
		if (strcmp(round3[i].get_name(), key1))
			round3_full_count++;
		if (strstr(round3[i].get_out(), key2))
			round3_scrn_count++;
		if (strstr(round3[i].get_out(), key3))
			round3_plot_count++;
		if (strstr(round3[i].get_out(), key4))
			round3_com_count++;
	}
	//counting in 'cruise' array
	for (i = 0; i < NCRUISE; i++)
	{
		if (strcmp(cruise[i].get_name(), key1))
			cruise_full_count++;
		if (strstr(cruise[i].get_out(), key2))
			cruise_scrn_count++;
		if (strstr(cruise[i].get_out(), key3))
			cruise_plot_count++;
		if (strstr(cruise[i].get_out(), key4))
			cruise_com_count++;
	}
	//output to Cruise::protected
	ncruise3 = round3_full_count + cruise_full_count;
	nscrn_cruise3 = round3_scrn_count + cruise_scrn_count;
	nplot_cruise3 = round3_plot_count + cruise_plot_count;
	ncom_cruise3 = round3_com_count + cruise_com_count;
}

///////////////////////////////////////////////////////////////////////////////
//Building 'cruise3' module-array by eliminating empty slots in 'round3' and 'cruise'
//and merging the two arrays 
//
//Output: Cruise::cruise3[] 
//
//001212 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::vehicle_array()
{
	const char* test = "empty";
	int i(0);

	//load nonempty slots from round3 array into cruise3 array
	int k = 0;
	for (i = 0; i < NROUND3; i++)
	{
		if (strcmp(round3[i].get_name(), test))
		{
			cruise3[k] = round3[i];
			k++;
		}
	}
	//load nonempty slots from cruise array into cruise3 array	
	int m = 0;
	for (auto& item : cruise)
	{
		if (strcmp(item.get_name(), test))
		{
			cruise3[k + m] = item;
			m++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Building 'scrn_cruise3' module-array from 'cruise3' array by keying on the word 'scrn'
//
//Output: Cruise::scrn_cruise3[] 
//
//001214 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::scrn_array()
{
	int k(0);
	const char* buff;
	const char* key = "scrn";

	for (int i = 0; i < ncruise3; i++)
	{
		buff = cruise3[i].get_out();
		if (strstr(buff, key))
		{
			scrn_cruise3[k] = cruise3[i];
			k++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Building 'plot_cruise3' module-array from 'cruise3' array by keying on the word 'plot'
//
//Output: Cruise::plot_cruise3[] 
//
//001214 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::plot_array()
{
	int k(0);
	const char* buff;
	const char* key = "plot";

	for (int i = 0; i < ncruise3; i++)
	{
		buff = cruise3[i].get_out();
		if (strstr(buff, key))
		{
			plot_cruise3[k] = cruise3[i];
			k++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Writing out banner of labels to screen
//
//First label is time-since-launch-of-vehicle 'time', always at round3[0]
// eight accross, unlimited down
// data field will be 15 spaces, total width 120 spaces
// labels longer than 14 characters will be truncated
//Accomodates 3x1 vectors only
//
//010106 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::scrn_banner()
{
	const char* buff1;
	char buff2[15];
	int label_length = 14;
	int k(0);

	cout << "\n Vehicle: CRUISE3 " << '\n';

	for (int i = 0; i < nscrn_cruise3; i++)
	{
		cout.setf(ios::left);

		buff1 = scrn_cruise3[i].get_name();

		//truncating if more than 14 characters
		strncpy(buff2, buff1, label_length); //!zi060927
		buff2[14] = 0;

		//vectors are recognized by upper case character 
		if (isupper(buff2[0]))
		{
			for (int i = 1; i < 4; i++)
			{

				cout.width(strlen(buff2));
				cout << buff2; cout.width(15 - strlen(buff2)); cout << i;
				k++;
				if (k > 7) { k = 0; cout << '\n'; }
			}
		}
		else
		{
			cout.width(15);
			cout << buff2;
			k++;
			if (k > 7) { k = 0; cout << '\n'; }
		}
	}
	cout << "\n\n";
}
///////////////////////////////////////////////////////////////////////////////
//Writing out banner of labels to 'tabout.asc'
//
//First label is time-since-launch-of-vehicle 'time', always at round3[0]
// eight accross, unlimited down
// data field will be 15 spaces, total width 120 spaces
// labels longer than 14 characters will be truncated
//Accomodates 3x1 vectors only
//
//010114 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::tabout_banner(ofstream& ftabout, const char* title)
{
	const char* buff1;
	char buff2[15];
	int label_length = 14;
	int k(0);

	ftabout << "\n" << title << "   " << __DATE__ << " " << __TIME__ << "\n";
	ftabout << "\n Vehicle: CRUISE3 " << '\n';


	for (int i = 0; i < nscrn_cruise3; i++)
	{
		ftabout.setf(ios::left);

		buff1 = scrn_cruise3[i].get_name();

		//truncating if more than 14 characters
		strncpy(buff2, buff1, label_length);
		buff2[14] = 0;

		//Vectors are recognized by upper case character 
		if (isupper(buff2[0]))
		{
			for (int i = 1; i < 4; i++)
			{
				ftabout.width(strlen(buff2));
				ftabout << buff2; ftabout.width(15 - strlen(buff2)); ftabout << i;
				k++;
				if (k > 7) { k = 0; ftabout << '\n'; }
			}
		}
		else
		{
			ftabout.width(15);
			ftabout << buff2;
			k++;
			if (k > 7) { k = 0; ftabout << '\n'; }
		}
	}
	ftabout << "\n\n";
}
///////////////////////////////////////////////////////////////////////////////
//Reading input data from input file 'input.asc' for each cruise vehicle object
//Assigning initial values to module-variables of 'round3' and 'cruise' arrays
//Reading aero and propulsion decks
//
//The first cruise object 'input.asc' reads until the first 'END' after
// 'CRUISE3'. The second cruise object reads untile the second 'END', etc until
// the data for all cruise objects are read
//
//Output:	cruise3_name ('Cruise' data member)
//			round3[] data values (Round3 data member)
//			cruise[] data values ('Cruise' data member)
//			event_ptr_list[] ('Event' data members)
//
//Limitation: only real and integer variables can be read 			 
//
//001230 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
//030727 New table look-up scheme, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::vehicle_data(fstream& input)
{
	char line_clear[CHARL];
	char read[CHARN];	//name of variable read from input.asc
	const char* buff = NULL;			//name of module-variable
	double data(0);		//data of module-variable 
	char file_name[CHARN];	//name of data-deck file
	const char* integer = NULL;
	int int_data(0);
	char buff2[CHARN];
	int file_ptr = NULL;
	char name[CHARN];
	Variable* variable = NULL;
	char oper;
	double value(0);
	int e(0);
	const char* watchpoint = NULL;
	int i(0);

	input.getline(cruise3_name, CHARL, '\n');
	//reading data until 'END' is encountered
	do
	{
		//reading variable data into module-variable arrays after discarding comments
		input >> read;
		if (ispunct(read[0]))
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
						//loading integer value
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
			for (auto& item : cruise)
			{
				buff = item.get_name();
				if (!strcmp(buff, read))
				{
					input >> data;
					//checking for integers
					integer = item.get_type();
					if (!strcmp(integer, "int"))
					{
						//loading interger value
						int_data = (int)data;
						item.gets(int_data);
						input.getline(line_clear, CHARL, '\n');
					}
					else
					{
						//loading real value
						item.gets(data);
						input.getline(line_clear, CHARL, '\n');
					}
				}
			}
			//reading aero data from aero-deck file
			if (!strcmp(read, "AERO_DECK")) {
				//reading aerodeck file name
				input >> file_name;
				input.getline(line_clear, CHARL, '\n');

				read_tables(file_name, aerotable);
			}

			//reading prop data from prop-deck file
			if (!strcmp(read, "PROP_DECK")) {
				//reading propdeck file name
				input >> file_name;
				input.getline(line_clear, CHARL, '\n');

				read_tables(file_name, proptable);
			}

			//reading events into 'Event' pointer array 'event_ptr_list' of size NEVENT
			if (!strcmp(read, "IF"))
			{
				event_total++;
				//reading name of watch variable
				input >> name;
				//determining it's module-variable
				for (int m = 0; m < NROUND3; m++)
				{
					buff = round3[m].get_name();
					if (!strcmp(buff, name))
						variable = &round3[m];
				}
				for (auto& item : cruise)
				{
					buff = item.get_name();
					if (!strcmp(buff, name))
						variable = &item;
				}

				//reading other criteria
				input >> oper;
				input >> value;
				event_ptr_list[e]->set_variable(variable);
				event_ptr_list[e]->set_value(value);
				event_ptr_list[e]->set_operator(oper);
				input.getline(line_clear, CHARL, '\n');

				//acquiring indices and values of event variables
				int el1 = 0; //element # in round3[]
				int el2 = 0; //element # in cruise[]
				event_ptr_list[e]->set_round3_size(el1);
				event_ptr_list[e]->set_cruise_size(el2);
				do
				{
					input >> buff2;
					if (ispunct(read[0]))
					{
						input.getline(line_clear, CHARL, '\n');
					}
					else
					{
						for (int k = 0; k < NROUND3; k++)
						{
							buff = round3[k].get_name();
							if (!strcmp(buff, buff2))
							{
								event_ptr_list[e]->set_round3_index(el1, k);
								input >> data;
								event_ptr_list[e]->set_round3_value(el1, data);
								input.getline(line_clear, CHARL, '\n');
								el1++;
								event_ptr_list[e]->set_round3_size(el1);
								if (el1 == NVAR)
								{
									cerr << "*** Error: Check EVENTS (size of NVAR) *** \n";
									exit(1);
								}
							}
						}
						for (int k = 0; k < NCRUISE; k++)
						{
							buff = cruise[k].get_name();
							if (!strcmp(buff, buff2))
							{
								event_ptr_list[e]->set_cruise_index(el2, k);
								input >> data;
								event_ptr_list[e]->set_cruise_value(el2, data);
								input.getline(line_clear, CHARL, '\n');
								el2++;
								event_ptr_list[e]->set_cruise_size(el2);
								if (el2 == NVAR)
								{
									cerr << "*** Error: Check EVENTS (size of NVAR) *** \n";
									exit(1);
								}
							}
						}
					}
				} while (strcmp(buff2, "ENDIF"));
				//increment event counter
				e++;
			} //end of loading events

		} //end of reading non-comment lines
	} while (strcmp(read, "END")); //reached 'END' of vehicle object

	//flushing the line after END and starting new line
	input.getline(line_clear, CHARL, '\n');

	//diagnostic: file pointer
	file_ptr = int(input.tellg());
}
///////////////////////////////////////////////////////////////////////////////
//Building index array of those 'round3[]' and cruise[] variables 
// that are output to screen  
//
//Output: Cruise::round3_scrn_ind[], cruise_crn_ind[] 
//
//001213 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::scrn_index_arrays()
{
	const char* test = "scrn";
	int k = 0;
	int i(0);

	for (i = 0; i < NROUND3; i++)
	{
		if (strstr(round3[i].get_out(), test))
		{
			round3_scrn_ind[k] = i;
			k++;
		}
	}
	int l = 0;
	for (i = 0; i < NCRUISE; i++)
	{
		if (strstr(cruise[i].get_out(), test))
		{
			cruise_scrn_ind[l] = i;
			l++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Writing data to screen
//
//First label is time-since-launch-of-vehicle 'time', always at round3[0]
//Accomodates real, integers (printed as real) and 3x1 vectors 
//Eight accross, unlimited down
//Data field 15 spaces, total width 120 spaces
//
//010112 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::scrn_data()
{

	int index;
	const char* integer;
	const char* vector;
	Matrix VEC(3, 1);
	int k(0);
	int i(0);

	cout << cruise3_name << '\n';
	cout.setf(ios::left);

	//writing to screen the variables from the 'Round3' class
	for (i = 0; i < round3_scrn_count; i++)
	{
		index = round3_scrn_ind[i];
		//checking for integers
		integer = round3[index].get_type();
		vector = round3[index].get_name();
		if (!strcmp(integer, "int"))
		{
			cout.width(15);
			cout << round3[index].integer();
			k++; if (k > 7) { k = 0; cout << '\n'; }
		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = round3[index].vec();

			cout.width(15);
			cout << VEC.get_loc(0, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
			cout.width(15);
			cout << VEC.get_loc(1, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
			cout.width(15);
			cout << VEC.get_loc(2, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
		}
		else
		{
			//real variables
			cout.width(15);
			cout << round3[index].real();
			k++; if (k > 7) { k = 0; cout << '\n'; }
		}
	}
	//writing to screen the variables from the 'Cruise' class
	for (i = 0; i < cruise_scrn_count; i++)
	{
		index = cruise_scrn_ind[i];
		//checking for integers
		integer = cruise[index].get_type();
		vector = cruise[index].get_name();
		if (!strcmp(integer, "int"))
		{
			cout.width(15);
			cout << cruise[index].integer();
			k++; if (k > 7) { k = 0; cout << '\n'; }

		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = cruise[index].vec();
			cout.width(15);
			cout << VEC.get_loc(0, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
			cout.width(15);
			cout << VEC.get_loc(1, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
			cout.width(15);
			cout << VEC.get_loc(2, 0);
			k++; if (k > 7) { k = 0; cout << '\n'; }
		}
		else
		{
			//real variables
			cout.width(15);
			cout << cruise[index].real();
			k++; if (k > 7) { k = 0; cout << '\n'; }
		}
	}
	cout << "\n";
}
///////////////////////////////////////////////////////////////////////////////
//Writing data to 'tabout.asc'
//
//First label is time-since-launch-of-vehicle 'time', must be at round3[0]
// Accomodates real, integers (printed as real) and 3x1 vectors 
// Eight accross, unlimited down
// Data field 15 spaces, total width 120 spaces
//
//010114 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::tabout_data(ofstream& ftabout)
{

	int index;
	const char* integer;
	const char* vector;
	Matrix VEC(3, 1);
	int k(0);
	int i(0);

	ftabout << cruise3_name << '\n';
	ftabout.setf(ios::left);

	//writing to 'tabout.asc' the variables from the 'Round3' class
	for (i = 0; i < round3_scrn_count; i++)
	{
		index = round3_scrn_ind[i];
		//checking for integers
		integer = round3[index].get_type();
		vector = round3[index].get_name();
		if (!strcmp(integer, "int"))
		{
			ftabout.width(15);
			ftabout << round3[index].integer();
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = round3[index].vec();

			ftabout.width(15);
			ftabout << VEC.get_loc(0, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
			ftabout.width(15);
			ftabout << VEC.get_loc(1, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
			ftabout.width(15);
			ftabout << VEC.get_loc(2, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
		}
		else
		{
			//real variables
			ftabout.width(15);
			ftabout << round3[index].real();
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
		}
	}
	//writing to 'tabout.asc' the variables from the 'Cruise' class
	for (i = 0; i < cruise_scrn_count; i++)
	{
		index = cruise_scrn_ind[i];
		//checking for integers
		integer = cruise[index].get_type();
		vector = cruise[index].get_name();
		if (!strcmp(integer, "int"))
		{
			ftabout.width(15);
			ftabout << cruise[index].integer();
			k++; if (k > 7) { k = 0; ftabout << '\n'; }

		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = cruise[index].vec();
			ftabout.width(15);
			ftabout << VEC.get_loc(0, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
			ftabout.width(15);
			ftabout << VEC.get_loc(1, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
			ftabout.width(15);
			ftabout << VEC.get_loc(2, 0);
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
		}
		else
		{
			//real variables
			ftabout.width(15);
			ftabout << cruise[index].real();
			k++; if (k > 7) { k = 0; ftabout << '\n'; }
		}
	}
	ftabout << "\n";
}
///////////////////////////////////////////////////////////////////////////////
//Writing out banner of labels to 'ploti.asc', i=1,2,3...
//
//First label is time-since-launch-of-vehicle 'time', always at round3[0]
//Five accross, unlimited down
//Data field width 16 spaces, total width 80 spaces
//Labels longer than 8 characters will be truncated
//Accomodates 3x1 vectors
//
//010115 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::plot_banner(ofstream& fplot, const char* title)
{
	const char* buff1;
	char buff2[15];
	int label_length = 13;
	int k(0);
	int m(0);
	int i(0);

	fplot << "1" << title << " '" << cruise3_name << " ' " << __DATE__ << " " << __TIME__ << "\n";

	//determining the number vector variables
	for (i = 0; i < nplot_cruise3; i++)
	{
		buff1 = plot_cruise3[i].get_name();
		if (isupper(buff1[0])) m++;
	}
	//increase number of variables by vector components
	int nvariables = nplot_cruise3 + 2 * m;

	fplot << "  0  0 " << nvariables << "\n";

	//writing banner to plot file 'ploti.asc'
	for (i = 0; i < nplot_cruise3; i++)
	{
		fplot.setf(ios::left);

		buff1 = plot_cruise3[i].get_name();

		//truncating if more than 8 characters
		strncpy(buff2, buff1, label_length);
		buff2[13] = 0;

		//Vectors are recognized by upper case character 
		if (isupper(buff2[0]))
		{
			for (int i = 1; i < 4; i++)
			{
				fplot.width(strlen(buff2));
				fplot << buff2; fplot.width(16 - strlen(buff2)); fplot << i;
				k++;
				if (k > 4) { k = 0; fplot << '\n'; }
			}
		}
		else
		{
			fplot.width(16);
			fplot << buff2;
			k++;
			if (k > 4) { k = 0; fplot << '\n'; }
		}
	}
	if ((nvariables % 5))fplot << "\n";
}
///////////////////////////////////////////////////////////////////////////////
//Building index array of those 'round3[]' and cruise[] variables that are  
//output to 'ploti.asc'
//
//Output: Cruise::round3_plot_ind[], cruise_plot_ind[] 
//
//001213 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::plot_index_arrays()
{
	const char* test = "plot";
	int k(0);
	int i(0);

	for (i = 0; i < NROUND3; i++)
	{
		if (strstr(round3[i].get_out(), test))
		{
			round3_plot_ind[k] = i;
			k++;
		}
	}
	int l = 0;
	for (i = 0; i < NCRUISE; i++)
	{
		if (strstr(cruise[i].get_out(), test))
		{
			cruise_plot_ind[l] = i;
			l++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Writing data to 'ploti.asc', i=1,2,3...
//
//Accomodates real, integers (printed as real) and 3x1 vectors 
//five accross, unlimited down
//data field 16 spaces, total width 80 spaces
//
//010116 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::plot_data(ofstream& fplot, bool merge)
{

	int index;
	const char* integer;
	const char* vector;
	Matrix VEC(3, 1);
	int k(0);
	int i(0);

	fplot.setf(ios::left);

	//writing to 'ploti.asc' the variables from the 'Round3' class
	for (i = 0; i < round3_plot_count; i++)
	{
		index = round3_plot_ind[i];
		//checking for integers
		integer = round3[index].get_type();
		vector = round3[index].get_name();
		if (!strcmp(integer, "int"))
		{
			//casting integer to real variable
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << (double)round3[index].integer();
			k++;
		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = round3[index].vec();

			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(0, 0);
			k++;
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(1, 0);
			k++;
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(2, 0);
			k++;
		}
		else
		{
			//real variables
			if (merge && (!index))
				//for merging files, time at last entry must be '-1'
			{
				fplot.width(16);
				fplot << "-1.0";
				k++;
			}
			else
			{
				if (k > 4) { k = 0; fplot << '\n'; }
				fplot.width(16);
				fplot << round3[index].real();
				k++;
			}
		}
	}
	//writing to 'ploti.asc' the variables from the 'Cruise' class
	for (i = 0; i < cruise_plot_count; i++)
	{
		index = cruise_plot_ind[i];
		//checking for integers
		integer = cruise[index].get_type();
		vector = cruise[index].get_name();
		if (!strcmp(integer, "int"))
		{
			//casting integer to real variable
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << (double)cruise[index].integer();
			k++;
		}
		//checking vor vectors
		else if (isupper(vector[0]))
		{
			VEC = cruise[index].vec();
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(0, 0);
			k++;
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(1, 0);
			k++;
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << VEC.get_loc(2, 0);
			k++;
		}
		else
		{
			//real variables
			if (k > 4) { k = 0; fplot << '\n'; }
			fplot.width(16);
			fplot << cruise[index].real();
			k++;
		}
	}
	fplot << "\n";
}
///////////////////////////////////////////////////////////////////////////////
//Watching for and executing events
// 
//Max number of events set by global constant NEVENT
//Max number of variables in any event set by global constant NVAR
//Each event has to be surrounded in 'input.asc' by 'IF' and 'ENDIF'
//Event criteria and variables can be 'double' or 'int'
//New variable-values are subsituted before calling modules
//
//010125 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::event(const char* options)
{
	Variable* watch_variable_ptr;
	double current_value = 0;
	int current_value_int = 0;
	double crit_value = 0;
	int crit_value_int = 0;
	char oper;
	const char* intest = NULL;

	//initializing event flag ('Cruise' member)
	event_epoch = false;

	//returning if no events occur for this vehicle object
	//or if all events have already occurred
	if (!event_total)return;

	//getting watch variable's current value and critical value
	watch_variable_ptr = event_ptr_list[nevent]->get_variable();
	crit_value = event_ptr_list[nevent]->get_value();

	//testing for integer
	intest = watch_variable_ptr->get_type();
	if (!strcmp(intest, "int"))
	{
		current_value_int = watch_variable_ptr->integer();
		crit_value_int = (int)crit_value;
	}
	else
		current_value = watch_variable_ptr->real();
	//getting relational operator
	oper = event_ptr_list[nevent]->get_operator();

	//checking if event occurred
	if (oper == '<')
	{
		if (!strcmp(intest, "int"))
		{
			if (current_value_int < crit_value_int)
				event_epoch = true;
		}
		else
		{
			if (current_value < crit_value)
				event_epoch = true;
		}
	}
	else if (oper == '=')
	{
		if (!strcmp(intest, "int"))
		{
			if (current_value_int == crit_value_int)
				event_epoch = true;
		}
		else
		{
			if (current_value == crit_value)event_epoch = true;
		}
	}
	else if (oper == '>')
	{
		if (!strcmp(intest, "int"))
		{
			if (current_value_int > crit_value_int)
				event_epoch = true;
		}
		else
		{
			if (current_value > crit_value)
				event_epoch = true;
		}
	}

	//loading new variables
	if (event_epoch)
	{
		int* round3_index_list;
		double* round3_value_list;
		int round3_size(0);
		int* cruise_index_list;
		double* cruise_value_list;
		int cruise_size(0);
		int index(0);
		const char* integer;
		double value(0);
		int value_int(0);
		int i(0);

		round3_index_list = event_ptr_list[nevent]->get_round3_indices();
		round3_value_list = event_ptr_list[nevent]->get_round3_values();
		round3_size = event_ptr_list[nevent]->get_round3_size();

		cruise_index_list = event_ptr_list[nevent]->get_cruise_indices();
		cruise_value_list = event_ptr_list[nevent]->get_cruise_values();
		cruise_size = event_ptr_list[nevent]->get_cruise_size();

		for (i = 0; i < round3_size; i++)
		{
			index = round3_index_list[i];
			value = round3_value_list[i];

			integer = round3[index].get_type();
			if (!strcmp(integer, "int"))
			{
				value_int = (int)value;
				round3[index].gets(value_int);
			}
			round3[index].gets(value); // new value assigned
		}
		for (i = 0; i < cruise_size; i++)
		{
			index = cruise_index_list[i];
			value = cruise_value_list[i];

			integer = cruise[index].get_type();
			if (!strcmp(integer, "int"))
			{
				value_int = (int)value;
				cruise[index].gets(value_int);
			}
			cruise[index].gets(value); // new value assigned
		}

		//writing event message to console
		double time = round3[0].real();
		const char* name = watch_variable_ptr->get_name();
		if (strstr(options, "y_events"))
		{
			cout << " *** Event #" << nevent + 1 << '\t' << cruise3_name << '\t' << "time = " << time
				<< "\tsec;  criteria:  " << name << " " << oper << " " << crit_value << "\t***\n";
		}
		//increment event number
		nevent++;
		//reset 'event_total' to zero after last event has occured
		if (nevent == event_total)event_total = 0;
	}
}
///////////////////////////////////////////////////////////////////////////////
//Composing documention on file 'doc.asc'
//Listing 'cruise' and 'round3' module-variable arrays
//
//010126 Created by Peter Zipfel
//020911 Added module-variable error flagging, PZi
//030627 Adapted to CRUISE simulation, PZi
//060929 Modification to accomodate C++8, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::document(ostream& fdoc, const char* title, Document* doc_cruise3)
{
	int i(0);
	int j(0);

	fdoc << "*********************************************************************************************************************\n";
	fdoc << "********************************************** CRUISE3 **************************************************************\n";
	fdoc << "*********************************************************************************************************************\n";
	fdoc << "\n" << "*** " << title << "   " << __DATE__ << " " << __TIME__ << " ***\n\n";
	//	fdoc<<"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456\n";
	fdoc << "\n\n                                       Cruise Module-Variable Array \n\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";
	fdoc << "|LOC|        NAME       |                    DEFINITION                       |   MODULE   | PURPOSE |    OUTPUT    |\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";

	char name_error_code[] = "A";
	for (i = 0; i < NCRUISE; i++)
	{
		for (j = 0; j < i; j++) {
			if (!strcmp(cruise[i].get_name(), cruise[j].get_name()) && strcmp(cruise[i].get_name(), "empty"))
				cruise[i].put_error(name_error_code);
		}
		if (!strcmp(cruise[i].get_error(), "A")) cout << " *** Error code 'A': duplicate name in cruise[] array, see 'doc.asc' ***\n";
		if (!strcmp(cruise[i].get_error(), "*")) cout << " *** Error code '*': duplicate location in cruise[] array, see 'doc.asc' ***\n";

		fdoc << cruise[i].get_error();
		fdoc.setf(ios::left);
		fdoc.width(4); fdoc << i;
		if (!strcmp(cruise[i].get_type(), "int"))
		{
			fdoc.width(15); fdoc << cruise[i].get_name();
			fdoc.width(5); fdoc << " int ";
		}
		else
		{
			fdoc.width(20); fdoc << cruise[i].get_name();
		}
		fdoc.width(54); fdoc << cruise[i].get_def();
		fdoc.width(13); fdoc << cruise[i].get_mod();
		fdoc.width(10); fdoc << cruise[i].get_role();
		fdoc << cruise[i].get_out();
		fdoc << "\n";
		if (!((i + 1) % 10))fdoc << "----------------------------------------------------------------------------------------------------------------------\n";
	}

	fdoc << "\n\n                                       Round3 Module-Variable Array \n\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";
	fdoc << "|LOC|        NAME       |                   DEFINITION                        |   MODULE   | PURPOSE |    OUTPUT    |\n";
	fdoc << "---------------------------------------------------------------------------------------------------------------------\n";

	for (i = 0; i < NROUND3; i++)
	{
		for (j = 0; j < i; j++) {
			if (!strcmp(round3[i].get_name(), round3[j].get_name()) && strcmp(round3[i].get_name(), "empty"))
				round3[i].put_error(name_error_code);
		}
		if (!strcmp(round3[i].get_error(), "A")) cout << " *** Error code 'A': duplicate name in round3[] array, see 'doc.asc' ***\n";
		if (!strcmp(round3[i].get_error(), "*")) cout << " *** Error code '*': duplicate location in round3[] array, see 'doc.asc' ***\n";

		fdoc << round3[i].get_error();
		fdoc.setf(ios::left);
		fdoc.width(4); fdoc << i;
		if (!strcmp(round3[i].get_type(), "int"))
		{
			fdoc.width(15); fdoc << round3[i].get_name();
			fdoc.width(5); fdoc << " int ";
		}
		else
		{
			fdoc.width(20); fdoc << round3[i].get_name();
		}
		fdoc.width(54); fdoc << round3[i].get_def();
		fdoc.width(13); fdoc << round3[i].get_mod();
		fdoc.width(10); fdoc << round3[i].get_role();
		fdoc << round3[i].get_out();
		fdoc << "\n";
		if (!((i + 1) % 10))fdoc << "----------------------------------------------------------------------------------------------------------------------\n";
	}

	//building doc_cruise3[] for documenting 'input.asc' and eliminating 'empty' slots
	int counter = 0;
	for (i = 0; i < NCRUISE; i++) {
		if (strcmp(cruise[i].get_name(), "empty")) {
			doc_cruise3[counter].put_doc_offset(counter);
			doc_cruise3[counter].put_name(cruise[i].get_name());

			//z060929		doc_cruise3[counter].put_type(cruise[i].get_type());

						//Previous line breakes in MSC++8 debugger because 'put_type()' function
						// was updated to 'strcpy()' from 'strcpy()
						// Solution: when "int" is not present (because not an integer module-variable),
						//   'dum' keeps the original I-accent characters, which are equivalent to '-51'; 
						//   therefore, do not execute 'put_type()' if module-variable is not an integer
			const char* dum;
			dum = cruise[i].get_type();
			if (*dum != -51)
				doc_cruise3[counter].put_type(dum);
			//z060929-end

			doc_cruise3[counter].put_def(cruise[i].get_def());
			doc_cruise3[counter].put_mod(cruise[i].get_mod());
			counter++;
		}
	}
	for (i = 0; i < NROUND3; i++) {
		if (strcmp(round3[i].get_name(), "empty")) {
			doc_cruise3[counter].put_doc_offset(counter);
			doc_cruise3[counter].put_name(round3[i].get_name());

			//z060929		doc_cruise3[counter].put_type(round3[i].get_type());

			const char* dum;
			dum = round3[i].get_type();
			if (*dum != -51)
				doc_cruise3[counter].put_type(dum);
			//z060929-end

			doc_cruise3[counter].put_def(round3[i].get_def());
			doc_cruise3[counter].put_mod(round3[i].get_mod());
			counter++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Building index array of those 'round3[]' and cruise[] variables 
// that are output to 'combus' 'data'  
//
//Output: Cruise::round3_com_ind[], cruise_com_ind[] 
//
//010210 Created by Peter Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

void Cruise::com_index_arrays()
{
	const char* test = "com";
	int k = 0;
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
	for (i = 0; i < NCRUISE; i++)
	{
		if (strstr(cruise[i].get_out(), test))
		{
			cruise_com_ind[l] = i;
			l++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Initializing loading 'packet' with 'CRUISE3' data
//
//Uses C-code 'sprintf' function to convert 'int' to 'char'
//Differs from 'loading_packet' only by initializing 'status=1'
//
//Output by 'return packet'
//
//010401 Created by Peter H Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

Packet Cruise::loading_packet_init(int num_cruise, int num_target, int num_satellite)
{
	string id;
	char object[4];
	static int c_count(0);
	int index(0);
	int i(0);

	c_count++;
	if (c_count == (num_cruise + 1))c_count = 1;
	sprintf(object, "%i", c_count);
	id = "c" + string(object);

	//building 'data' array of module-variables
	for (i = 0; i < round3_com_count; i++)
	{
		index = round3_com_ind[i];
		com_cruise3[i] = round3[index];
	}
	for (int j = 0; j < cruise_com_count; j++)
	{
		index = cruise_com_ind[j];
		com_cruise3[i + j] = cruise[index];
	}
	//refreshing the packet
	packet.set_id(id);
	packet.set_status(1);
	packet.set_data(com_cruise3);
	packet.set_ndata(ncom_cruise3);

	return packet;
}
///////////////////////////////////////////////////////////////////////////////
//Loading 'packet' with 'CRUISE3' data
//
//Uses C-code 'sprintf' function to convert 'int' to 'char'
//
//010206 Created by Peter H Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

Packet Cruise::loading_packet(int num_cruise, int num_target, int num_satellite)
{
	int index(0);
	int i(0);
	int j(0);

	/*	string id;
		char object[4];
		static int c_count=0;

		c_count++;
		if(c_count==(num_cruise+1))c_count=1;
		sprintf(object,"%i",c_count);
		id="c"+string(object);
	*/
	//building 'data' array of module-variables
	for (i = 0; i < round3_com_count; i++)
	{
		index = round3_com_ind[i];
		com_cruise3[i] = round3[index];
	}
	for (j = 0; j < cruise_com_count; j++)
	{
		index = cruise_com_ind[j];
		com_cruise3[i + j] = cruise[index];
	}
	//refreshing the packet
//	packet.set_id(id);
	packet.set_data(com_cruise3);
	packet.set_ndata(ncom_cruise3);

	return packet;
}
///////////////////////////////////////////////////////////////////////////////
//Reading tables from table decks
//
//Supports 1, 2, 3 dim tables stored seperately in data decks
//Keying on AERO_DECK and PROP_DECK in 'input.asc' this function  reads the tables
// from the data files and stores them in 'Cruise::Datadeck aerotable' and 
// 'Cruise::Datadeck proptable'. In the modules, table look up is carried out by 
// double value=aerodeck.look_up(string name, double var1);  
// double value=aerodeck.look_up(string name, double var1,double var2);  
// double value=aerodeck.look_up(string name, double var1,double var2,double var3);  
//
//To add new tables, just include them in the files of  AERO_DECK and PROP_DECK
//For debugging puposes un-comment the print out provision of the tables below
//
//030721 Created by Peter H Zipfel
//060426 Corrected line 1401 (replaced 'if else' by 'if'), PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::read_tables(const char* file_name, Datadeck& datatable)
{

	char line_clear[CHARL];
	char temp[CHARN];	//buffer for table data
	string table_deck_title;
	int table_count(0);
	int table_dim(0);
	double value(0);
	int file_ptr = NULL;
	int var_dim[3] = { 1,1,1, };
	int tt(0);

	//opening data-deck file stream
	ifstream tbl_stream(file_name);

	if (tbl_stream.fail())
	{
		cerr << "*** Error: File stream '" << file_name << "' failed to open (check spelling) ***\n"; exit(1);
	}

	//determing the total # of tbl_stream
	while (!tbl_stream.eof())
	{
		tbl_stream >> temp;
		if (!strcmp(temp, "TITLE")) {
			tbl_stream.getline(line_clear, CHARL, '\n');
			table_deck_title = line_clear;
		}
		if (strstr(temp, "DIM"))
			table_count++;

	} //EOF reached of data-deck

	//removing EOF bit (-1)
	tbl_stream.clear();
	//rewinding to beginning
	tbl_stream.seekg(ios::beg);

	//creating pointer array table 
	datatable.set_title(table_deck_title);
	datatable.set_capacity(table_count);
	datatable.alloc_mem();

	//discarding all entries until first DIM
	do {
		tbl_stream >> temp;
	} while (!strstr(temp, "DIM"));

	//loading tables one at a time
	for (int t = 0; t < table_count; t++) {

		//creating and allocating memory to object 'table'
		try { table = new Table; }
		catch (bad_alloc xa) { cerr << "*** Allocation failure of 'table' ***\n"; exit(1); }

		//extracting table dimension
		//at this point 'temp' is holding xDIM
		int dim_check(0);
		char dim_buff[2];
		int table_dim(0);
		strncpy(dim_buff, temp, 1);
		//converting character to integer
		dim_check = sscanf(dim_buff, "%d", &table_dim);
		table->set_dim(table_dim);

		//extracting table name
		tbl_stream >> temp;
		table->set_name(temp);
		tbl_stream.getline(line_clear, CHARL, '\n');

		//extracting dimensions of independent variables
		var_dim[0] = 1; var_dim[1] = 1; var_dim[2] = 1;
		for (tt = 0; tt < table_dim; tt++) {
			tbl_stream >> temp;
			tbl_stream >> var_dim[tt];
			if (tt == 0)
				table->set_var1_dim(var_dim[tt]);
			if (tt == 1)
				table->set_var2_dim(var_dim[tt]);
			if (tt == 2)
				table->set_var3_dim(var_dim[tt]);
		}
		tbl_stream.getline(line_clear, CHARL, '\n');

		//allocating memory for variables and data arrays
		table->var1_values = new double[var_dim[0]];
		table->var2_values = new double[var_dim[1]];
		table->var3_values = new double[var_dim[2]];
		table->data = new double[var_dim[0] * var_dim[1] * var_dim[2]];

		//determining max number of rows of data
		int num_rows = var_dim[0];
		if (var_dim[0] < var_dim[1]) num_rows = var_dim[1];
		if (var_dim[2] > num_rows) num_rows = var_dim[2];

		//reading num_row of data 
		for (tt = 0; tt < num_rows; tt++) {

			//loading 1.variable values
			if (tt < var_dim[0]) {
				tbl_stream >> value;
				table->set_var1_value(tt, value);
			}

			//loading 2.variable values, but bypass if default dimension one
			if (tt < var_dim[1] && var_dim[1] != 1) {
				tbl_stream >> value;
				table->set_var2_value(tt, value);
			}

			//loading 3.variable values, but bypass if default dimension one
			if (tt < var_dim[2] && var_dim[2] != 1) {
				tbl_stream >> value;
				table->set_var3_value(tt, value);
			}

			//loading tabular data, which in all cases has only 'var_dim[0]' rows
			if (tt < var_dim[0]) {

				//read one row of data
				for (int ttt = 0; ttt < var_dim[1] * var_dim[2]; ttt++) {
					tbl_stream >> value;
					table->set_data(tt * var_dim[1] * var_dim[2] + ttt, value);
				}
			}
		}//end of reading data

		//loading table into 'Datadeck' pointer array 'Table **table_ptr'
		datatable.set_counter(t);
		datatable.add_table(*table);
		tbl_stream >> temp; //reading next DIM entry

	}//end of 'for' loop, finished loading all tables

	/////////////////////// DIAGNOSTICS //////////////////////////////////////
	if constexpr (enableDiagostics)
	{
		//Diagnostic display of tables on console
		int num_tables = datatable.get_capacity();
		string deck_title = datatable.get_title();
		cout << "\n ********************* Data Deck *********************\n";
		cout << "TITLE " << deck_title << "\n\n";

		for (int ti = 0; ti < num_tables; ti++) {
			int tbl_dim = datatable[ti]->get_dim();

			if (tbl_dim == 1) {
				string tbl_name = datatable[ti]->get_name();
				cout << tbl_dim << "DIM " << tbl_name << '\n';
				int var1_dim = datatable[ti]->get_var1_dim();
				cout << "NX1 " << var1_dim << '\n';
				for (int tii = 0; tii < var1_dim; tii++) {
					cout << datatable[ti]->var1_values[tii] << "\t" << datatable[ti]->data[tii] << endl;
				}
				cout << '\n';
			}//end of 1-dim tables

			if (tbl_dim == 2) {
				string tbl_name = datatable[ti]->get_name();
				cout << tbl_dim << "DIM " << tbl_name << '\n';
				int var1_dim = datatable[ti]->get_var1_dim();
				int var2_dim = datatable[ti]->get_var2_dim();
				cout << "NX1 " << var1_dim << "   NX2 " << var2_dim << '\n';

				//find maximum number of rows
				int num_rows = var1_dim;
				if (num_rows < var2_dim)
					num_rows = var2_dim;

				for (int tii = 0; tii < num_rows; tii++) {
					if (tii < var1_dim)
						cout << datatable[ti]->var1_values[tii] << "\t";
					else
						cout << "*\t";
					if (tii < var2_dim)
						cout << datatable[ti]->var2_values[tii] << "\t";
					else
						cout << "*\t";

					if (tii < var1_dim) {
						for (int tk = 0; tk < var2_dim; tk++)
							cout << datatable[ti]->data[tk + tii * var2_dim] << '\t';
						cout << endl;
					}
					else {
						for (int tk = 0; tk < var2_dim; tk++)
							cout << "*\t";
						cout << endl;
					}

				}
				cout << '\n';
			}//end of 2-dim tables

			if (tbl_dim == 3) {
				string tbl_name = datatable[ti]->get_name();
				cout << tbl_dim << "DIM " << tbl_name << '\n';
				int var1_dim = datatable[ti]->get_var1_dim();
				int var2_dim = datatable[ti]->get_var2_dim();
				int var3_dim = datatable[ti]->get_var3_dim();
				cout << "NX1 " << var1_dim << "   NX2 " << var2_dim << "   NX3 " << var3_dim << '\n';

				//find maximum number of rows
				int num_rows = var1_dim;
				if (num_rows < var2_dim)
					num_rows = var2_dim;
				if (num_rows < var3_dim)
					num_rows = var3_dim;

				for (int tii = 0; tii < num_rows; tii++) {
					if (tii < var1_dim)
						cout << datatable[ti]->var1_values[tii] << "\t";
					else
						cout << "*\t";
					if (tii < var2_dim)
						cout << datatable[ti]->var2_values[tii] << "\t";
					else
						cout << "*\t";
					if (tii < var3_dim)
						cout << datatable[ti]->var3_values[tii] << "\t";
					else
						cout << "*\t";

					if (tii < var1_dim) {
						for (int tk = 0; tk < var2_dim * var3_dim; tk++)
							cout << datatable[ti]->data[tk + tii * var2_dim * var3_dim] << '\t';
						cout << endl;
					}
					else {
						for (int tk = 0; tk < var2_dim * var3_dim; tk++)
							cout << "*\t";
						cout << endl;
					}
				}
				cout << '\n';
			}//end of 3-dim tables
		}//end of diagnostic table print-out
	}
	//////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
//Definition of aerodynamic module-variables 
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[30-39]
// 
//Defining and initializing module-variables
//
//001226 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_aerodynamics()
{
	//Definition of module-variables
	cruise[30].init("cl", 0, "Lift coefficient - ND", "aerodynamics", "out", "");
	cruise[31].init("cd", 0, "Drag coefficient - ND", "aerodynamics", "out", "");
	cruise[32].init("cl_ov_cd", 0, "Lift-over-drag ratio - ND", "aerodynamics", "diag", "scrn,plot");
	cruise[33].init("area", 0.929, "Aerodynamic reference area - m^2", "aerodynamics", "data", "");
	cruise[34].init("cla", 0, "Lift coefficient slope - 1/deg", "aerodynamics", "out", "");
}
//$$$//////////////////////////////////////////////////////////////////////////
//Aerodynamic module
//Member function of class 'Cruise'
// Generates drag polar of cruise cruise missile 'Cruise3'
// Area = 0.929 m^2, drag polars for c.g at STA 134 in 
//
//001023 Created by Peter Zipfel
//001227 Upgraded to module-variable arrays, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::aerodynamics()
{
	//local variables
	double cd0(0);
	double cl0(0);
	double ckk(0);
	double cla0(0);

	//local module-variables
	double cl(0);
	double cd(0);
	double cl_ov_cd(0);
	double cla(0);

	//localizing module-variables
	//input from other modules
	double mach = round3[14].real();
	double alphax = cruise[51].real();
	//-------------------------------------------------------------------------
	cd0 = aerotable.look_up("cd0_vs_mach", mach);
	cl0 = aerotable.look_up("cl0_vs_mach", mach);
	cla = aerotable.look_up("cla_vs_mach", mach);
	ckk = aerotable.look_up("ckk_vs_mach", mach);
	cla0 = aerotable.look_up("cla0_vs_mach", mach);
	cl = cla0 + cla * (alphax);
	cd = cd0 + ckk * pow((cl - cl0), 2);
	cl_ov_cd = cl / cd;
	//-------------------------------------------------------------------------
	//loading module-variables
	//output to other modules
	cruise[30].gets(cl);
	cruise[31].gets(cd);
	cruise[34].gets(cla);
	//diagnostics
	cruise[32].gets(cl_ov_cd);
}

///////////////////////////////////////////////////////////////////////////////
//Definition of propulsion module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[10-29]
//
// Initializing the module-variables
// Initializing the state variable derivatives to zero 
//
//State derivatives: 
//		treqd = Derivative of thrust req'd - N/s
//		fmassed = Derivative of fuel mass - kg/s
//		
//001103 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

void Cruise::def_propulsion()
{
	//Definition of module-variables
	cruise[10].init("mprop", "int", 10, "Mode switch - ND", "propulsion", "data/diag", "scrn,plot");
	cruise[11].init("cg", 0, "Vehicle c.g. location - inch BSTA", "propulsion", "diag", "plot");
	cruise[12].init("fidle", 0, "Idle thrust - N", "propulsion", "diag", "");
	cruise[13].init("thrust_com", 0, "Commanded thrust - N", "propulsion", "data", "");
	cruise[14].init("thrust", 0, "Turbojet thrust - N", "propulsion", "out", "scrn,plot,com");
	cruise[15].init("treqd", 0, "Derivative of thrust req'd - N/s", "propulsion", "state", "");
	cruise[16].init("treq", 0, "Thrust req'd - N/s", "propulsion", "state", "");
	cruise[17].init("fmassed", 0, "Derivative of fuel mass expended - kg/s", "propulsion", "state", "");
	cruise[18].init("fmasse", 0, "Fuel mass expended - kg", "propulsion", "state", "");
	cruise[19].init("fuelmass", 0, "Fuel mass in vehicle - kg", "propulsion", "save", "");
	cruise[20].init("mach_com", 0, "Commanded Mach number - ND", "propulsion", "data", "");
	cruise[21].init("gfthm", 0, "Gain of Mach hold loop - N", "propulsion", "data", "");
	cruise[22].init("tfth", 0, "Time constant of Mach hold loop - s", "propulsion", "data", "");
	cruise[23].init("mass", 0, "Vehicle mass - kg", "propulsion", "out", "scrn");
	cruise[24].init("tav", 0, "Thrust available - N", "propulsion", "diag", "");
	cruise[25].init("mass_init", 0, "Initial vehicle mass - kg", "propulsion", "data", "");
	cruise[26].init("fuel_init", 0, "Initial fuel - kg", "propulsion", "data", "");
}

///////////////////////////////////////////////////////////////////////////////
//Initialization of propulsion module
//Member function of class 'Cruise'
// Initializing mass
//		
//001103 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::init_propulsion()
{
	//local module-variable
	double mass(0);

	//localizing module-variables
	//input data
	double mass_init = cruise[25].real();
	//-------------------------------------------------------------------------
	//initializing mass
	mass = mass_init;
	//-------------------------------------------------------------------------
	//loading module variables
	cruise[23].gets(mass);
}

//$$$//////////////////////////////////////////////////////////////////////////
//Propulsion module
//Member function of class Cruise
// Generates thrust of Cruise3 vehicle
//
//High efficiency turbojet
//Gross mass of vehicle (full fuel) 1000 kg
//Max fuel mass 150 kg
//This module performs the following functions:
//
//(1) Provides the thrust available and fuel flow tables
//(2) Provides the idle thrust and fuel flow tables
//(3) Provides the c.g. location as a function of cruise missile mass
//
//mprop=0 No thrusting (input)
//		1 Commanded thrust (input), use thrust_com        
//		2 Idle thrusting (input)
//		3 Max thrusting (input)
//		4 Required thrust determined by Mach hold loop (input)
//		5 Required thrust < idle thrust (diagnostic)
//		6 Required thrust > max thrust (diagnostic)
//		
//001023 Created by Peter Zipfel
//001227 Upgraded to module-variable arrays, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::propulsion(double int_step)
{
	//local variables
	double tcom(0);
	double epsmch(0);
	double treqss(0);
	double treqb(0);
	double treqd_new(0);
	double fmassed_new(0);
	double ff(0);
	double treqs(0);

	//local module-variables
	double cg(0);
	double fidle(0);
	double thrust(0);
	double tav(0);

	//localizing module-variables
	//input data
	int mprop = cruise[10].integer();
	double thrust_com = cruise[13].real();
	double mach_com = cruise[20].real();
	double gfthm = cruise[21].real();
	double tfth = cruise[22].real();
	double mass_init = cruise[25].real();
	double fuel_init = cruise[26].real();
	//input from other modules
	double pdynmc = round3[13].real();
	double mach = round3[14].real();
	double alt = round3[21].real();
	//state variables
	double treqd = cruise[15].real();
	double treq = cruise[16].real();
	double fmassed = cruise[17].real();
	double fmasse = cruise[18].real();
	//restore saved values
	double fuelmass = cruise[19].real();
	//input from other modules
	double mass = cruise[23].real();
	double cd = cruise[31].real();
	double area = cruise[33].real();
	double alphax = cruise[51].real();
	//-------------------------------------------------------------------------	
	//No thrust, just calculate c.g. location
	if (mprop == 0)
	{
		thrust = 0;
		ff = 0;
		//Determine cg location from previous cycle for table entry 
		cg = proptable.look_up("cg_vs_mass", mass);
		cruise[14].gets(thrust);
		return;
	}
	//Look-up idle thrust
	fidle = proptable.look_up("fidle_vs_alt_mach", alt, mach);
	//Look-up max thrust available
	tav = proptable.look_up("tav_vs_alt_mach", alt, mach);

	//Branch to the desired thrusting mode
	//commanded thrust
	if (mprop == 1)
	{
		thrust = thrust_com;
		//Fuel flow from S.L. to 3048 m
		ff = proptable.look_up("ff_vs_thrust_alt_mach", thrust, alt, mach);
		//Initialize state variable 'treq' (thrust required)
		treq = thrust_com;
	}
	//idle thrusting
	else if (mprop == 2)
	{
		thrust = fidle;
		//Idle fuel flow from S.L. to 12192 m
		ff = proptable.look_up("iff_vs_alt", alt);
	}
	//max thrusting
	else if (mprop == 3)
	{
		thrust = tav;
		//Fuel flow from S.L. to 3048 m
		ff = proptable.look_up("ff_vs_thrust_alt_mach", thrust, alt, mach);
	}
	//thrust determined by mach-hold loop
	else if (mprop > 3)
	{
		//Calculate thrust required in stability axis
		mprop = 4;
		treqs = cd * pdynmc * area;
		//Mach hold loop
		epsmch = mach_com - mach;
		tcom = epsmch * gfthm + treqs;
		treqd_new = (tcom - 2. * treq) / tfth;
		treq = integrate(treqd_new, treqd, treq, int_step);
		treqd = treqd_new;
		treqss = treq;
		//Thrust in body axis
		treqb = treqss / cos(alphax * RAD);
		//Thrust limiting
		if (treqb < fidle) { mprop = 5; treqb = fidle; };
		if (treqb > tav) { mprop = 6; treqb = tav; };
		thrust = treqb;
		//Fuel flow from S.L. to 3048 m 
		ff = proptable.look_up("ff_vs_thrust_alt_mach", thrust, alt, mach);
	}
	//Current vehicle mass obtained through integrating over fuel mass expended 
	fmassed_new = ff;
	fmasse = integrate(fmassed_new, fmassed, fmasse, int_step);
	fmassed = fmassed_new;
	mass = mass_init - fmasse;
	fuelmass = fuel_init - fmasse;

	//Current c.g. location
	cg = proptable.look_up("cg_vs_mass", mass);

	//setting thrust to zero when fuel expended
	if (fuelmass <= 0) thrust = 0;
	//-------------------------------------------------------------------------
	//loading module-variables
	//state variables
	cruise[15].gets(treqd);
	cruise[16].gets(treq);
	cruise[17].gets(fmassed);
	cruise[18].gets(fmasse);
	//saving variables
	cruise[19].gets(fuelmass);
	//output to other modules
	cruise[10].gets(mprop);
	cruise[14].gets(thrust);
	cruise[23].gets(mass);
	//diagnostics
	cruise[11].gets(cg);
	cruise[12].gets(fidle);
	cruise[24].gets(tav);
}
///////////////////////////////////////////////////////////////////////////////
//Definition of force module-variables
// Member function of class 'Cruise'
//
//Note that FSPV is entered into the round3[10] array because it is needed
// for the newton module, which is a member of the 'Round3' class
//		
//001129 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_forces()
{
	//Definition of module-variables
	round3[10].init("FSPV", 0, 0, 0, "Specific force in V-coord - m/s^2", "forces", "out", "plot");
}

//$$$//////////////////////////////////////////////////////////////////////////
//Force Module 
//Member function of class 'Cruise' 
//Calulates the total force acting on the vehicle
//
//000623 Created by Michael Chiaramonte
//000724 Function calls have been removed, Michael Horvath
//001227 Upgraded to module-variable arrays, PZi
///////////////////////////////////////////////////////////////////////////////
void Cruise::forces()
{
	//localizing module-variables
	//input from other modules
	double pdynmc = round3[13].real();
	Matrix FSPV = round3[10].vec();
	double cl = cruise[30].real();
	double cd = cruise[31].real();
	double area = cruise[33].real();
	double thrust = cruise[14].real();
	double mass = cruise[23].real();
	double alphax = cruise[51].real();
	double phimvx = cruise[52].real();
	//-------------------------------------------------------------------------
	double phimv = phimvx * RAD;
	double alpha = alphax * RAD;

	double fspv1 = (-pdynmc * area * cd + thrust * cos(alpha)) / mass;
	double fspv2 = sin(phimv) * (pdynmc * area * cl + thrust * sin(alpha)) / mass;
	double fspv3 = -cos(phimv) * (pdynmc * area * cl + thrust * sin(alpha)) / mass;

	FSPV.assign_loc(0, 0, fspv1);
	FSPV.assign_loc(1, 0, fspv2);
	FSPV.assign_loc(2, 0, fspv3);
	//-------------------------------------------------------------------------
	//loading module-variables
	//output to other modules
	round3[10].gets_vec(FSPV);
}
///////////////////////////////////////////////////////////////////////////////
//Definition of 'targeting' module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[130-139]
//		
//010813 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_targeting()
{
	//definition of module-variables
	cruise[130].init("mtargeting", "int", 0, "Mode switch - ND", "targeting", "data", "scrn,plot");
	cruise[131].init("del_radius", 0, "Increase in Earth's radius for 'visibility' - m", "targeting", "data", "");
	cruise[132].init("clost_tgt_slot", "int", 0, "Closest of satellite tracked targets ' - ND", "targeting", "out", "");
	cruise[133].init("tgtng_sat_slot", "int", 0, "Satellite providing targeting ' - ND", "targeting", "out", "");
}
//$$$//////////////////////////////////////////////////////////////////////////  
//Targeting module
//Member function of class 'Cruise'
//
//mtargeting:
//			= 0 no satellite targeting taking place
//			= 1 targeting enabled
//NOTE:
//The 'Target'-object variables 'lonx','latx','alt','dvbe','psivgx','thtvgx', SBII
// must be located in 'combus' at 'Packet data[i]', i=2,3,4,5,6,7,10 respectively.
//These locations are determined by the sequence of the "com"-key entry
// in the module-variable array 'round3[]'
//		
//010813 Created by Peter H Zipfel
//070313 Added output to console, PZi
///////////////////////////////////////////////////////////////////////////////	
void Cruise::targeting(Packet* combus, int vehicle_slot, int num_vehicles, int num_target
	, int num_satellite)
{
	//local variables
	double range(0);
	bool satellite_found = false;

	Variable* data_t;
	static int out_count(0);

	//local module-variables
	int clost_tgt_slot(0);
	int tgtng_sat_slot(0);
	double wp_lonx(0);
	double wp_latx(0);
	double wp_alt(0);

	//localizing module-variables
	//input data
	int mtargeting = cruise[130].integer();
	//-------------------------------------------------------------------------
	//returning if no targeting is taking place
	if (mtargeting == 0) return;

	//determining satellites that can provide targeting data and identify them in 'visibility[]'
	targeting_satellite(combus, num_vehicles);

	//is there a satellite that can provide targeting info? 
	//if there is more than one, the first one in the vehicle slot# sequence is taken
	//j target counter; i vehicle counter, k satellite counter	for(int k=0;k<num_satellite;k++)
	for (int k = 0; k < num_satellite; k++)
	{
		if (!satellite_found)
		{
			int active = visibility[k].tracking;
			if (active)
			{
				satellite_found = true;
				tgtng_sat_slot = visibility[k].vehicle_slot;
				//building 'grnd_range[]' (ranges to all targets)
				targeting_grnd_ranges(combus, num_vehicles);

				//determining closest target
				range = BIG;

				int j = 0;

				for (int i = 0; i < num_vehicles; i++)
				{
					string id = combus[i].get_id();
					int loc = (int)id.find("t");
					if (!loc)
					{
						//getting ground ranges to targets
						double new_range = grnd_range[j];
						j++;
						if (new_range < range)
						{
							range = new_range;
							clost_tgt_slot = i;
						}
					}
				}//closest target determined
			}//first satellite that is able to provide targeting info
		}//satellite found
	}//all satellites were interrogated
	if (satellite_found)
	{
		//loading the closest target position for the 'guidance' module
		data_t = combus[clost_tgt_slot].get_data();
		wp_lonx = data_t[2].real();
		wp_latx = data_t[3].real();
		wp_alt = data_t[4].real();
		string target_id = combus[clost_tgt_slot].get_id();
		string satellite_id = combus[tgtng_sat_slot].get_id();
		out_count++;
		if (out_count == 1) cout << " *** Satellite_" << satellite_id << " tracks Target_" << target_id << " *** \n";
	}
	else
	{
		out_count++;
		if (out_count == 1) cout << " *** No satellite can track targets *** \n";
	}
	//-------------------------------------------------------------------------
	//loading module-variables
	//output to other modules
	cruise[132].gets(clost_tgt_slot);
	cruise[133].gets(tgtng_sat_slot);
	cruise[85].gets(wp_lonx);
	cruise[86].gets(wp_latx);
	cruise[87].gets(wp_alt);
}
///////////////////////////////////////////////////////////////////////////////  
//Determining whether satellites are visible from 'this' cruise missile and can track targets
// Checking for:	(1) are 'this' cruise missile and satellites in line-of-sight
//					(2) are first target of 'vehicle_list' and any satellite in line-of-sight 
//The status of all satellites (targeting or not-targeting) is stored in 'Targeting visibility[]'
//
//Assumption:
//  If the first target can be seen, so all targets can be seen by the satellite;
//  therefore target visibility is soley based on first target 
//Requirement:
// The 'Target'-object variables SBII(3x1) must be located in 'combus' at
//  'Packet data[i]', i=10;
//  this location is determined by the sequence of the "com"-key entry
//   in the module-variable array 'round3[]' 
//
//Output: 'Targeting Cruise::visibility[]'; entry: not visible = '0', visible = '1'
//		
//010813 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::targeting_satellite(Packet* combus, int num_vehicles)
{
	//local variables
	int k(0);
	int i(0);
	string id;
	Variable* data_first_target; //module-variable data of first target 
	Variable* data_sat;
	Matrix STII(3, 1);
	Matrix SSII(3, 1);
	double grazing_angle(0);
	double satellite_missile_angle(0);
	double satellite_target_angle(0);
	double alt_crit(0);
	bool first_target = false;

	//localizing module-variables
	//input data
	double del_radius = cruise[131].real();
	//input from other modules
	Matrix SBII = round3[35].vec();
	//-------------------------------------------------------------------------
	double radius = REARTH + del_radius;
	//locating first target in 'combus' and its inertial vector
	for (i = 0; i < num_vehicles; i++)
	{
		if (!first_target)
		{
			id = combus[i].get_id();
			int loc = (int)id.find("t"); //Schildt p.687, returns index (0, first letter) of match
			if (!loc)
			{
				first_target = true;
				data_first_target = combus[i].get_data();
				STII = data_first_target[10].vec();
			}
		}
	}
	//cycling through the vehicles, locating  satellites
	for (i = 0; i < num_vehicles; i++)
	{
		id = combus[i].get_id();
		int loc = (int)id.find("s"); //Schildt p.687, returns index (0, first letter) of match

		if (!loc)
			//satellite is found
		{
			data_sat = combus[i].get_data();
			SSII = data_sat[10].vec();
			//
				//Determining clear line-of-sight between satellite and cruise
			double dsi = SSII.absolute();
			grazing_angle = acos(radius / dsi);
			satellite_missile_angle = angle(SBII, SSII);

			if (satellite_missile_angle < grazing_angle)
				//always visible
			{
				visibility[k].tracking = 1;
				visibility[k].vehicle_slot = i;
				k++;
			}
			else
				//visible if altitude of cruise is sufficient
			{
				//critical altitude
				double radius_crit = 0;
				double dum = cos(satellite_missile_angle - grazing_angle);
				if (fabs(dum) > EPS) radius_crit = radius / dum;
				alt_crit = radius_crit - REARTH;
				double dbi = SBII.absolute();
				if (dbi > radius_crit)
				{
					visibility[k].tracking = 1;
					visibility[k].vehicle_slot = i;
					k++;
				}
				else
				{
					visibility[k].tracking = 0;
					visibility[k].vehicle_slot = i;
					k++;
				}
			}
			//determining if there is NO clear line-of-site to first target
			// then reset 'tracking=0'
			satellite_target_angle = angle(SBII, STII);
			if (satellite_target_angle > grazing_angle)
			{
				visibility[k - 1].tracking = 0;
			}
		}
	}//all satellites evaluated and stored in 'visibility[]'
}
///////////////////////////////////////////////////////////////////////////////  
//Calculating ground distances of cruise missile to all targets
// same function as 'seeker_grnd_ranges()'
//
//Assumption:
//	The 'Target'-object variables 'lonx' and 'latx' must be located in 'combus' at
// 'Packet data[i]', i=1 and i=2, respectively
//These locations are determined by the sequence of the "com"-key entry
// in the module-variable array 'round3[]' 
//
//Output: 'Cruise::grnd_range[]', ground range from current 'Cruise' object to all 'Target' objects
//		
//010813 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::targeting_grnd_ranges(Packet* combus, int num_vehicles)
{
	//local variables
	int k(0);
	string id;

	//localizing module-variables
	//input from other modules
	double lonx = round3[19].real();
	double latx = round3[20].real();
	//-------------------------------------------------------------------------
	double lon_c = lonx * RAD;
	double lat_c = latx * RAD;

	for (int i = 0; i < num_vehicles; i++)
	{
		id = combus[i].get_id();
		int loc = (int)id.find("t");
		if (!loc)
		{
			Variable* data_c2 = combus[i].get_data();
			double lonx_t = data_c2[2].real();
			double latx_t = data_c2[3].real();

			double lon_t = lonx_t * RAD;
			double lat_t = latx_t * RAD;

			//calculating separation distance over round earth
			double dum = sin(lat_t) * sin(lat_c) + cos(lat_t) * cos(lat_c) * cos(lon_t - lon_c);

			//load into 'grnd_range' array 
			grnd_range[k] = REARTH * acos(dum);
			k++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Definition of seeker module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[100-119]
//		
//010221 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_seeker()
{
	//definition of module-variables
	cruise[100].init("mseeker", "int", 0, "Mode switch - ND", "seeker", "data/save", "scrn");
	cruise[101].init("acq_range", 0, "Seeker acquisition range - m", "seeker", "data", "");
	cruise[105].init("range_go", 0, "LOS range-to-go - m", "seeker", "out", "plot,scrn");
	cruise[106].init("STBG", 0, 0, 0, "Displacement of target wrt vehicle - m", "seeker", "out", "plot");
	cruise[107].init("WOEB", 0, 0, 0, "LOS rate wrt earth  in body coord - rad/s", "seeker", "out", "");
	cruise[108].init("closing_speed", 0, "closing velocity - m/s^2", "seeker", "out", "");
	cruise[109].init("time_go", 0, "Time-to-go - s", "seeker", "out", "plot,scrn");
	cruise[110].init("psisbx", 0, "Seeker azimuth angle - deg", "seeker", "out", "plot,scrn");
	cruise[111].init("thtsbx", 0, "Seeker elevation angle - deg", "seeker", "out", "plot,scrn");
	cruise[112].init("targ_com_slot", "int", 0, "Slot # of target,attacked by cruise missile, in 'combus' - ND", "seeker", "save", "");
	cruise[113].init("UTBB", 0, 0, 0, "Unit LOS vector - ND", "seeker", "out", "");
	cruise[114].init("acquisition", "int", 0, "Acquisition flag (initially false) - ND", "seeker", "init/save", "scrn");
}
//$$$/////////////////////////////////////////////////////////////////////////////  
//Seeker module
//Member function of class 'Cruise'
//
//mseeker:
//			= 0 no seeker 
//			= 1 seeker enable
//			= 2 seeker acquisition
//			= 3 seeker tracking
//
//NOTE:
//The 'Target'-object variables 'lonx','latx','alt','psivgx','thtvgx','VTEG','STII'
// must be located in 'combus' at 'Packet data[i]', i=2,3,4,6,7,9,10 respectively.
//These locations are determined by the sequence of the "com"-key entry
// in the module-variable array 'round3[]'
//		
//010221 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////	
void Cruise::seeker(Packet* combus, int vehicle_slot, int num_vehicles, int num_target)
{
	//local variables
	double range(0);
	string target_id;
	Variable* data_t;
	double lonx_t(0); //target longitude
	double latx_t(0); //target latitude
	double alt_t(0); //target altitude
	double psivgx_t(0); //target heading angle
	double thtvgx_t(0); //target incline angle
	Matrix STBI(3, 1);  //displacement of target wrt cruise missile in inertial coordinates
	double inv_dtb(0); //inverse of range-to-go
	Matrix VTBG(3, 1);  //velocity of target wrt missle
	Matrix VBTG(3, 1);  //velocity of cruise missile wrt target
	Matrix DUM33(3, 3);
	Matrix DUM3(3, 1);
	Matrix UTBG(3, 1);  //unit LOS vector
	Matrix POLAR(3, 1);
	Matrix TGI(3, 3);	 //T.M. of geographic, wrt inertial coord
	Matrix STII(3, 1);  //Inertial target position
	Matrix VTEG(3, 1);  //Target geographic velocity

	//local module-variables
	double range_go(0);
	Matrix STBG(3, 1);
	Matrix WOEB(3, 1);
	double closing_speed(0);
	double time_go(0);
	double psisbx(0);
	double thtsbx(0);
	Matrix UTBB(3, 1);

	//localizing module-variables
	//input data
	int mseeker = cruise[100].integer();
	double acq_range = cruise[101].real();
	int acquisition = cruise[114].integer();
	//input from other modules
	double time = round3[0].real();
	Matrix TIG = round3[23].mat();
	Matrix VBEG = round3[32].vec();
	Matrix SBII = round3[35].vec();
	Matrix TBG = cruise[46].mat();
	//restore saved values
	int mcontrol = cruise[40].integer();
	int mguidance = cruise[80].integer();
	int targ_com_slot = cruise[112].integer();
	//-------------------------------------------------------------------------
	//returning if no seeker
	if (mseeker == 0) return;

	//building Cruise::grnd_range[] member array (ranges to all targets)
	seeker_grnd_ranges(combus, num_vehicles);

	//acquisition has not occured yet and seeker is enabled
	if (!acquisition && (mseeker == 1))
	{
		for (int j = 0; j < num_target; j++)
		{
			//getting groundranges to targets
			range = grnd_range[j];

			//picking the target within acquisition range
			if (range < acq_range)
			{
				acquisition = 1;
				//seeker starts tracking
				mseeker = 3;

				//building target id = t(j+1)
				char number[4];
				sprintf(number, "%i", j + 1);
				target_id = "t" + string(number);

				//finding slot 'i' of target in 'combus' (same as in vehicle_list)
				for (int i = 0; i < num_vehicles; i++)
				{
					string id = combus[i].get_id();
					if (id == target_id)
					{
						targ_com_slot = i;
					}
				}
				//getting cruise missile # (current vehicle = current'combus' slot) and target #
				string id_targ = combus[targ_com_slot].get_id();
				string id_missl = combus[vehicle_slot].get_id();

				//writing seeker acquisition message to console
				cout << "\n" << " *** Acquisition by Missile_" << id_missl << " of Target_" << target_id
					<< " at time = " << time << " sec ***\n\n";
			}
		}
	}//acquisition has occurred and target-packet slot # is identified in 'combus'

	//seeker is tracking
	if (mseeker == 3)
	{
		// getting data of target being tracked (tied to a special sequence in 'combus.data')
		data_t = combus[targ_com_slot].get_data();
		lonx_t = data_t[1].real();
		latx_t = data_t[2].real();
		alt_t = data_t[3].real();
		psivgx_t = data_t[6].real();
		thtvgx_t = data_t[7].real();
		VTEG = data_t[9].vec();
		STII = data_t[10].vec();

		//calculating relative cruise-target displacement in cruise geo. coord
		STBI = STII - SBII;
		TGI = TIG.trans();
		STBG = TGI * STBI;

		//calculating range-to-go
		range_go = STBG.absolute();

		//calculating unit vector of LOS and converting to skew-symmetric form
		inv_dtb = 1 / range_go;
		UTBG = STBG * inv_dtb;

		//calculating LOS rate in body coordinates
		VTBG = VTEG - VBEG;
		WOEB = TBG * UTBG.skew_sym() * VTBG * inv_dtb;

		//calculating closing speed (pos if closing on target, otherwise negative)
		VBTG = VTBG * (-1);
		closing_speed = UTBG ^ VBTG;

		//calculating time-to-go
		time_go = range_go / closing_speed;

		//LOS angles wrt body frame
		UTBB = TBG * UTBG;
		POLAR = UTBB.pol_from_cart();
		psisbx = POLAR.get_loc(1, 0) * DEG;
		thtsbx = POLAR.get_loc(2, 0) * DEG;

		//-------------------------------------------------------------------------
			//loading module-variables
			//(done in this block so the initial zero values are maintained until overwritten here)
			//output to other modules
		cruise[105].gets(range_go);
		cruise[106].gets_vec(STBG);
		cruise[107].gets_vec(WOEB);
		cruise[108].gets(closing_speed);
		cruise[109].gets(time_go);
		cruise[110].gets(psisbx);
		cruise[111].gets(thtsbx);
		cruise[113].gets_vec(UTBB);
	}
	//-------------------------------------------------------------------------
	//loading module-variables
	//saving variables
	cruise[40].gets(mcontrol);
	cruise[80].gets(mguidance);
	cruise[112].gets(targ_com_slot);
	cruise[114].gets(acquisition);
	//output to other modules
	cruise[100].gets(mseeker);
}

///////////////////////////////////////////////////////////////////////////////  
//Calculating ground distances to all targets
//
//Assumptions:
//	The 'Target'-object variables 'lonx' and 'latx' must be located in 'combus' at
// 'Packet data[i]', i=1 and i=2, respectively
//These locations are determined by the sequence of the "com"-key entry
// in the module-variable array 'round3[]' 
//
//Output: Cruise::grnd_range[], ground range from current 'Cruise' object to all 'Target' objects
//		
//010215 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::seeker_grnd_ranges(Packet* combus, int num_vehicles)
{
	//local variables
	int k = 0;
	string id;

	//localizing module-variables
	double lonx_c = round3[19].real();
	double latx_c = round3[20].real();

	double lon_c = lonx_c * RAD;
	double lat_c = latx_c * RAD;

	for (int i = 0; i < num_vehicles; i++)
	{
		id = combus[i].get_id();
		int loc = (int)id.find("t");
		if (!loc)
		{
			Variable* data_c2 = combus[i].get_data();
			double lonx_t = data_c2[2].real();
			double latx_t = data_c2[3].real();

			double lon_t = lonx_t * RAD;
			double lat_t = latx_t * RAD;

			//calculating separation distance over round earth
			double dum = sin(lat_t) * sin(lat_c) + cos(lat_t) * cos(lat_c) * cos(lon_t - lon_c);

			//load into 'grnd_range' array 
			grnd_range[k] = REARTH * acos(dum);
			k++;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
//Definition of guidance module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[80-99]
//		
//010209 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_guidance()
{
	//definition of module-variables
	cruise[80].init("mguidance", "int", 0, "Switch for guidance options - ND", "guidance", "data", "scrn");
	cruise[81].init("pronav_gain", 0, "Proportional navigation gain - ND", "guidance", "data", "");
	cruise[82].init("line_gain", 0, "Line guidance gain - 1/s", "guidance", "data", "");
	cruise[83].init("nl_gain_fact", 1, "Nonlinear gain factor - ND", "guidance", "data", "");
	cruise[84].init("decrement", 0, "distance decrement - m", "guidance", "data", "");
	cruise[85].init("wp_lonx", 0, "Longitude of way point - deg", "guidance", "data", "");
	cruise[86].init("wp_latx", 0, "Latitude of way point - deg", "guidance", "data", "");
	cruise[87].init("wp_alt", 0, "Altitude of way point - m", "guidance", "data", "");
	cruise[88].init("psifgx", 0, "Heading line-of-attack angle - deg", "guidance", "data", "");
	cruise[89].init("thtfgx", 0, "Pitch line-of-attack angle - deg", "guidance", "data", "");
	cruise[90].init("point_gain", 0, "Point guidance gain - 1/s", "guidance", "data", "");
	cruise[91].init("wp_sltrange", 999999, "Range to waypoint - m", "guidance", "dia", "");
	cruise[92].init("nl_gain", 0, "Nonlinear gain - rad", "guidance", "dia", "");
	cruise[93].init("VBEO", 0, 0, 0, "Vehicle velocity in LOS coordinats - m/s", "guidance", "dia", "");
	cruise[94].init("VBEF", 0, 0, 0, "Vehicle velocity in LOA coordinats - m/s", "guidance", "dia", "");
	cruise[95].init("wp_grdrange", 999999, "Ground range to waypoint - m", "guidance", "dia", "scrn,plot");
	cruise[96].init("SWBG", 0, 0, 0, "Vehicle wrt waypoint/target in geo coor - m", "guidance", "out", "");
	cruise[97].init("rad_min", 0, "Minimum arc radius, calculated - m", "guidance", "dia", "");
	cruise[98].init("rad_geometric", 0, "Geometric arc radius, calculated - m", "guidance", "diag", "");
	cruise[99].init("wp_flag", "int", 0, "=1:closing on target; =-1:fleeting; =0:outside - ND", "guidance", "dia", "plot");
}
//$$$//////////////////////////////////////////////////////////////////////////  
//Guidance module
//Member function of class 'Cruise'
//
//mguidance:
//			= 30 line-guidance lateral, with mcontrol 46 
//			= 03 line-guidance in pitch 
//			= 33 line-guidance lateral and in pitch, with mcontrol=44
//			= 40 point-guidance lateral, with mcontrol 46
//			= 43 point-guidance lateral, line-guidance in pitch, with mcontrol=44
//			= 60 po-nav lateral  
//			= 06 pro-nav in pitch 
//			= 66 po-nav lateral and in pitch, with mcontrol=44
//			= 70 arc-guidance lateral, with mcontrol=36
// 
//010209 Created by Peter H Zipfel
//010820 Added point guidance capability, PZi
//010823 New waypoint arc guidance, PZi
///////////////////////////////////////////////////////////////////////////////	
void Cruise::guidance()
{
	//local variables
	Matrix APNB(3, 1);
	Matrix ALGV(3, 1);
	Matrix APGV(3, 1);

	//local module-variables
	double ancomx(0);
	double alcomx(0);

	//localizing module-variables
	//input data
	int mguidance = cruise[80].integer();
	//input from other modules
	double grav = round3[11].real();
	double phicx = cruise[53].real();
	double anposlimx = cruise[58].real();
	double anneglimx = cruise[59].real();
	double allimx = cruise[72].real();
	double range_go = cruise[105].real();
	//-------------------------------------------------------------------------
	//returning if no guidance
	if (mguidance == 0)
	{
		alcomx = 0;
		ancomx = 0;
		return;
	}
	//lateral line guidance
	if (mguidance == 30)
	{
		ALGV = guidance_line();
		alcomx = ALGV.get_loc(1, 0) / grav;
	}
	//pitch line guidance
	if (mguidance == 3)
	{
		ALGV = guidance_line();
		alcomx = 0;
		ancomx = -ALGV.get_loc(2, 0) / grav;
	}
	//line guidance lateral and pitch
	if (mguidance == 33)
	{
		ALGV = guidance_line();
		alcomx = ALGV.get_loc(1, 0) / grav;
		ancomx = -ALGV.get_loc(2, 0) / grav;
	}
	//lateral pro-nav
	if (mguidance == 60)
	{
		APNB = guidance_pronav();
		alcomx = APNB.get_loc(1, 0) / grav;
		ancomx = 0;
	}
	//pitch pro-nav
	if (mguidance == 6)
	{
		APNB = guidance_pronav();
		alcomx = 0;
		ancomx = -APNB.get_loc(2, 0) / grav;
	}
	//pro-nav lateral and pitch
	if (mguidance == 66)
	{
		APNB = guidance_pronav();
		alcomx = APNB.get_loc(1, 0) / grav;
		ancomx = -APNB.get_loc(2, 0) / grav;
	}
	//point guidance lateral and line guidance pitch
	if (mguidance == 43)
	{
		ALGV = guidance_line();
		APGV = guidance_point();
		alcomx = APGV.get_loc(1, 0) / grav;
		ancomx = -ALGV.get_loc(2, 0) / grav;
	}
	//point guidance lateral
	if (mguidance == 40)
	{
		APGV = guidance_point();
		alcomx = APGV.get_loc(1, 0) / grav;
	}
	//arc guidance lateral
	if (mguidance == 70)
	{
		phicx = guidance_arc();
	}

	//limiting normal load factor command
	if (ancomx > anposlimx) ancomx = anposlimx;
	if (ancomx < anneglimx) ancomx = anneglimx;

	//limiting lateral load factor command
	if (alcomx > allimx) alcomx = allimx;
	if (alcomx < -allimx) alcomx = -allimx;
	//-------------------------------------------------------------------------
	//loading module-variables
	//output to other modules
	cruise[53].gets(phicx);
	cruise[70].gets(ancomx);
	cruise[71].gets(alcomx);
}

///////////////////////////////////////////////////////////////////////////////
//Pro-nav guidance
//applicable to mguidance:
//  						= 60 po-nav lateral  
//							= 06 pro-nav in pitch 
//							= 66 po-nav lateral and in pitch 
//return output: APNB(3x1) - m/s^2
//where:
//		alcomx=APNB(2)/grav, lateral acceleration command - g's
//		ancomx=-APNB(3)/grav, normal acceleration command - g's
//
//010301 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

Matrix Cruise::guidance_pronav()
{
	//local variables
	Matrix APNB(3, 1);
	Matrix GRAV_G(3, 1);

	//localizing module-variables
	//input data
	double pronav_gain = cruise[81].real();
	//input from other modules
	double grav = round3[11].real();
	Matrix TBG = cruise[46].mat();
	double range_go = cruise[105].real();
	Matrix WOEB = cruise[107].vec();
	double closing_speed = cruise[108].real();
	double psisbx = cruise[110].real();
	double thtsbx = cruise[111].real();
	Matrix UTBB = cruise[113].vec();
	//-------------------------------------------------------------------------
	//gravity vector in geographic coordinates
	GRAV_G.assign_loc(2, 0, grav);

	//required acceleration in body coordinates
	APNB = WOEB.skew_sym() * UTBB * (pronav_gain * closing_speed) - TBG * GRAV_G;

	return APNB;
}
///////////////////////////////////////////////////////////////////////////////
//Guidance to a line
//Against stationary waypoints or targets
//Waypoint are provided as: 'wp_lonx', 'wp_latx', 'wp_alt'
//applicable to mguidance:
//			= 30 line-guidance lateral 
//			= 03 line-guidance in pitch 
//			= 33 line-guidance lateral and in pitch
//return output:
//	 ALGV, acceleration demanded by line guidance in velocity coord. - m/s^2
//where:
//		alcomx=ALGV(2)/grav, lateral acceleration command - g's
//		ancomx=-ALGV(3)/grav, normal acceleration command - g's
//
//010405 Created by Peter H Zipfel
//010904 Upgraded to Waypoint guidance, PZi
///////////////////////////////////////////////////////////////////////////////

Matrix Cruise::guidance_line()
{
	//local variables
	Matrix ALGV(3, 1);
	Matrix TFG(3, 3);
	Matrix SWII(3, 1);
	Matrix TOG(3, 3);
	Matrix POLAR(3, 1);
	Matrix VH(3, 1);
	Matrix SH(3, 1);
	double psiog(0);
	double thtog(0);
	double swbg1(0);
	double swbg2(0);
	double vbeo2(0);
	double vbeo3(0);
	double vbef2(0);
	double vbef3(0);
	double algv1(0);
	double algv2(0);
	double algv3(0);
	double vbeg1(0);
	double vbeg2(0);
	double dvbe(0);
	double rad_min(0);

	//local module-variables
	double wp_sltrange(0);
	double wp_grdrange(0);
	Matrix VBEO(3, 1);
	Matrix VBEF(3, 1);
	Matrix SWBG(3, 1);
	double nl_gain(0);
	int wp_flag(0);

	//localizing module-variables
	//input data
	double line_gain = cruise[82].real();
	double nl_gain_fact = cruise[83].real();
	double decrement = cruise[84].real();
	double wp_lonx = cruise[85].real();
	double wp_latx = cruise[86].real();
	double wp_alt = cruise[87].real();
	double psifgx = cruise[88].real();
	double thtfgx = cruise[89].real();
	//input from other modules
	double time = round3[0].real();
	double grav = round3[11].real();
	Matrix TIG = round3[23].mat();
	double thtvgx = round3[29].real();
	Matrix VBEG = round3[32].vec();
	Matrix SBII = round3[35].vec();
	double philimx = cruise[56].real();
	//-------------------------------------------------------------------------
	//TM of LOA wrt geographic axes
	TFG = mat2tr(psifgx * RAD, thtfgx * RAD);

	//converting waypoint to inertial coordinates
	SWII = cadine(wp_lonx * RAD, wp_latx * RAD, wp_alt, time);

	//waypoint wrt cruise missile displacement in geographic coord (synthetic LOS)
	SWBG = TIG.trans() * (SWII - SBII);

	//building TM of LOS wrt geographic axes; also getting range-to-go to waypoint
	POLAR = SWBG.pol_from_cart();
	wp_sltrange = POLAR.get_loc(0, 0);
	psiog = POLAR.get_loc(1, 0);
	thtog = POLAR.get_loc(2, 0);
	TOG = mat2tr(psiog, thtog);

	//ground range to waypoint (approximated by using cruise missile local-level plane)
	swbg1 = SWBG.get_loc(0, 0);
	swbg2 = SWBG.get_loc(1, 0);
	wp_grdrange = sqrt(swbg1 * swbg1 + swbg2 * swbg2);

	//converting geographic cruise missile velocity to LOS and LOA coordinates
	VBEO = TOG * VBEG;
	vbeo2 = VBEO.get_loc(1, 0);
	vbeo3 = VBEO.get_loc(2, 0);

	VBEF = TFG * VBEG;
	vbef2 = VBEF.get_loc(1, 0);
	vbef3 = VBEF.get_loc(2, 0);

	//nonlinear gain
	nl_gain = nl_gain_fact * (1 - exp(-wp_sltrange / decrement));

	//line guidance steering law
	algv1 = grav * sin(thtvgx * RAD);
	algv2 = line_gain * (-vbeo2 + nl_gain * vbef2);
	algv3 = line_gain * (-vbeo3 + nl_gain * vbef3) - grav * cos(thtvgx * RAD);

	//packing accelerations int vector
	ALGV.assign_loc(0, 0, algv1);
	ALGV.assign_loc(1, 0, algv2);
	ALGV.assign_loc(2, 0, algv3);

	//setting way point flag (if within 2 times turning radius): closing (+1) or fleeting (-1)
	dvbe = VBEG.absolute();
	rad_min = dvbe * dvbe / (grav * tan(philimx * RAD));
	if (wp_grdrange < 2 * rad_min)
	{
		//projection of displacement vector into horizontal plane, SH
		SH.assign_loc(0, 0, swbg1);
		SH.assign_loc(1, 0, swbg2);
		SH.assign_loc(2, 0, 0);
		//projection of velocity vector into horizontal plane, VH
		vbeg1 = VBEG.get_loc(0, 0);
		vbeg2 = VBEG.get_loc(1, 0);
		VH.assign_loc(0, 0, vbeg1);
		VH.assign_loc(1, 0, vbeg2);
		VH.assign_loc(2, 0, 0);
		//setting flag eihter to +1 or -1
		wp_flag = sign(VH ^ SH);
	}
	else
		wp_flag = 0;
	//-------------------------------------------------------------------------
	//loading module-variables
	//diagnostics
	cruise[91].gets(wp_sltrange);
	cruise[92].gets(nl_gain);
	cruise[93].gets_vec(VBEO);
	cruise[94].gets_vec(VBEF);
	cruise[95].gets(wp_grdrange);
	cruise[96].gets_vec(SWBG);
	cruise[97].gets(rad_min);
	cruise[99].gets(wp_flag);

	return ALGV;
}

///////////////////////////////////////////////////////////////////////////////
//Guidance to a point
//special case of line guidance
//Against stationary waypoints or targets
//Waypoint are provided as: 'wp_lonx', 'wp_latx', 'wp_alt'
//applicable to mguidance:
//			= 40 point-guidance lateral 
//			= 04 point-guidance in pitch 
//			= 44 point-guidance lateral and in pitch
//return output:
//	 ALGV, acceleration demanded by point guidance in velocity coord. - m/s^2
//where:
//		alcomx=ALGV(2)/grav, lateral acceleration command - g's
//		ancomx=-ALGV(3)/grav, normal acceleration command - g's
//
//010816 Created by Peter H Zipfel
//010830 Upgraded to Waypoint guidance, PZi
///////////////////////////////////////////////////////////////////////////////

Matrix Cruise::guidance_point()
{
	//local variables
	Matrix APGV(3, 1);
	Matrix TFG(3, 3);
	Matrix SWII(3, 1);
	Matrix TOG(3, 3);
	Matrix POLAR(3, 1);
	Matrix VH(3, 1);
	Matrix SH(3, 1);
	double psiog(0);
	double thtog(0);
	double vbeg1(0);
	double vbeg2(0);
	double dvbe(0);
	double swbg1(0);
	double swbg2(0);
	double vbeo2(0);
	double vbeo3(0);
	double apgv1(0);
	double apgv2(0);
	double apgv3(0);

	//local module-variables
	double wp_sltrange(0);
	double wp_grdrange(0);
	double rad_min(0);
	int wp_flag(0);
	Matrix VBEO(3, 1);
	Matrix SWBG(3, 1);

	//localizing module-variables
	//input data
	double wp_lonx = cruise[85].real();
	double wp_latx = cruise[86].real();
	double wp_alt = cruise[87].real();
	double point_gain = cruise[90].real();
	//input from other modules
	double time = round3[0].real();
	double grav = round3[11].real();
	Matrix TIG = round3[23].mat();
	double thtvgx = round3[29].real();
	Matrix VBEG = round3[32].vec();
	Matrix SBII = round3[35].vec();
	double philimx = cruise[56].real();
	//-------------------------------------------------------------------------
	//converting waypoint to inertial coordinates
	SWII = cadine(wp_lonx * RAD, wp_latx * RAD, wp_alt, time);

	//waypoint wrt cruise missile displacement in geographic coord (synthetic LOS)
	SWBG = TIG.trans() * (SWII - SBII);

	//building TM of LOS wrt geographic axes; also getting range-to-go to waypoint
	POLAR = SWBG.pol_from_cart();
	wp_sltrange = POLAR.get_loc(0, 0);
	psiog = POLAR.get_loc(1, 0);
	thtog = POLAR.get_loc(2, 0);
	TOG = mat2tr(psiog, thtog);

	//ground range to waypoint (approximated by using cruise missile local-level plane)
	swbg1 = SWBG.get_loc(0, 0);
	swbg2 = SWBG.get_loc(1, 0);
	wp_grdrange = sqrt(swbg1 * swbg1 + swbg2 * swbg2);

	//converting geographic cruise missile velocity to LOS and LOA coordinates
	VBEO = TOG * VBEG;
	vbeo2 = VBEO.get_loc(1, 0);
	vbeo3 = VBEO.get_loc(2, 0);

	//point guidance steering law
	apgv1 = grav * sin(thtvgx * RAD);
	apgv2 = point_gain * (-vbeo2);
	apgv3 = point_gain * (-vbeo3) - grav * cos(thtvgx * RAD);

	//packing accelerations int vector
	APGV.assign_loc(0, 0, apgv1);
	APGV.assign_loc(1, 0, apgv2);
	APGV.assign_loc(2, 0, apgv3);

	//setting way point flag (if within 2 times turning radius): closing (+1) or fleeting (-1)
	dvbe = VBEG.absolute();
	rad_min = dvbe * dvbe / (grav * tan(philimx * RAD));
	if (wp_grdrange < 2 * rad_min)
	{
		//projection of displacement vector into horizontal plane, SH
		SH.assign_loc(0, 0, swbg1);
		SH.assign_loc(1, 0, swbg2);
		SH.assign_loc(2, 0, 0);
		//projection of velocity vector into horizontal plane, VH
		vbeg1 = VBEG.get_loc(0, 0);
		vbeg2 = VBEG.get_loc(1, 0);
		VH.assign_loc(0, 0, vbeg1);
		VH.assign_loc(1, 0, vbeg2);
		VH.assign_loc(2, 0, 0);
		//setting flag eihter to +1 or -1
		wp_flag = sign(VH ^ SH);
	}
	else
		wp_flag = 0;
	//-------------------------------------------------------------------------
	//loading module-variables
	//diagnostics
	cruise[91].gets(wp_sltrange);
	cruise[93].gets_vec(VBEO);
	cruise[95].gets(wp_grdrange);
	cruise[96].gets_vec(SWBG);
	cruise[97].gets(rad_min);
	cruise[99].gets(wp_flag);

	return APGV;
}
///////////////////////////////////////////////////////////////////////////////
//Guidance on an arc through a waypoint (horizontal only)
//Waypoints are provided as: 'wp_lonx', 'wp_latx', 'wp_alt'
//applicable to:
//			mguidance = 70 lateral arc-guidance 
//return output:
//	 phicx = commanded bank angle - deg
//
//010823 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Cruise::guidance_arc()
{
	//local variables
	Matrix SWII(3, 1);
	Matrix POLAR(3, 1);
	Matrix TBV(3, 3);
	Matrix FSPB(3, 1);
	Matrix VH(3, 1);
	Matrix SH(3, 1);
	Matrix UV(3, 1);
	Matrix ZZ(3, 1);
	double dwbh(0);
	double vbeg1(0);
	double vbeg2(0);
	double swbg1(0);
	double swbg2(0);
	double psiwvx(0);
	double fspb3(0);
	double num(0);
	double denom(0);
	double argument(0);
	double phicx(0);
	double alpha(0);
	double phimv(0);
	double rad_dynamic(0);
	double rad_geometric(0);

	//local module-variables
	Matrix SWBG(3, 1);
	int wp_flag(0);
	double wp_grdrange(0);
	double rad_min(0);

	//localizing module-variables
	//input data
	double wp_lonx = cruise[85].real();
	double wp_latx = cruise[86].real();
	double wp_alt = cruise[87].real();
	//input from other modules
	double time = round3[0].real();
	Matrix FSPV = round3[10].vec();
	double grav = round3[11].real();
	Matrix TIG = round3[23].mat();
	double dvbe = round3[25].real();
	double psivgx = round3[28].real();
	Matrix VBEG = round3[32].vec();
	Matrix SBII = round3[35].vec();
	double alphax = cruise[51].real();
	double phimvx = cruise[52].real();
	double philimx = cruise[56].real();
	//-------------------------------------------------------------------------
	//converting waypoint coordinates
	SWII = cadine(wp_lonx * RAD, wp_latx * RAD, wp_alt, time);

	//displacement of waypoint wrt cruise missile in geographic coord
	SWBG = TIG.trans() * (SWII - SBII);

	//projection of displacement vector into horizontal plane, SH
	swbg1 = SWBG.get_loc(0, 0);
	swbg2 = SWBG.get_loc(1, 0);
	SH.assign_loc(0, 0, swbg1);
	SH.assign_loc(1, 0, swbg2);
	SH.assign_loc(2, 0, 0);

	//horizontal ground distance based on cruise missile geographic coordinates
	dwbh = sqrt(swbg1 * swbg1 + swbg2 * swbg2);

	//calculating azimuth angle of waypoint LOS wrt velocity vector, psiwvx
	//projection of velocity vector into horizontal plane, VH
	vbeg1 = VBEG.get_loc(0, 0);
	vbeg2 = VBEG.get_loc(1, 0);
	VH.assign_loc(0, 0, vbeg1);
	VH.assign_loc(1, 0, vbeg2);
	VH.assign_loc(2, 0, 0);

	//vector normal to arc plane, UV
	UV = VH.skew_sym() * SH;
	//steering angle, psiwvx 
	psiwvx = DEG * angle(VH, SH);
	//steering angle with proper sign
	ZZ.assign_loc(2, 0, 1);
	psiwvx = psiwvx * sign(UV ^ ZZ);

	//tranforming specific force to body coordinates and picking third component
	alpha = alphax * RAD;
	phimv = phimvx * RAD;
	TBV = cadtbv(phimv, alpha);
	FSPB = TBV * FSPV;
	fspb3 = FSPB.get_loc(2, 0);

	//selecting guidance mode
	if (fabs(psiwvx) < 90)
		//guiding on the arc through the waypoint
	{
		num = -2 * dvbe * dvbe * sin(psiwvx * RAD);
		denom = fspb3 * dwbh;
		if (denom != 0) argument = num / denom;
		if (fabs(asin(argument)) < philimx * RAD)
			phicx = DEG * asin(argument);
		else
			phicx = philimx * sign(argument);
	}
	else
	{
		//making a minimum turn
		phicx = philimx * sign(psiwvx);
	}
	//diagnostic: radii
	if (phimvx != 0) rad_dynamic = fabs(dvbe * dvbe / (fspb3 * sin(phimvx * RAD)));
	if (psiwvx != 0) rad_geometric = fabs(dwbh / (2 * sin(psiwvx * RAD)));

	//setting way point flag (if within 2 times turning radius): closing (+1) or fleeting (-1)
	rad_min = dvbe * dvbe / (grav * tan(philimx * RAD));
	if (dwbh < 2 * rad_min)
		wp_flag = sign(VH ^ SH);
	else
		wp_flag = 0;

	//diagnostic: ground range to waypoint
	wp_grdrange = dwbh;
	//-------------------------------------------------------------------------
	//loading module-variables
	//output to other modules
	cruise[96].gets_vec(SWBG);
	//diagnostics
	cruise[95].gets(wp_grdrange);
	cruise[97].gets(rad_min);
	cruise[98].gets(rad_geometric);
	cruise[99].gets(wp_flag);

	return phicx;
}

///////////////////////////////////////////////////////////////////////////////
//Definition of control module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[40-79]
//		
//001228 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_control()
{
	//definition of module-variables
	cruise[40].init("mcontrol", "int", 0, "Mode switch - ND", "control", "data", "scrn");
	cruise[41].init("psivgcx", 0, "Commanded heading angle - deg", "control", "data", "plot");
	cruise[42].init("thtvgcx", 0, "Commanded flight path angle - deg", "control", "data", "plot");
	cruise[43].init("alphacx", 0, "Commanded angle of attack - deg", "control", "data", "");
	cruise[44].init("phimvcx", 0, "Commanded bank angle - deg", "control", "data", "");
	cruise[45].init("TBV", 0, 0, 0, 0, 0, 0, 0, 0, 0, "TM of body wrt velocity coord. - ND", "control", "out", "");
	cruise[46].init("TBG", 0, 0, 0, 0, 0, 0, 0, 0, 0, "TM of body wrt geog. coord. - ND", "control", "out", "");
	cruise[47].init("gain_thtvg", 0, "Flight path angle hold control gain - g/deg", "control", "data", "");
	cruise[48].init("gain_psivg", 0, "Heading angle hold control gain - ND", "control", "data", "");
	cruise[49].init("anx", 0, "Normal load factor - g", "control", "diag", "scrn,plot");
	cruise[50].init("avx", 0, "Vertical load factor - g", "control", "diag", "scrn,plot");
	cruise[51].init("alphax", 0, "Angle of attack - deg", "control", "out", "scrn,plot");
	cruise[52].init("phimvx", 0, "Bank angle - deg", "control", "out", "scrn,plot");
	cruise[53].init("phicx", 0, "Bank angle command- deg", "control", "data", "scrn,plot");
	cruise[54].init("phix", 0, "Bank angle state - deg", "control", "state", "plot");
	cruise[55].init("phixd", 0, "Bank angle state derivative - deg/sec", "control", "state", "");
	cruise[56].init("philimx", 0, "Bank angle command limiter - deg", "control", "data", "");
	cruise[57].init("tphi", 0, "Time constant of bank angle response - sec", "control", "data", "");
	cruise[58].init("anposlimx", 0, "Positive load factor limiter - g's", "control", "data", "");
	cruise[59].init("anneglimx", 0, "Negative load factor limiter - g's", "control", "data", "");
	cruise[60].init("gacp", 0, "Root locus gain of accel loop - rad/s^2", "control", "data", "");
	cruise[61].init("ta", 0, "Ratio of prop/integral gains. If>0, P-I engaged", "control", "data", "");
	cruise[62].init("xi", 0, "Integral feedback - rad/s", "control", "state", "");
	cruise[63].init("xid", 0, "Integral feedback derivative - rad/s^2", "control", "state", "");
	cruise[64].init("qq", 0, "Pitch rate - rad/s", "control", "diag", "plot");
	cruise[65].init("tip", 0, "Incidence lag time constant - s", "control", "diag", "plot");
	cruise[66].init("alp", 0, "Angle of attack - rad", "control", "state", "plot");
	cruise[67].init("alpd", 0, "Angle of attack derivative - rad/s", "control", "state", "");
	cruise[68].init("alpposlimx", 0, "Angle of attack positive limiter - deg", "control", "data", "");
	cruise[69].init("alpneglimx", 0, "Angle of attack negative limiter - deg", "control", "data", "");
	cruise[70].init("ancomx", 0, "Load factor command - g's", "control", "data", "scrn,plot");
	cruise[71].init("alcomx", 0, "Lateral acceleration command - g's", "control", "data", "scrn,plot");
	cruise[72].init("allimx", 0, "Lateral acceleration limiter - g's", "control", "data", "");
	cruise[73].init("gcp", 0, "Lateral roll gain - rad", "control", "data", "");
	cruise[74].init("alx", 0, "Lateral acceleration - g's", "control", "diag", "plot");
	cruise[75].init("altdlim", 0, "Altitude rate limiter - m/s", "control", "data", "");
	cruise[76].init("gh", 0, "Altitude gain - g/m", "control", "data", "");
	cruise[77].init("gv", 0, "Altitude rate gain - g/(m/s)", "control", "data", "");
	cruise[78].init("altd", 0, "Altitude rate  - m/s", "control", "diag", "plot");
	cruise[79].init("altcom", 0, "Altitude command  - m", "control", "data", "plot");
}
//$$$////////////////////////////////////////////////////////////////////////// 
//Control module
//Member function of class 'Cruise' 
//
//mcontrol = 00: No control, phimv=0, alpha=0
//			 01: Flight path angle control, no heading control, thtvgcx
//			 10: Heading and alpha control, psivgcx, alphacx
//			 11: Heading and flight path angle control, thtvgcx and psivgcx
//			 03: Bank angle controller, phicx
//			 04: Accleration control in load-factor plane, ancomx
//			 40: Accleration control in lateral (horizontal) plane, alcomx
//			 44: Accleration control in lateral and load-factor plane alcomx, ancomx
//			 06: Altitude hold control with inner acceleration autopilot, altcom
//			 16: Heading and altitude hold controller, psivgcx, altcom			 			  
//			 46: Lateral acceleration and altitude controller, alcomx, altcom
//			 36: Bank angle control and altitude hold (used with mguidance=70), phicx, altcom
//
//000725 Created by Michael Horvath
//001228 Upgraded to module-variable arrays, PZi
//010126 Added acceleration and altitude controllers, PZi
///////////////////////////////////////////////////////////////////////////////	
void Cruise::control(double int_step)
{
	//local module-variables
	double phimvx(0);
	double alphax(0);
	Matrix TBV(3, 3);
	Matrix TBG(3, 3);
	Matrix TVG(3, 3);

	//localizing module-variables
	//input data
	int mcontrol = cruise[40].integer();
	double thtvgcx = cruise[42].real();
	double psivgcx = cruise[41].real();
	double alphacx = cruise[43].real();
	double ancomx = cruise[70].real();
	double alcomx = cruise[71].real();
	double altcom = cruise[79].real();
	//restore saved values
	double phicx = cruise[53].real();
	//input from other modules
	Matrix TGV = round3[22].mat();
	//-------------------------------------------------------------------------
	//No control
	if (mcontrol == 0)
	{
		phimvx = 0;
		alphax = 0;
	}
	//Flight path angle control
	if (mcontrol == 1)
	{
		phimvx = 0;
		alphax = control_flightpath(thtvgcx, phimvx);
	}
	//Heading and alpha control
	if (mcontrol == 10)
	{
		phicx = control_heading(psivgcx);
		phimvx = control_bank(phicx, int_step);
		alphax = alphacx;
	}
	//heading and flight path angle control
	if (mcontrol == 11)
	{
		phicx = control_heading(psivgcx);
		phimvx = control_bank(phicx, int_step);
		alphax = control_flightpath(thtvgcx, phimvx);
	}
	//bank angle and alpha control
	if (mcontrol == 3)
	{
		phimvx = control_bank(phicx, int_step);
		alphax = alphacx;
	}
	//load factor control
	if (mcontrol == 4)
	{
		alphax = control_load(ancomx, int_step);
	}
	//lateral acceleration control
	if (mcontrol == 40)
	{
		phicx = control_lateral(alcomx);
		phimvx = control_bank(phicx, int_step);
	}
	//acceleration control in both planes
	if (mcontrol == 44)
	{
		phicx = control_lateral(alcomx);
		phimvx = control_bank(phicx, int_step);
		alphax = control_load(ancomx, int_step);
	}

	//altitude control
	if (mcontrol == 6)
	{
		ancomx = control_altitude(altcom, phimvx);
		alphax = control_load(ancomx, int_step);
	}
	//heading and altitude
	if (mcontrol == 16)
	{
		phicx = control_heading(psivgcx);
		phimvx = control_bank(phicx, int_step);

		ancomx = control_altitude(altcom, phimvx);
		alphax = control_load(ancomx, int_step);
	}
	//lateral acceleration and altitude control
	if (mcontrol == 46)
	{
		phicx = control_lateral(alcomx);
		phimvx = control_bank(phicx, int_step);

		ancomx = control_altitude(altcom, phimvx);
		alphax = control_load(ancomx, int_step);
	}
	//lateral bank control and altitude control
	if (mcontrol == 36)
	{
		phimvx = control_bank(phicx, int_step);

		ancomx = control_altitude(altcom, phimvx);
		alphax = control_load(ancomx, int_step);
	}
	//calculating TBV and direction cosine matrix TBG;
	TBV = cadtbv(phimvx * RAD, alphax * RAD);
	TVG = TGV.trans();
	TBG = TBV * TVG;
	//-------------------------------------------------------------------------
	//loading module-variables
	//saving variables
	cruise[53].gets(phicx);
	//output to other modules
	cruise[45].gets_mat(TBV);
	cruise[46].gets_mat(TBG);
	cruise[51].gets(alphax);
	cruise[52].gets(phimvx);
	cruise[70].gets(ancomx);
}

///////////////////////////////////////////////////////////////////////////////
//Heading angle control
//
//return output: phimvx, bank angle - deg 
//
//000725 Created by Michael Horvath
//001228 Upgraded to module-variable arrays, PZi
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_heading(double psivgcx)
{
	//local variables
	double phimvx(0);
	double psivgx_comp(0);
	double sign_psivgx(0);

	//localizing module-variables
	//input data
	double gain_psivg = cruise[48].real();
	//input from other modules
	double psivgx = round3[28].real();
	//-------------------------------------------------------------------------
	//elininating the singulatiry at psivgx=+-180 deg by giving special
	//treatment to the heading control of +-45 deg from south
	if (fabs(psivgcx) <= 135)
		psivgx_comp = psivgx;
	else
	{
		if (psivgx * psivgcx >= 0)
			psivgx_comp = psivgx;
		else
		{
			if (psivgx >= 0)sign_psivgx = 1;
			else sign_psivgx = -1;
			psivgx_comp = 360 - psivgx * sign_psivgx;
		}
	}
	phimvx = gain_psivg * (psivgcx - psivgx_comp);
	return phimvx;
}

///////////////////////////////////////////////////////////////////////////////
//Flight path angle control
//
//return output: alphax, angle of attack - deg
//
//000725 Created by Michael Horvath
//001228 Upgraded to module-variable arrays, PZi
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_flightpath(double thtvgcx, double phimvx)
{

	//local module-variables
	double anx(0);
	double avx(0);
	double alphax(0);

	//localizing module-variables
	//input data
	double gain_thtvg = cruise[47].real();
	double alpposlimx = cruise[68].real();
	double alpneglimx = cruise[69].real();
	//input from other modules
	double pdynmc = round3[13].real();
	double thtvg = round3[18].real();
	double grav = round3[11].real();
	double mass = cruise[23].real();
	double area = cruise[33].real();
	double cla = cruise[34].real();
	//-------------------------------------------------------------------------
	//vertical and normal loadfactors
	avx = gain_thtvg * (thtvgcx * RAD - thtvg);
	anx = ((avx) / (cos(phimvx * RAD)));
	alphax = ((anx * mass * grav) / (pdynmc * area * cla));

	//limiting angle of attack
	if (alphax > alpposlimx) alphax = alpposlimx;
	if (alphax < alpneglimx) alphax = alpneglimx;

	//loading module-variables
	cruise[49].gets(anx);
	cruise[50].gets(avx);

	return alphax;
}
///////////////////////////////////////////////////////////////////////////////
//Bank angle controller
//
//return output: phix, bank angle - deg
//
//010126 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_bank(double phicx, double int_step)
{
	//local variables
	double phixd_new(0);

	//localizing module-variables
	//input data
	double philimx = cruise[56].real();
	double tphi = cruise[57].real();
	//state variables
	double phix = cruise[54].real();
	double phixd = cruise[55].real();
	//-------------------------------------------------------------------------	
	//limiting bank angle command
	if (phicx > philimx) phicx = philimx;
	if (phicx < -philimx) phicx = -philimx;

	//bank angle lag
	phixd_new = (phicx - phix) / tphi;
	phix = integrate(phixd_new, phixd, phix, int_step);
	phixd = phixd_new;
	//-------------------------------------------------------------------------
	//loading module-variables
	//state variables
	cruise[54].gets(phix);
	cruise[55].gets(phixd);

	return phix;
}
///////////////////////////////////////////////////////////////////////////////
//Load factor controller
//
//return output: alpx, angle of attack - deg
//
//010129 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_load(double ancomx, double int_step)
{
	//local variables
	Matrix FSPB(3, 1);
	Matrix TBV(3, 3);
	double alpha(0);
	double phimv(0);
	double fspb3(0);
	double eanx(0);
	double gr(0);
	double gi(0);
	double xid_new(0);
	double alpd_new(0);
	double alpx(0);

	//local module-variables
	double anx(0);
	double tip(0);
	double qq(0);

	//localizing module-variables
	//input data
	double anposlimx = cruise[58].real();
	double anneglimx = cruise[59].real();
	double phimvx = cruise[52].real();
	double gacp = cruise[60].real();
	double ta = cruise[61].real();
	double alphax = cruise[51].real();
	double alpposlimx = cruise[68].real();
	double alpneglimx = cruise[69].real();
	//input from other modules
	Matrix FSPV = round3[10].vec();
	double grav = round3[11].real();
	//state variables
	double xi = cruise[62].real();
	double xid = cruise[63].real();
	double alp = cruise[66].real();
	double alpd = cruise[67].real();
	//input from other modules
	double mass = cruise[23].real();
	double dvbe = round3[25].real();
	double pdynmc = round3[13].real();
	double thrust = cruise[14].real();
	double area = cruise[33].real();
	double cla = cruise[34].real();
	//-------------------------------------------------------------------------
	//tranforming specific force to body coordinates
	alpha = alphax * RAD;
	phimv = phimvx * RAD;
	TBV = cadtbv(phimv, alpha);
	FSPB = TBV * FSPV;

	//limiting load factor command
	if (ancomx > anposlimx) ancomx = anposlimx;
	if (ancomx < anneglimx) ancomx = anneglimx;

	//load factor feedback
	fspb3 = FSPB.get_loc(2, 0);
	anx = -fspb3 / grav;

	//error signal
	eanx = ancomx - anx;

	//incidence lag time constant
	tip = dvbe * mass / (pdynmc * area * cla / RAD + thrust);

	//integral path
	if (ta > 0)
	{
		gr = gacp * tip / dvbe;
		gi = gr / ta;
		xid_new = gi * eanx;
		xi = integrate(xid_new, xid, xi, int_step);
		xid = xid_new;
	}
	else xi = 0;

	//proportional path with integral signal(if present)
	qq = gr * eanx + xi;

	//incidence lag dynamics
	alpd_new = qq - alp / tip;
	alp = integrate(alpd_new, alpd, alp, int_step);
	alpd = alpd_new;

	alpx = alp * DEG;

	//limiting angle of attack
	if (alpx > alpposlimx) alpx = alpposlimx;
	if (alpx < alpneglimx) alpx = alpneglimx;
	//-------------------------------------------------------------------------
	//loading module-variables
	//state variables
	cruise[62].gets(xi);
	cruise[63].gets(xid);
	cruise[66].gets(alp);
	cruise[67].gets(alpd);
	//diagnostics
	cruise[49].gets(anx);
	cruise[64].gets(qq);
	cruise[65].gets(tip);

	return alpx;
}
///////////////////////////////////////////////////////////////////////////////
//Lateral acceleration controller
//
//return output: phicx, bank command - deg
//
//010130 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_lateral(double alcomx)
{
	//local variables
	Matrix FSPB(3, 1);
	Matrix TBV(3, 3);
	double alpha(0);
	double phimv(0);
	double fspb3(0);
	double anx(0);
	double sign(0);
	double phic(0);
	double phicx(0);
	double fspv2(0);

	//local module-variables
	double alx(0);

	//localizing module-variables
	//input data
	double allimx = cruise[72].real();
	double phimvx = cruise[52].real();
	double alphax = cruise[51].real();
	double gcp = cruise[73].real();
	//input from other modules
	Matrix FSPV = round3[10].vec();
	double grav = round3[11].real();
	//-------------------------------------------------------------------------
	//tranforming specific force to body coordinates
	alpha = alphax * RAD;
	phimv = phimvx * RAD;
	TBV = cadtbv(phimv, alpha);
	FSPB = TBV * FSPV;
	//normal load factor
	fspb3 = FSPB.get_loc(2, 0);
	anx = -fspb3 / grav;

	//limiting lateral load factor command
	if (alcomx > allimx) alcomx = allimx;
	if (alcomx < -allimx) alcomx = -allimx;

	//commanded bank angle
	if (anx >= 0)sign = 1;
	else sign = -1;
	phic = gcp * sign / (fabs(anx) + .001) * alcomx;
	phicx = phic * DEG;

	//diagnostic: lateral acceleration achieved
	fspv2 = FSPV.get_loc(1, 0);
	alx = fspv2 / grav;
	//-------------------------------------------------------------------------
	//loading module-variables
	cruise[74].gets(alx);

	return phicx;
}

///////////////////////////////////////////////////////////////////////////////
//Altitude controller
//
//return output: ancomx, load factor command - g's
//
//010131 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

double Cruise::control_altitude(double altcom, double phimvx)
{
	//local variables
	double ealt(0);
	double ancomx(0);

	//local module-variables
	double altd(0);

	//localizing module-variables
	//input data
	double anposlimx = cruise[58].real();
	double anneglimx = cruise[59].real();
	double altdlim = cruise[75].real();
	double gh = cruise[76].real();
	double gv = cruise[77].real();
	//input from other modules
	double alt = round3[21].real();
	double grav = round3[11].real();
	Matrix VBEG = round3[32].vec();
	//-------------------------------------------------------------------------
	//altitude error
	ealt = gh * (altcom - alt);

	//limiting altitude rate
	if (ealt > altdlim) ealt = altdlim;
	if (ealt < -altdlim) ealt = -altdlim;

	//altitude rate feedback
	altd = -VBEG.get_loc(2, 0);

	//load factor command
	ancomx = (gv * (ealt - altd) / grav + 1) * (1 / cos(phimvx * RAD));

	//limiting load factor command
	if (ancomx > anposlimx) ancomx = anposlimx;
	if (ancomx < anneglimx) ancomx = anneglimx;
	//-------------------------------------------------------------------------
	//loading module-variables
	//diagnsotics
	cruise[78].gets(altd);

	return ancomx;
}
///////////////////////////////////////////////////////////////////////////////
//Definition of intercept module-variables
//Member function of class 'Cruise'
//Module-variable locations are assigned to cruise[120-129]
//		
//010328 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::def_intercept()
{
	//definition of module-variables
	cruise[121].init("write", "int", 1, "True flag for writing miss to console - ND", "intercept", "save", "");
	cruise[122].init("miss", 0, "Miss distance - m", "intercept", "diag", "");
	cruise[123].init("hit_time", 0, "Intercept time - s", "intercept", "diag", "");
	cruise[124].init("MISS_G", 0, 0, 0, "Miss vector in geog. coord. - m", "intercept", "diag", "");
	cruise[125].init("time_m", 0, "Previous time - s", "intercept", "save", "");
	cruise[126].init("SBTGM", 0, 0, 0, "Previous displacment vector. - m", "intercept", "save", "");
	cruise[127].init("STMEG", 0, 0, 0, "Previous taget displacment vector. - m", "intercept", "save", "");
	cruise[128].init("SBMEG", 0, 0, 0, "Previous cruise missile displacment vector. - m", "intercept", "save", "");
}
//$$$//////////////////////////////////////////////////////////////////////////
//Intercept module
//Member function of class 'Cruise'
//Determining closest approach of cruise missile and target points
//
//Parameter Input: 'vehicle_slot' is current 'Cruise' object
//Input from module-variable array: 'targ_com_slot' target being attacked, determined in 'seeker' module
//
//Console output: miss distance and associated parameters written to console
//
//010328 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////
void Cruise::intercept(Packet* combus, int vehicle_slot, double int_step, const char* title)
{
	//local variables
	double tau(0);
	Matrix SBTG(3, 1);
	Matrix SBBMG(3, 1);
	double swbg1(0);
	double swbg2(0);
	double dwbh(0);

	//local module-variables
	double hit_time(0);
	Matrix MISS_G(3, 1);
	double miss(0);

	//localizing module-variables
	//input from other modules
	double time = round3[0].real();
	double dvbe = round3[25].real();
	double alt = round3[21].real();
	double psivgx = round3[28].real();
	double thtvgx = round3[29].real();
	Matrix SBEG = round3[31].vec();
	//restore saved values
	int write = cruise[121].integer();
	double time_m = cruise[125].real();
	Matrix SBTGM = cruise[126].vec();
	Matrix STMEG = cruise[127].vec();
	Matrix SBMEG = cruise[128].vec();
	//input from other modules
	int mguidance = cruise[80].integer();
	double wp_lonx = cruise[85].real();
	double wp_latx = cruise[86].real();
	double wp_alt = cruise[87].real();
	Matrix SWBG = cruise[96].vec();
	int wp_flag = cruise[99].integer();
	int mseeker = cruise[100].integer();
	double range_go = cruise[105].real();
	Matrix STBG = cruise[106].vec();
	double closing_speed = cruise[108].real();
	int targ_com_slot = cruise[112].integer();
	//-------------------------------------------------------------------------
	//Ground impact
	if ((alt <= 0) && write)
	{
		write = 0;

		//getting cruise missile #
		string id_missl = combus[vehicle_slot].get_id();

		//writing miss information to console
//		cout<<"\n"<<" *** "<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<" ***";
		cout << "\n" << " *** Ground impact of Missile_" << id_missl << "   Time = " << time << " sec ***\n";
		cout << "      speed = " << dvbe << " m/s  heading = " << psivgx << " deg      gamma = " << thtvgx << " deg\n\n";

		//declaring cruise missile 'dead'
		combus[vehicle_slot].set_status(0);
	}
	//Waypoint hoizontal miss distance 
	if (mguidance == 70 || mguidance == 40 || mguidance == 30)
	{
		if (wp_flag == -1)
		{

			swbg1 = SWBG.get_loc(0, 0);
			swbg2 = SWBG.get_loc(1, 0);
			dwbh = sqrt(swbg1 * swbg1 + swbg2 * swbg2);

			//getting cruise missile #
			string id_missl = combus[vehicle_slot].get_id();

			//writing miss information to console
			cout << "\n" << " *** Missile " << id_missl << " overflies waypoint at longitude = "
				<< wp_lonx << " deg, latitude = " << wp_latx << " deg at time = " << time << " sec *** \n";
			cout << "      SWBG-horizontal miss distance  = " << dwbh << " m north = " << SWBG.get_loc(0, 0)
				<< " m  east = " << SWBG.get_loc(1, 0) << " m\n";
		}
	}
	//Terminal line/line or point/line guidance
	if (mguidance == 33 || mguidance == 43)
	{
		//Line guidance: simple miss distance calculation at last integration, no interpolation
		if ((alt <= wp_alt) && write)
		{
			write = 0;

			miss = SWBG.absolute();

			//getting cruise missile #
			string id_missl = combus[vehicle_slot].get_id();

			//writing miss information to console
	//		cout<<"\n"<<" *** "<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<" ***";
			cout << "\n" << " *** Impact of Missile_" << id_missl << " on waypoint coord.: longitude = " << wp_lonx << " deg, latitude = "
				<< wp_latx << " deg, altitude = " << wp_alt << " m\n";
			cout << "      miss distance = " << miss << " m    intercept time = " << time << " sec\n";
			cout << "      north = " << SWBG.get_loc(0, 0) << " m      east = " << SWBG.get_loc(1, 0)
				<< " m        down = " << SWBG.get_loc(2, 0) << " m\n";
			cout << "      speed = " << dvbe << " m/s heading = " << psivgx << " deg     gamma = " << thtvgx << " deg\n\n";

			//declaring cruise missile 'dead'
			combus[vehicle_slot].set_status(0);
		}
	}//end of terminal lin/line or point/line guidance

	//Seeker/pronav
	if (mseeker == 3)
	{
		//entering sphere of target influence of 100m 
		if (range_go < 100)
		{
			Variable* data_t;
			Matrix STEG(3, 1);
			Matrix SBBMG(3, 1);
			Matrix STTMG(3, 1);

			//get target location
			data_t = combus[targ_com_slot].get_data();
			STEG = data_t[8].vec();

			//Intercept (closing speed becomes negative)
			//Miss is closest distance between cruise missile and target points; obtained by linear interpolation
			//between integration steps
			SBTG = STBG * (-1);
			if ((closing_speed < 0) && write)
			{
				write = 0;

				SBBMG = SBEG - SBMEG;
				STTMG = STEG - STMEG;

				//intercept time at point of closest approach
				hit_time = time_m - int_step * ((SBBMG - STTMG) ^ SBTGM) / (SBBMG ^ SBBMG);

				//miss distance vector in geographic coordinates
				tau = hit_time - time_m;
				MISS_G = (SBBMG - STTMG) * (tau / int_step) + SBTGM;
				miss = MISS_G.absolute();

				//getting cruise missile # and target #
				string id_targ = combus[targ_com_slot].get_id();
				string id_missl = combus[vehicle_slot].get_id();

				//writing miss information to console
	//			cout<<"\n"<<" *** "<<title<<"   "<< __DATE__ <<" "<< __TIME__ <<" ***";
				cout << "\n" << " *** Intercept of Missile_" << id_missl << " and Target_" << id_targ << " ***\n";
				cout << "      miss distance = " << miss << " m    intercept time = " << hit_time << " sec\n";
				cout << "      north = " << MISS_G.get_loc(0, 0) << " m      east = " << MISS_G.get_loc(1, 0)
					<< " m        down = " << MISS_G.get_loc(2, 0) << " m\n";
				cout << "      speed = " << dvbe << " m/s heading = " << psivgx << " deg     gamma = " << thtvgx << " deg\n\n";

				//declaring cruise missile 'dead (0)'and target 'hit (-1)'
				combus[targ_com_slot].set_status(-1);
				combus[vehicle_slot].set_status(0);
			}
			//save from previous cycle
			SBTGM = SBTG;
			STMEG = STEG;
			SBMEG = SBEG;
			time_m = time;
		}
	}//end of seeker/pronav
	//-------------------------------------------------------------------------
	//loading module-variables
	//saving variables
	cruise[121].gets(write);
	cruise[125].gets(time_m);
	cruise[126].gets_vec(SBTGM);
	cruise[127].gets_vec(STMEG);
	cruise[128].gets_vec(SBMEG);
	//diagnostics
	cruise[122].gets(miss);
	cruise[123].gets(hit_time);
	cruise[124].gets_vec(MISS_G);
}
