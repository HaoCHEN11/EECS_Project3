#ifndef _table
#define _table


#include <iostream>

using namespace std;

#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
#include "link.h"
#include <deque>
#include <map>
#include <set>

class Table {
  public:
    unsigned myNumber;

    // cell struct to store latency and nest hop info
    struct cell 
    {
      unsigned nextHop;
      double latency;
    };

    map<int, cell>costTable; 

    deque<Link>linkedList;

    //------------methods---------------
    Table();
    Table(unsigned n);
    ~Table();
    // Students should write this class

    ostream & Print(ostream &os) const;

    void AddLink(const Link* l);

    void update();

    map<int, cell> *  Dijkstra(deque<Link> queue, int me );

    bool isNew(const Link* l) const;

    int getNextHop(int dest) const;
};
#endif

#if defined(DISTANCEVECTOR)

#include <deque>
#include <map>

#define INFINITY_LAT 1000000
#define NO_FROM 100
class Table {
 public:
   unsigned myNumber;
 
   // cell struct to store latency and nest hop info
   struct cell 
   {
     unsigned nextHop;
     double latency;
   };
   map<int, map<int, cell> >table;

   map<int, int>neighborVect; //first: neighbor number, second: distance.
   //
     int getNeighborLatency(int neighbor) const;
     void setNeighborLatency(int nei, double lat); 
     //----need to check----14---
     
   
   
   //

 public:
   Table(); 
   Table(unsigned n);
   ~Table();

   ostream & Print(ostream &os) const;

   //set and get my number, latency and nextHop
   void SetNumber(unsigned n);
   unsigned GetNumber()const;

   void SetLatency(int row, int column, double lat);
   double  GetLatency(int row, int column)const;

   void SetNextHop(int row, int column, unsigned next);
   unsigned GetNextHop(int row, int column)const;

   /*Add link and node*/
   void ChangeLink(int src, int dest, double lat);
   void AddNeighborVector(map<int, cell> neighborVector, int neighborNum);

   map<int, Table::cell> SendMyVector();

   /*------------private------------*/
   map<int, Table::cell>::iterator FindCellIt(int row, int column)const;
   bool isExist(int row, int column)const;
   bool isNeighbor(int num)const;

   bool SetMiniLat(map<int, cell>::iterator myIt);
   bool updateLat();


  //ostream & Print(ostream &os) const;
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
