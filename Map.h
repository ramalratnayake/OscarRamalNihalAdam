// Map.h ... interface to Map data type

#ifndef MAP_H
#define MAP_H

#include "Places.h"

typedef struct edge{
    LocationID  start;
    LocationID  end;
    TransportID type;
} Edge;

// graph representation is hidden 
typedef struct MapRep *Map; 

// operations on graphs 
Map  newMap();  
void disposeMap(Map g); 
void showMap(Map g); 
int  numV(Map g);
int  numE(Map g, TransportID t);
int *reachableLocations(Map g ,int *length, int from, int drac, int railLength, int road, int sea);


typedef struct QueueRep *Queue;

Queue newQueue(); // create new empty queue
void dropQueue(Queue); // free memory used by queue
void showQueue(Queue); // display as 3 > 5 > 4 > ...
void QueueJoin(Queue,int); // add item on queue
int QueueLeave(Queue); // remove item from queue
int QueueIsEmpty(Queue); // check for no items


int shortestPath(Map g, int src, int dest, int *path,int railLength, int drac,int road,int boat);
#endif
