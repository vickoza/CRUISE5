#pragma once
#include "Round3.hpp"

///////////////////////////////////////////////////////////////////////////////
//Derived class: Satellite
//
//Second level of derived class of the 'Cadac' hierarchy, branching from 'Round3'
//Contains Module 'forces' and 'seeker'
//
//010810 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

class Satellite :public Round3
{
protected:
	//name of SATELLITE3 vehicle object
	char satellite3_name[CHARL];

	//Communications output array of module-variables of vehicle object 'Satellite'
	Variable* com_satellite3 = nullptr; int ncom_satellite3 = 0;

	//Packet of data for each satellite vehicle
	Packet packet;

	//Indicator array pointing to the module-variable which are to 
	//be written to 'combus' 'packets'
	int* satellite_com_ind = nullptr; int satellite_com_count = 0;

public:
	Satellite();
	Satellite(Module* module_list, int num_modules);
	virtual~Satellite();

	//executive functions dummy returns
	virtual void vehicle_array() override {};
	virtual void scrn_array() override {};
	virtual void plot_array() override {};
	virtual void scrn_banner() override {};
	virtual void tabout_banner(ofstream& ftabout, const char* title) override {};
	virtual void tabout_data(ofstream& ftabout) override {};
	virtual void scrn_index_arrays() override {};
	virtual void scrn_data() override {};
	virtual void plot_banner(ofstream& fplot, const char* title) override {};
	virtual void plot_index_arrays() override {};
	virtual void plot_data(ofstream& fplot, bool merge) override {};
	virtual void event(const char* options) override {};
	virtual void doc_input(fstream& input) {};

	//executive functions active
	virtual void sizing_arrays() override;
	virtual void vehicle_data(fstream& input) override;
	virtual void read_tables(const char* file_name, Datadeck& datatable) override {};
	virtual void com_index_arrays() override;
	virtual void document(ostream& fdoc, const char* title, Document* doc_vehicle) override;
	virtual Packet loading_packet_init(int num_cruise, int num_target, int num_satellite) override;
	virtual Packet loading_packet(int num_cruise, int num_target, int num_satellite) override;

	//module function dummy returns
	virtual void def_aerodynamics() override {};
	virtual void aerodynamics() override {};
	virtual void def_propulsion() override {};
	virtual void init_propulsion() override {};
	virtual void propulsion(double int_step) override {};
	virtual void def_control() override {};
	virtual void control(double int_step) override {};
	virtual void def_guidance() override {};
	virtual void guidance() override {};
	virtual void def_targeting() override {};
	virtual void targeting(Packet* combus, int vehicle_slot, int num_vehicles
		, int num_target, int num_satellite) override {};
	virtual void def_intercept() override {};
	virtual void intercept(Packet* combus, int vehicle_slot, double int_step, const char* title) override {};
	virtual void def_seeker() override {};
	virtual void seeker(Packet* combus, int vehicle_slot, int num_vehicles, int num_target) override {};

	//module functions active
	virtual void def_forces() override;
	virtual void forces() override;
};

//////////////////////////////////////////////////////////////////////////////
//Definition of force module-variables
//Member function of class 'Satellite'
//
//Note that FSPV is entered into the round3[10] array because it is needed
//for the 'newton' module, which is a member of the 'Round3' class
//		
//010812 Created by Peter Zipfel
///////////////////////////////////////////////////////////////////////////////

