#pragma once
#include "Round3.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Derived class: Cruise
//
//Second level of derived class of the 'Cadac' hierarchy
//Models aerodynamics, propulsion, guidance and control 
//Contains Modules: aerodynamics, propulsion, forces, control, guidance 
//
//011128 Created by Peter H Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////

class Cruise :public Round3
{
protected:
	//name of CRUISE3 vehicle object
	char cruise3_name[CHARL] = {};

	//event list of 'Event' object pointers and actual number of events 
	Event* event_ptr_list[NEVENT] = {}; int nevent = 0;
	//total number of envents for a vehicle object
	int event_total = 0;

	//compacted array of all module-variables of vehicle object 'Cruise'
	Variable* cruise3 = nullptr; int ncruise3 = 0;

	//screen output array of module-variables of vehicle object 'Cruise'
	Variable* scrn_cruise3 = nullptr; int nscrn_cruise3 = 0;

	//plot output array of module-variables of vehicle object 'Cruise'
	Variable* plot_cruise3 = nullptr; int nplot_cruise3 = 0;

	//communications output array of module-variables of vehicle object 'Cruise'
	Variable* com_cruise3 = nullptr; int ncom_cruise3 = 0;

	//packet of data for each cruise vehicle
	Packet packet;

	//indicator array pointing to the module-variable which are to 
	// be written to the screen
	int* cruise_scrn_ind = nullptr; int cruise_scrn_count = 0;

	//indicator array pointing to the module-variable which are to 
	// be written to the 'ploti.asc' files
	int* cruise_plot_ind = nullptr; int cruise_plot_count = 0;

	//indicator array pointing to the module-variable which are to 
	// be written to 'combus' 'packets'
	int* cruise_com_ind = nullptr; int cruise_com_count = 0;

	//array of ground distances of 'Missile' object from all 'Target' objects
	double* grnd_range = nullptr;

	//array of all satellites with indicator whether visible form 'this' missile
	// and first target to assure that satellite can provide targeting data to missile 
	Targeting* visibility = nullptr;

	//declaring Table pointer as temporary storage of a single table
	Table* table = nullptr;
	//	declaring Datadeck 'aerotable' that stores all aerodynamic tables
	Datadeck aerotable;
	//	declaring Datadeck 'proptable' that stores all propulsion tables
	Datadeck proptable;

public:
	Cruise() {};
	Cruise(Module* module_list, int num_modules, int num_target, int num_satellite);
	virtual~Cruise();

	//executive functions
	void sizing_arrays() override;
	void vehicle_array() override;
	void scrn_array() override;
	void plot_array() override;
	void scrn_banner() override;
	void tabout_banner(ofstream& ftabout, const char* title) override;
	void tabout_data(ofstream& ftabout) override;
	void vehicle_data(fstream& input) override;
	void read_tables(const char* file_name, Datadeck& datatable) override;
	void scrn_index_arrays() override;
	void scrn_data() override;
	void plot_banner(ofstream& fplot, const char* title) override;
	void plot_index_arrays() override;
	void plot_data(ofstream& fplot, bool merge) override;
	void event(const char* options) override;
	void document(ostream& fdoc, const char* title, Document* doc_cruise3) override;
	void com_index_arrays() override;
	Packet loading_packet_init(int num_cruise, int num_target, int num_satellite) override;
	Packet loading_packet(int num_cruise, int num_target, int num_satellite) override;

	//module functions
	void def_aerodynamics() override;
	void aerodynamics() override;
	void def_propulsion() override;
	void init_propulsion() override;
	void propulsion(double int_step) override;
	void def_forces() override;
	void forces() override;
	void def_control() override;
	void control(double int_step) override;
	void def_guidance() override;
	void guidance() override;
	void def_targeting() override;
	virtual void targeting(Packet* combus, int vehicle_slot, int num_vehicles
		, int num_target, int num_satellite) override;
	virtual void def_seeker();
	virtual void seeker(Packet* combus, int vehicle_slot, int num_vehicles, int num_target);
	virtual void def_intercept();
	virtual void intercept(Packet* combus, int vehicle_slot, double int_step, const char* title);

	//functions of control module
	double control_heading(double psivgcx);
	double control_flightpath(double thtvgcx, double phimv);
	double control_bank(double phicx, double int_step);
	double control_load(double ancomx, double int_step);
	double control_lateral(double alcomx);
	double control_altitude(double altcom, double phimvx);

	//functions of guidance module
	Matrix guidance_line();
	Matrix guidance_pronav();
	Matrix guidance_point();
	double guidance_arc();

	//functions of seeker module 
	void seeker_grnd_ranges(Packet* combus, int num_vehicles);

	//functions of targeting module
	void targeting_satellite(Packet* combus, int num_vehicles);
	void targeting_grnd_ranges(Packet* combus, int num_vehicles);
};
