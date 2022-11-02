#ifndef	OS_API
#define	OS_API

/*
 *	os_api.h -- A minimal API for operating system model in SystemC
 *
 *	System-Level Architecture and Modeling Lab
 *	Department of Electrical and Computer Engineering
 *	The University of Texas at Austin 
 *
 * 	Author: Kamyar Mirzazad Barijough (kammirzazad@utexas.edu)
 *  Edited 10/7/22: Erika Susana Alcorta added channel skeleton
 */

#include <systemc.h>
#include <queue>

class   os_api : virtual public sc_interface
{
	public:

	virtual int	pre_wait() = 0;
	virtual void	post_wait(int) = 0;
	virtual void	time_wait(int, sc_time_unit) = 0;
	virtual void	task_terminate() = 0;
	virtual void	reg_task(const char*) = 0;
};

class OS: public virtual sc_channel, public os_api
{
    /* Add your code in this class */
    class Task {
      public:
        int id;
        sc_event e;
        const char* name;
    };
    Task* current_task = NULL;
    int task_count = 0;
    std::queue<Task*> rdyq;
    Task * alivetasks[3];

public:
  OS(const sc_module_name name): sc_channel(name) {}

  void dispatch() {
    if (!rdyq.empty()){
      current_task = rdyq.front();
      rdyq.pop();
      // cout << "\t Dispatching " << current_task->name << endl;
      // cout << "\t\tnotify " << current_task->name << endl;
      current_task->e.notify(SC_ZERO_TIME);
    }
    else {
      current_task = NULL;
    }
  }

  void yield() {
    Task *t = current_task;
    rdyq.push(t);
    // cout << "\t yield::pushing " << t->name << endl;
    dispatch();
    // if (current_task != t){
      // cout << "\t\twait " << t->name << endl;
      wait(t->e);
    // }
  }

  void reg_task(const char* name) {
    Task *t = new Task();
    alivetasks[task_count] = t;
    t->id = task_count;
    t->name = name;
    task_count++;
    if (current_task == NULL) {
      current_task = t;
      // cout << "\t reg_task::current_task=" << current_task->name << endl;
    }
    else {
      rdyq.push(t);
      // cout << "\t\twait " << t->name << endl;
      wait(t->e);
    }
   }

  int pre_wait() { 
    Task *t = current_task;
    // cout << "\t pre_wait" << t->name << endl;
    dispatch();
    return t->id;
  }
  void post_wait(int task_id) {
    Task *t = alivetasks[task_id];
    rdyq.push(t);
    // cout << "\t post_wait::push" << t->name << endl;
    if (current_task == NULL) {
      dispatch();
    }
    // if (current_task != t) {
      // cout << "\t\twait " << t->name << endl;
      wait(t->e);
    // }
  }
  
  void time_wait(int time, sc_time_unit unit) {
    wait(time, unit);
    // cout << "\t time_wait::yield" << endl;
    yield();
  }
  void task_terminate() {
    delete current_task;
    dispatch();
  }
  

};

#endif
