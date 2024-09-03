#pragma once

#include "global_header.hpp"
#include "Datadeck.hpp"
#include "Variable.hpp"
#include "Packet.hpp"
#include "Document.hpp"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
//Abstract base class: Cadac
//
//011128 Created by Peter H Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////
class Cadac
{
private:
	char name[CHARN] = {}; //vehicle object name

protected:

	//module-variable array of class 'Round3'
	//first 10 locations are reserved for executive variables
	Variable* round3 = nullptr;

	//array of module-variables as defined in class 'Cruise'
	//first 10 locations are reserved for executive variables
	Variable* cruise = nullptr;

	//Array of module-variables as defined in class 'Target'
	//first 10 locations are reserved for executive variables
	Variable* target = nullptr;

	//Array of module-variables as defined in class 'Satellite'
	//first  10 locations are  reserved for executive variables
	Variable* satellite = nullptr;


public:
	//flag indicating an 'event' has occured
	bool event_epoch = false;

	//time elapsed in event 
	double event_time = 0; //event_time

	///////////////////////////////////////////////////////////////////////////
	//Constructor of class 'Cadac'
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	Cadac() {};

	///////////////////////////////////////////////////////////////////////////
	//Destructor of class 'Cadac'
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	virtual~Cadac() {};

	///////////////////////////////////////////////////////////////////////////
	//Setting vehicle object name
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	void set_name(const char* vehicle_name) { strcpy(name, vehicle_name); }

	///////////////////////////////////////////////////////////////////////////
	//Getting vehicle object name
	//
	//010703 Created by Peter H Zipfel
	///////////////////////////////////////////////////////////////////////////
	const char* get_vname() { return name; }

	//////////////////////////executive functions /////////////////////////////
	virtual void sizing_arrays() = 0;
	virtual void vehicle_array() = 0;
	virtual void scrn_array() = 0;
	virtual void plot_array() = 0;
	virtual void scrn_banner() = 0;
	virtual void tabout_banner(std::ofstream& ftabout, const char* title) = 0;
	virtual void tabout_data(std::ofstream& ftabout) = 0;
	virtual void vehicle_data(std::fstream& input) = 0;
	virtual void read_tables(const char* file_name, Datadeck& datatable) = 0;
	virtual void scrn_index_arrays() = 0;
	virtual void scrn_data() = 0;
	virtual void plot_banner(std::ofstream& fplot, const char* title) = 0;
	virtual void plot_index_arrays() = 0;
	virtual void plot_data(std::ofstream& fplot, bool merge) = 0;
	virtual void event(const char* options) = 0;
	virtual void document(std::ostream& fdoc, const char* title, Document* doc_cruise3) = 0;
	virtual void com_index_arrays() = 0;
	virtual Packet loading_packet_init(int num_cruise, int num_target, int num_satellite) = 0;
	virtual Packet loading_packet(int num_cruise, int num_target, int num_satellite) = 0;

	//module functions -MOD
	virtual void def_environment() = 0;
	virtual void init_environment(double sim_time, double int_step) = 0;
	virtual void environment(double sim_time, double event_time, double& int_step, double& out_fact) = 0;
	virtual void def_aerodynamics() = 0;
	virtual void aerodynamics() = 0;
	virtual void def_propulsion() = 0;
	virtual void init_propulsion() = 0;
	virtual void propulsion(double int_step) = 0;
	virtual void def_forces() = 0;
	virtual void forces() = 0;
	virtual void def_newton() = 0;
	virtual void init_newton() = 0;
	virtual void newton(double int_step) = 0;
	virtual void def_control() = 0;
	virtual void control(double int_step) = 0;
	virtual void def_guidance() = 0;
	virtual void guidance() = 0;
	virtual void def_targeting() = 0;
	virtual void targeting(Packet* combus, int vehicle_slot, int num_vehicles
		, int num_target, int num_satellite) = 0;
	virtual void def_seeker() = 0;
	virtual void seeker(Packet* combus, int vehicle_slot, int num_vehicles, int num_target) = 0;
	virtual void def_intercept() = 0;
	virtual void intercept(Packet* combus, int vehicle_slot, double int_step, const char* title) = 0;
};
