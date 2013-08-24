#ifndef _messages
#define _messages

#include <iostream>

#include "node.h"
#include "link.h"
#include "table.h"

#if defined(GENERIC)
struct RoutingMessage {
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(LINKSTATE)
struct RoutingMessage {
  Link link;
  RoutingMessage(const Link l);
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)
struct RoutingMessage {
 
  unsigned srcNum;
  /*
   * struct cell {
    unsigned fromNum;
	double latency;
  };
  */
  map<int, Table::cell>myVector;

  RoutingMessage(map<int, Table::cell> v, unsigned src);
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}

#endif
