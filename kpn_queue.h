#ifndef	KPN_QUEUE
#define	KPN_QUEUE

#include <systemc.h>
#include "HWBus.h"

template<typename T>
class   queue_api : virtual public sc_interface
{
	public:

	virtual void    put(T*, int) = 0;
	virtual void    get(T*&, int) = 0;
};

template<typename T>
class kpn_queue: public virtual sc_channel, public queue_api<T>
{
public:

    sc_fifo<T> *queue;
  kpn_queue(const sc_module_name name, int size): sc_channel(name) {
    queue = new sc_fifo<T>(size);
   }

  void put(T *data, int length) {
    for(int i = 0; i < length; i++) {
        queue->write(data[i]);
    }
  }

  void get(T* &data, int length) {
    for(int i = 0; i < length; i++) {
        queue->read(data[i]);
    }
  }
  

};

class MultiMasterMACTLM : public IMasterHardwareBusLinkAccess, public sc_channel
{
  public:
    sc_port<IMasterHardwareBusProtocol> protocol;
    sc_mutex busy;
  MultiMasterMACTLM(sc_module_name name) : sc_channel(name) {}
	void	MasterRead(int addr, void *data, unsigned long len) {  }

	void	MasterWrite(int addr, const void* data, unsigned long len) {
    // cout << "\t\t" << sc_time_stamp() << " Master MAC Ping to" << addr << endl;
    int ping=0;
    unsigned long i;
		unsigned char *p;
		sc_uint<DATA_WIDTH> word = 0;
   
		for(p = (unsigned char*)data, i = 0; i < len; i++, p++)
		{
			word = (word<<8) + *p;
      
			if(!((i+1)%DATA_BYTES)) 
			{
        // cout << "\t\t" << sc_time_stamp() << " Master MAC Write to" << addr << endl;
        if (ping == 0){
          do {
            busy.lock();
            ping = protocol->masterPingSlave(addr, word);
            if (ping == -1) {
              busy.unlock();
              wait(100, SC_NS);
            }
          } while(ping == -1);
        }
        else {
				  protocol->masterWrite(addr, word);
        }
				word = 0;
      }
		}
    
		if(i%DATA_BYTES)
		{
			word <<= 8 * (DATA_BYTES - (i%DATA_BYTES));
      // cout << "\t\t" << sc_time_stamp() << " Master MAC Write to" << addr << endl;
			protocol->masterWrite(addr, word);
		}
    busy.unlock();
  }
};

template<typename T>
class MasterDriverWrite: public kpn_queue<T>
{
public:
  sc_port<IMasterHardwareBusLinkAccess> MAC;
  int address;

  MasterDriverWrite(sc_module_name name, int addr): kpn_queue<T>(name, 1)
  {
    address = addr;
  }

  void put(T *data, int length) override
  {
    // cout << "\t" << sc_time_stamp() << " Master Driver Write to" << address << endl;
    MAC->MasterWrite(address, data, length);
  }

  void get(T* &data, int length) override
  {
    cerr << "Trying to read on write-only driver" << endl;
    sc_abort();
  }
  

};

template<typename T>
class SlaveDriverRead: public virtual sc_channel, public queue_api<T>
{
public:
  sc_port<ISlaveHardwareBusLinkAccess> MAC;
  int address;

  SlaveDriverRead(sc_module_name name, int addr): address(addr) {}

  void put(T *data, int length) //override
  {
    cerr << "Trying to write on a read-only driver" << endl;
    sc_abort();
  }

  void get(T* &data, int length) //override
  {
    // cout << "\t" << sc_time_stamp() << " Slave Driver Read to" << address << endl;
    MAC->SlaveRead(address, data, length);
  }
  
  

};


#endif
