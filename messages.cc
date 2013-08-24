#include "messages.h"


#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
  os << "RoutingMessage()";
  return os;
}
#endif


#if defined(LINKSTATE)

ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage(const Link l)
{
  link = l;
}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs) : link(rhs.link)
{

}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage &rhs) {
   return *(new(this)RoutingMessage(rhs));
}

#endif


#if defined(DISTANCEVECTOR)

ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage(map<int, Table::cell> v, unsigned src) {
  srcNum = src;
  myVector = v;
}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs) : srcNum(rhs.srcNum), myVector(rhs.myVector) {
}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage &rhs) {
   return *(new(this)RoutingMessage(rhs));
}

#endif

