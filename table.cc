#include "table.h"

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
#define INFI 1000000
#define UNDEFINED -1

Table::Table() {}
Table::Table(unsigned n) {
  myNumber = n;
  cout<<"consTable: "<<n<<endl;
}

Table::~Table() {}
ostream & Table::Print(ostream &os) const {
  os<<"Table()";
  return os;
}

bool Table::isNew(const Link* l) const {
  for(deque<Link>::const_iterator it = linkedList.begin(); it != linkedList.end(); it++){
     if(it->GetSrc() == l->GetSrc() && it->GetDest() == l->GetDest() && it->GetLatency() == l->GetLatency()){
       return false;
     }
  }
  return true;
}


void Table::AddLink(const Link* l) {
  if(isNew(l)) {
    Link *link = new Link(*l);
    linkedList.push_back(*link);
  }
}

int Table::getNextHop(int dest) const{
  map<int, cell>::const_iterator it = costTable.find(dest);
  if(it == costTable.end()){
    return -1;
  } else{
    return it->second.nextHop;
  } 
}

void Table::update() {
  costTable = *(Dijkstra(linkedList, myNumber));
}

map<int, Table::cell> * Table::Dijkstra(deque<Link> queue, int me) {
  cerr << "Dijkstra start i am " << me << "\n";
  map<int, map<int, int> > bitmap;
  set<int> vertex;
  for(deque<Link>::iterator it = queue.begin(); it != queue.end(); it++){
    int src = it->GetSrc();
    int dst = it->GetDest();
    int lat = it->GetLatency();
    cerr << "Add from queue to bit map " << src << " to " << dst << "\n";
    bitmap[src][dst] = lat;
    vertex.insert(src);
    vertex.insert(dst);
  }
  
  // Dijkstra algorithm
  map<int, int> distance;
  map<int, int> previous;
  map<int, int> next;
  set<int> q;
  
  for(set<int>::iterator it = vertex.begin(); it != vertex.end(); it++){
    distance[*it] = INFI;
    previous[*it] = UNDEFINED;
  }

  distance[me] = 0;
  previous[me] = me;

  for(set<int>::iterator it = vertex.begin(); it != vertex.end(); it++){
    q.insert(*it);
  }

  while(q.size() > 0){
    cerr << "Queue size" << q.size() << "\n";

    // Find the shortest distance in set q
    int min_dis = distance[*(q.begin())];
    set<int>::iterator min_it = q.begin();
    int min_v;
    for(set<int>::iterator it = q.begin(); it != q.end(); it++){
      if(distance[*it] < min_dis){
         min_dis = distance[*it];
	 min_it = it;
      }
    }
    min_v = *min_it;

    if(min_dis == INFI){
      cerr << "Bad thing happens\n";
      break;
    }

    cerr << "We found shortest distance in q" << min_v << "\n";
    
    if(bitmap.find(min_v) == bitmap.end()){
      // No neighbours for the node min_v
      cerr << "No neighbour for " << min_v << "\n";
      cerr << "Neighbour size for 1 " << bitmap.find(min_v)->second.size() << "\n";
    } else{ 
      for(map<int, int>::iterator it = bitmap.find(min_v)->second.begin(); it != bitmap.find(min_v)->second.end() ; it++){
        // all neighbors of min_v are here
        int alt = distance[min_v] + it->second;
        if(alt < distance[it->first]){
	  distance[it->first] = alt;
	  previous[it->first] = min_v;
        }
      }
    }
    cerr << "Now to erase" << min_v << "\n";
    q.erase(min_it);
  }

  // Post Dijkstra 
  int end, tail;
  for(set<int>::iterator it = vertex.begin(); it != vertex.end(); it++){
    tail = *it;
    end = *it;
    while(previous[end] != me){
      end = previous[end];
    }
    
    next[tail] = end;
  }

  map<int, cell> * r = new map<int, cell>;  
  for(set<int>::iterator it = vertex.begin(); it != vertex.end(); it++){
    cell c;
    c.nextHop = next[*it];
    c.latency = distance[*it];
    r->insert(pair<int, cell> (*it, c));
  }
  cerr << "Dijkstra finish\n";
  return r; 
}

#endif

#if defined(DISTANCEVECTOR)

Table::Table() {}
Table::Table(unsigned n) {
  myNumber = n;
  neighborVect[myNumber]= 0;//-----------------------14--------
  SetLatency(n,n,0);
  SetNextHop(n,n,n);//n means from self.
}
Table::~Table() {}


ostream & Table::Print(ostream &os) const {
 for (map<int, map<int, cell> >::const_iterator iT = table.begin(); iT != table.end(); ++iT) {
   for (map<int, cell>::const_iterator it = iT->second.begin(); it != iT->second.end(); ++it) {
     os <<'('<<iT->first<<'|'<<it->first<<")(lat:"<<it->second.latency<<'|'<<"next:"<<it->second.nextHop<<')'<<'\t';
   }
   os<<endl;
 }
 os<<"Table()";
 return os;
}

//-------------get, set Latency, From, Number----------
void Table::SetNumber(unsigned n) {myNumber = n;}
unsigned Table::GetNumber()const {return myNumber;}

void Table::SetLatency(int row, int column, double lat) { 
  table[row][column].latency = lat;
}
double Table::GetLatency(int row, int column)const {
  if (isExist(row, column)) {
    map<int, cell>::iterator i = FindCellIt(row, column);
	return i->second.latency;
  }
  else cout <<"router not found."<<endl;
}

void Table::SetNextHop(int row, int column, unsigned next) {
  table[row][column].nextHop = next;
}
unsigned Table::GetNextHop(int row, int column)const {
  if (isExist(row, column)) {
    map<int, cell>::iterator i = FindCellIt(row, column);
	return i->second.nextHop;
  }
  else {
    cout<<"[NextHOP]I don`t know who can reach me."<<endl;
    return NO_FROM;
  }
}
//-------------14----------
void  Table::ChangeLink(int src, int dest, double lat) {
  if (isExist(src, dest)) {
    setNeighborLatency(dest, lat);
  }
  else{
    SetLatency(dest, dest, 0);
    SetNextHop(dest,dest, dest);
    SetLatency(src, dest, INFINITY_LAT);
    setNeighborLatency(dest, lat);
  }
}

void Table::AddNeighborVector(map<int, cell> neighborVector, int neighborNum) {
  
  //print for test;
  
  cout<<"Print arrived vector from:"<<neighborNum<<"to:"<<myNumber<<"size:"<<neighborVector.size()<<endl;
  for (map<int,cell>::iterator iP = neighborVector.begin(); iP != neighborVector.end(); ++iP) {
    
    cout<<"(from:"<<myNumber<<" to: "<<iP->first<<")(lat:"<<iP->second.latency<<"|next"<<iP->second.nextHop<<')'<<endl;
  }
  
  //-----start------
  for (map<int, cell>::iterator it = neighborVector.begin(); it != neighborVector.end(); ++it) {
    unsigned column = it->first;
    double latOfNei = it->second.latency;
    unsigned nextOfNei = it->second.nextHop; //extract the info of each node.
    if (!isExist(myNumber, column)) {
      SetLatency(myNumber, column, INFINITY_LAT);
    }
    SetLatency(neighborNum, column, latOfNei);
    SetNextHop(neighborNum, column, nextOfNei); //-----modified-----single step, so only 
  }
}
//-------------------14---------

map<int, Table::cell> Table::SendMyVector() {return table[myNumber];}

// FindCellIt should be used after isExist.
map<int, Table::cell>::iterator Table::FindCellIt(int row, int column)const {
  map<int, map<int, cell> >::const_iterator iT = table.find(row);
  map<int, cell>::const_iterator outIt = iT->second.find(column);
}

bool Table::isExist(int row, int column)const {
  map<int, map<int, cell> >::const_iterator iT = table.find(row);
  if (iT == table.end()) return false;
  else {
    map<int, cell>::const_iterator i = iT->second.find(column);
	if (i == iT->second.end()) return false;
	else return true;
  }
}
bool Table::isNeighbor(int num)const {
  map<int, map<int, cell> >::const_iterator iT = table.find(num);
  if (iT == table.end()) return false;
  else return true;
}

bool Table::SetMiniLat(map<int, cell>::iterator myIt) {
  bool isUpdated = false;
  double oldLat = myIt->second.latency;
  double newLat = INFINITY_LAT;
  unsigned next = myIt->second.nextHop;
  unsigned destNode = myIt->first;
  if (destNode == myNumber) {return isUpdated;}
  for (map<int, map<int, cell> >::iterator iT = table.begin(); iT != table.end(); ++iT) {
    unsigned neiNum = iT->first;
    if (neiNum != myNumber) {
      double meToNeighborLat = getNeighborLatency(neiNum);
      double neighborToDestLat = GetLatency(neiNum, destNode);
      double tmpLat = meToNeighborLat + neighborToDestLat;
      if (tmpLat < newLat) {
        newLat = tmpLat;
	next = neiNum;
      }  
    }
  }
  if (newLat != oldLat) isUpdated = true;
  SetLatency(myNumber, destNode, newLat);
  SetNextHop(myNumber, destNode, next);
  return isUpdated;
}

bool Table::updateLat() {
  bool isUpdated = false;
  // check each node in myVector.
  for (map<int, cell>::iterator myIt = table[myNumber].begin(); myIt != table[myNumber].end(); ++myIt) {
    isUpdated = (isUpdated || SetMiniLat(myIt));
  } 
  return isUpdated;
}
/*
void Table::AddRow(int row) {
  for (map<int, cell>::iterator it = table[myNumber].begin(); it != table[myNumber].end(); ++it) {
    unsigned newNumber = it->first;
    // add new element into table;
    SetLatency(row, newNumber, INFINITY_LAT);
    SetNextHop(row, newNumber, NO_FROM);//--------------modified----------
    cout<<"row("<<row<<'|'<<newNumber<<')';
  }
  cout<<"row:"<<row<<"have is new coming."<<endl;
}
void Table::AddColumn(int column) {
  for (map<int, map<int, cell> >::iterator iT = table.begin(); iT != table.end(); ++iT) {
    unsigned newNumber = iT->first;
    SetLatency(newNumber, column, INFINITY_LAT);
    SetNextHop(newNumber, column, NO_FROM);
    cout<<"col:("<<newNumber<<'|'<<column<<')';
  }
  cout<<endl;
}
*/
//--------14------neighborVector------------
int Table::getNeighborLatency(int neighbor) const{
  map<int, int>::const_iterator iv = neighborVect.find(neighbor);
  if(iv == neighborVect.end()) {
    cout<<"is not my neighbor!"<<endl;
    return -1;
  }
  else return iv->second;
}
void Table::setNeighborLatency(int nei, double lat) {
  neighborVect[nei] = lat;
}

//ostream & Table::Print(ostream &os) const {}
#endif
