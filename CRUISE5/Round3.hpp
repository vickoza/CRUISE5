#pragma once

#include "Cadac.hpp"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Derived class: Round3
//
//First derived class in the 'Cadac' hierarchy
//Models atmosphere, gravitational acceleration and equations of motions
//Contains modules: environment and Newton's law
//
//011128 Created by Peter H Zipfel
//030627 Adapted to CRUISE simulation, PZi
///////////////////////////////////////////////////////////////////////////////
class Round3 :public Cadac
{
protected:
	//Indicator array pointing to the module-variable which are to 
	//be written to the screen
	int* round3_scrn_ind = nullptr; int round3_scrn_count = 0;

	//Indicator array pointing to the module-variable which are to 
	//be written to the 'ploti.asc' files
	int* round3_plot_ind = nullptr; int round3_plot_count = 0;

	//Indicator array pointing to the module-variable which are to 
	//be written to 'combus' 'packets'
	int* round3_com_ind = nullptr; int round3_com_count = 0;
public:
	Round3();
	virtual~Round3() {};

	//virtual functions to be defined in this class
	void def_environment() override;
	void init_environment(double sim_time, double int_step)  override;
	void environment(double sim_time, double event_time, double& int_step, double& out_fact)  override;
	void def_newton()  override;
	void init_newton()  override;
	void newton(double int_step)  override;
};
