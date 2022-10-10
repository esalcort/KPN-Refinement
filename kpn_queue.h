#ifndef	KPN_QUEUE
#define	KPN_QUEUE

#include <systemc.h>

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
        queue->write(data[i-1]);
    }
  }

  void get(T* &data, int length) {
    for(int i = 0; i < length; i++) {
        queue->read(data[i]);
    }
  }
  

};

#endif
