#pragma once
#include "Round3.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Derived class: Target
//
//Second level of derived class of the 'Cadac' hierarchy, branching from 'Round3'
//Models target accelerations
//Contains Module 'forces' and 'intercept'
//
//010205 Created by Peter H Zipfel
///////////////////////////////////////////////////////////////////////////////

class Target :public Round3
{
protected:
	//name of TARGET3 vehicle object
	char target3_name[CHARL] = "";

	//Communications output array of module-variables of vehicle object 'Target'
	Variable* com_target3 = nullptr; int ncom_target3 = 0;

	//Packet of data for each target vehicle
	Packet packet;

	//Indicator array pointing to the module-variable which are to 
	//be written to 'combus' 'packets'
	int* target_com_ind = nullptr; int target_com_count = 0;
public:
	Target();
	Target(Module* module_list, int num_modules);
	virtual~Target();

	//executive functions dummy returns
	void vehicle_array() override {};
	void scrn_array() override {};
	void plot_array() override {};
	void scrn_banner() override {};
	void tabout_banner(ofstream& ftabout, const char* title) override {};
	void tabout_data(ofstream& ftabout) override {};
	void scrn_index_arrays() override {};
	void scrn_data() override {};
	void plot_banner(ofstream& fplot, const char* title) override {};
	void plot_index_arrays() override {};
	void plot_data(ofstream& fplot, bool merge) override {};
	void event(const char* options) override {};
	virtual void doc_input(fstream& input) {};

	//executive functions active
	void sizing_arrays() override;
	void vehicle_data(fstream& input) override;
	void read_tables(const char* file_name, Datadeck& datatable) override {};
	void com_index_arrays() override;
	void document(ostream& fdoc, const char* title, Document* doc_vehicle) override;
	Packet loading_packet_init(int num_cruise, int num_target, int num_satellite) override;
	Packet loading_packet(int num_cruise, int num_target, int num_satellite) override;

	//module function dummy returns
	void def_aerodynamics() override {};
	void aerodynamics() override {};
	void def_propulsion() override {};
	void init_propulsion() override {};
	void propulsion(double int_step) override {};
	void def_control() override {};
	void control(double int_step) override {};
	void def_guidance() override {};
	void guidance() override {};
	void def_targeting() override {};
	void targeting(Packet* combus, int vehicle_slot, int num_vehicles
		, int num_target, int num_satellite) override {};
	void def_seeker() override {};
	void seeker(Packet* combus, int vehicle_slot, int num_vehicles, int num_target) override {};

	//module functions active
	void def_forces() override;
	void forces() override;
	void def_intercept() override;
	void intercept(Packet* combus, int vehicle_slot, double int_step, const char* title) override;
};
