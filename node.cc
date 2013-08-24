#include "node.h"
#include "context.h"
#include "error.h"

/*
Node::Node(const unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l) 
{}
Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}
*/
void Node::SetNumber(const unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}


// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time


/*
  void Node::SendToNeighbors(const RoutingMessage *m) {
  deque<Node*> *pNei = GetNeighbors();
  deque<Node*> neighbors = *pNei;
  for (deque<Node*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
    SendToNeighbor( *it, m);
  }
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m) {
  double t = GetTime();
  Event *newEvent = new Event(t, ROUTING_MESSAGE_ARRIVAL, (void*)n, (void*)m);
  
}
*/


deque<Node*> *Node::GetNeighbors() const
{
  return context->GetNeighbors(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)
Node::Node(const unsigned n, SimulationContext *c, double b, double l) :number(n), context(c), table(n)
{
  //cout<<"ncons:"<<n<<endl;
}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), table(rhs.number) {}
  
Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr<<"linkhasbeenup: begin"<<endl;
  if (table.isNew(l)) {
    cerr<<"isNew"<<endl;
    table.AddLink(l);
    table.update();
    RoutingMessage *msg = new RoutingMessage(*l);
    context->SendToNeighbors(this, msg);
  }
  cerr << *this<<": Link Update: "<<*l<<endl;
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  Link l = m->link;
  if (table.isNew(&l)) {
    table.AddLink(&l);
    table.update();
    RoutingMessage *msg = new RoutingMessage(l);
    context->SendToNeighbors(this, msg);
  }
  cerr << *this << " Routing Message: "<<*m;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  unsigned destNumber = destination->number;
  unsigned nextHop = table.getNextHop(destNumber);
  deque<Node*> *Pneighbors = GetNeighbors();
  deque<Node*> neighbors = *Pneighbors;
  for (deque<Node*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
    if ((*it)->number == nextHop) {
	  return new Node(*(*it));
	}
  }
}

Table *Node::GetRoutingTable() const
{
  cout<<"[GetRoutingTable]"<<endl;
  Table& out = * new Table(0);
  out = table;
  cout<<out<<endl;
  cout<<"[GetRoutingTable]"<<endl;
  return &out;

}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

Node::Node(const unsigned n, SimulationContext *c, double b, double l) :number(n), context(c), table(n)
{
  //cout<<"ncons:"<<n<<endl;
}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), table(rhs.number) {}
  
Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}


void Node::LinkHasBeenUpdated(const Link *l)
{
  cout<<"[LinkHasBeenUp:]---------"<<endl;
  // update our table
  //
  unsigned src = l->GetSrc();
  unsigned dest = l->GetDest();
  double lat = l->GetLatency();
  table.ChangeLink(src, dest, lat);

  cerr << "Post Link change, prior update table\n";
  cerr << table <<endl;  

  // send out routing mesages
  //
  if (table.updateLat()) {
  RoutingMessage *msg = new RoutingMessage(table.SendMyVector(), src);
  
  context->SendToNeighbors(this, msg);
    cout<<"[LinkHasBeenUp]:postEvent"<<endl;
  }
  else{
	  cerr << "Nothing is updated\n";
  }
  cerr << *this<<": Link Update: "<<*l<<endl;
  cout<<table<<endl;

}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cout<<"[ProcesIncomingRM]:begin"<<endl;
  cout<<"table before updated"<<'\n'<<table<<endl;
  map<int, Table::cell> v = m->myVector;
  unsigned src = m->srcNum;
  //
  //if changed, send further message.
  //
  cout<<"from: "<<src<<" to me:"<<number<<endl; 
  
  table.AddNeighborVector(v, src);
  if (table.updateLat()) {
    RoutingMessage *msg = new RoutingMessage(table.SendMyVector(), number);
    context->SendToNeighbors(this, msg);
    cout<<table<<endl;
    cout<<"table has been updated."<<endl;
    cout<<table<<endl;
  }else{
    cerr << "Nothing updated\n";
  }

  cerr<<"[ProcessIncomingRM]:end"<<endl;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination) const {
  unsigned destNumber = destination->number;
  unsigned nextHop = table.GetNextHop(number, destNumber);
  deque<Node*> *Pneighbors = GetNeighbors();
  deque<Node*> neighbors = *Pneighbors;
  for (deque<Node*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
    cout<<(*it)->number<<'n'<<nextHop<<endl;
    if ((*it)->number == nextHop) {
	  return new Node(*(*it));
	}
  }
}

Table *Node::GetRoutingTable() const {
  cout<<"[GetRoutingTable]"<<endl;
  Table& out = * new Table(0);
  out = table;
  cout<<out<<endl;
  cout<<"[GetRoutingTable]"<<endl;
  return &out;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  return os;
}
#endif
