#include <systemc.h>
#include "os_api.h"
#include "HWBus.h"

#define FIXED_TRANSACTION_SIZE  128
#define TOTAL_INPUTS   32


SC_MODULE(PE1)
{
  sc_port<os_api> os;
  sc_fifo_in<float*> iq;
  sc_fifo<float*> qA;
  sc_fifo_out<float*> qB;
  sc_fifo_out<float*> qC;

  SC_CTOR(PE1) { SC_THREAD(run_A); SC_THREAD(run_B);}

  void run_A(void) {

    float *aPtr;

    while(true) {
      
      os->reg_task("A");
      
      cout << "A1: " << sc_time_stamp() << endl;
      wait(10, SC_NS);

      int t_id = os->pre_wait();
      iq.read(aPtr);
      os->post_wait(t_id);


      cout << "A2: " << sc_time_stamp() << endl;
      wait(1, SC_NS);
    
      t_id = os->pre_wait();
      qC.write(aPtr);
      os->post_wait(t_id);
      wait(1, SC_NS);


      cout << "A3: " << sc_time_stamp() << endl;
      wait(3, SC_NS);

      cout << "A4: " << sc_time_stamp() << endl;
      wait(10, SC_NS);

      t_id = os->pre_wait();
      qA.write(aPtr);
      os->post_wait(t_id);
      wait(1, SC_NS);
    }
  }

  void run_B(void) {

    float *bPtr;

    while(true) {

      os->reg_task("B");

      cout << "B1: " << sc_time_stamp() << endl;
      wait(10, SC_NS);

      cout << "B2: " << sc_time_stamp() << endl;
      wait(5, SC_NS);

      int t_id = os->pre_wait();
      qA.read(bPtr);
      os->post_wait(t_id);
      wait(1, SC_NS);

      cout << "B3: " << sc_time_stamp() << endl;
      wait(10, SC_NS);

      cout << "B4: " << sc_time_stamp() << endl;
      wait(10, SC_NS);

      t_id = os->pre_wait();
      qB.write(bPtr);
      os->post_wait(t_id);
    }
  }

};

SC_MODULE(PE2)
{
  sc_fifo_in<float*> qB;
  sc_fifo_in<float*> qC;

  SC_CTOR(PE2) { SC_THREAD(run_C); }

  void run_C(void) 
  {
    float *aPtr;
    float *bPtr;
    while(true) {
      qB.read(aPtr);
      qC.read(bPtr);

      cout << "C1: " << sc_time_stamp() << endl;
      wait(20, SC_NS);

      cout << "C2: " << sc_time_stamp() << endl;
      wait(20, SC_NS);
  }
  }

};

SC_MODULE(Top) {
  OS os;

  PE1 pe1;
  PE2 pe2;

  sc_fifo<float*> iq;
  sc_fifo<float*> qB;
  sc_fifo<float*> qC;

  float *inPtr[TOTAL_INPUTS];

  SC_CTOR(Top): os("OS"), pe1("PE1"), pe2("PE2") {
    pe1.os(os);
    pe1.qB(qB);
    pe1.qC(qC);
    pe1.iq(iq);
    pe2.qB(qB);
    pe2.qC(qC);
    SC_THREAD(send_inputs);
  }

  ~Top() {
    for (int i = 0; i < TOTAL_INPUTS; i++) {
      delete inPtr[i];
    }
  }

  void send_inputs() {
    for (int i = 0; i < TOTAL_INPUTS; i++) {
      inPtr[i] = new float[FIXED_TRANSACTION_SIZE];
      iq.write(inPtr[i]);
    }
  }

  void start_of_simulation() { }
};

int sc_main(int, char*[]) {
  Top top("Top");
  sc_start();
  cout << "Done: " << sc_time_stamp() << endl;
  return 0;
}