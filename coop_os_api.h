#ifndef	COOPERATIVE_OS_API
#define	COOPERATIVE_OS_API

/*
 *	coop_os_api.h -- A minimal API for cooperative operating systems
 *
 *	System-Level Architecture and Modeling Lab
 *	Department of Electrical and Computer Engineering
 *	The University of Texas at Austin 
 *
 * 	Author: Kamyar Mirzazad Barijough (kammirzazad@utexas.edu)
 */

#include <systemc.h>

class   coop_os_api : virtual public sc_interface
{
	public:

	virtual int	pre_wait() = 0;
	virtual void	yield() = 0;
	virtual void	post_wait(int) = 0;
	virtual void	time_wait(int) = 0; // time in ms
	virtual void	task_terminate() = 0;
	virtual void	reg_task(const char*) = 0;
};

#endif
