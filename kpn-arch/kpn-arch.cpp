#include <systemc.h>
#include "os_api.h"
#include "HWBus.h"
#include "kpn_queue.h"

#define FIXED_ARRAY_LENGTH  32
#define TOTAL_INPUTS   32


SC_MODULE(PE1)
{
  sc_port<os_api> os;
  sc_port<queue_api<float> > iq;
  kpn_queue<float> qA;
  sc_port<queue_api<float> > qB;
  sc_port<queue_api<float> > qC;

  SC_CTOR(PE1) : qA("qA", FIXED_ARRAY_LENGTH) { SC_THREAD(run_A); SC_THREAD(run_B)}

  void run_A(void) {

    float aArray[FIXED_ARRAY_LENGTH];
    float *aPtr = aArray;
      
    os->reg_task("A");

    while(true) {
      
      cout << "A1: " << sc_time_stamp() << endl;
      os->time_wait(10, SC_NS);

      int t_id = os->pre_wait();
      iq->get(aPtr, FIXED_ARRAY_LENGTH);
      os->post_wait(t_id);


      cout << "A2: " << sc_time_stamp() << endl;
      os->time_wait(1, SC_NS);
    
      t_id = os->pre_wait();
      qC->put(aPtr, FIXED_ARRAY_LENGTH);
      os->post_wait(t_id);
      os->time_wait(1, SC_NS);


      cout << "A3: " << sc_time_stamp() << endl;
      os->time_wait(3, SC_NS);

      cout << "A4: " << sc_time_stamp() << endl;
      os->time_wait(10, SC_NS);

      t_id = os->pre_wait();
      qA.put(aPtr, FIXED_ARRAY_LENGTH);
      os->post_wait(t_id);
      os->time_wait(1, SC_NS);
    }
  }

  void run_B(void) {

    float bArray[FIXED_ARRAY_LENGTH];
    float *bPtr = bArray;

    os->reg_task("B");

    while(true) {

      cout << "B1: " << sc_time_stamp() << endl;
      os->time_wait(10, SC_NS);

      cout << "B2: " << sc_time_stamp() << endl;
      os->time_wait(5, SC_NS);

      int t_id = os->pre_wait();
      qA.get(bPtr, FIXED_ARRAY_LENGTH);
      os->post_wait(t_id);
      os->time_wait(1, SC_NS);

      cout << "B3: " << sc_time_stamp() << endl;
      os->time_wait(10, SC_NS);

      cout << "B4: " << sc_time_stamp() << endl;
      os->time_wait(10, SC_NS);

      t_id = os->pre_wait();
      qB->put(bPtr, FIXED_ARRAY_LENGTH);
      os->post_wait(t_id);
      os->time_wait(1, SC_NS);
    }
  }

};

SC_MODULE(PE2)
{
  sc_port<queue_api<float> > qB;
  sc_port<queue_api<float> > qC;

  SC_CTOR(PE2) { SC_THREAD(run_C); }

  void run_C(void) 
  {
    float aArray[FIXED_ARRAY_LENGTH];
    float bArray[FIXED_ARRAY_LENGTH];
    float *aPtr = aArray;
    float *bPtr = bArray;
    
    while(true) {
      qC->get(aPtr, FIXED_ARRAY_LENGTH);
      qB->get(bPtr, FIXED_ARRAY_LENGTH);

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

  // sc_fifo<float*> iq;
  kpn_queue<float> iq;
  kpn_queue<float> qB;
  kpn_queue<float> qC;

  float *inPtr[TOTAL_INPUTS];

  SC_CTOR(Top): os("OS"), pe1("PE1"), pe2("PE2"),
                iq("iq", FIXED_ARRAY_LENGTH),
                qB("qB", FIXED_ARRAY_LENGTH),
                qC("qC", FIXED_ARRAY_LENGTH) {
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
      inPtr[i] = new float[FIXED_ARRAY_LENGTH];
      iq.put(inPtr[i], FIXED_ARRAY_LENGTH);
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