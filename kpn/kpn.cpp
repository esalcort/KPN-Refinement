#include <systemc.h>
#include "os_api.h"
#include "HWBus.h"

SC_MODULE(A)
{
  sc_port<os_api> os;
  sc_fifo_out<int*> ab;
  int * abptr;

  SC_CTOR(A) { SC_THREAD(run); }

  void run(void) {

    cout << "A1: " << sc_time_stamp() << endl;
    wait(10, SC_NS);

    cout << "A2: " << sc_time_stamp() << endl;
    wait(1, SC_NS);
  
    ab.write(abptr);
    wait(5, SC_NS);

    cout << "A3: " << sc_time_stamp() << endl;
    wait(1, SC_NS);

    cout << "A4: " << sc_time_stamp() << endl;
    wait(1, SC_NS);
  }
};

SC_MODULE(B)
{
  sc_port<os_api> os;
  sc_fifo_in<int *> ab;
  int * abptr;

  SC_CTOR(B) { SC_THREAD(run); }

  void run(void) {

    cout << "B1: " << sc_time_stamp() << endl;
    wait(10, SC_NS);

    cout << "B2: " << sc_time_stamp() << endl;
    wait(5, SC_NS);

    ab.read(abptr);
    wait(5, SC_NS);

    cout << "B3: " << sc_time_stamp() << endl;
    wait(10, SC_NS);

    cout << "B4: " << sc_time_stamp() << endl;
    wait(10, SC_NS);

  }
};

SC_MODULE(Top) {
  OS os;
  sc_fifo<int*> f1;

  A a;
  B b;

  int * abptr = new int[10];

  SC_CTOR(Top): os("OS"), a("A"), b("B") {
    a.os(os);
    b.os(os);
    a.ab(f1);
    b.ab(f1);
    a.abptr = abptr;
  }

  void start_of_simulation() { os.reg_task("A"); os.reg_task("B");}
};

int sc_main(int, char*[])
{
  Top top("Top");
  sc_start();
  cout << "Done: " << sc_time_stamp() << endl;
  return 0;
}