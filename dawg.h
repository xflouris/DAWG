#ifndef DAWG_H
#define DAWG_H

#define         ASCII_SIZE              256
#define         SOLID                   1
#define         NON_SOLID               2

struct TEdge
 {
   struct TState              * from;
   struct TState              * to;
   int                          type;
   struct TIntList            * data;
 };

struct TState
 {
   struct TEdge              ** edges;
   struct TState              * suf_link;
   int                          id;
 };

struct TIntList
 {
   int                          x;
   struct TIntList            * Next; 
 };

struct TStateList
 {
   struct TState              * state;
   struct TStateList          * Next;
 };

int ascii[ASCII_SIZE];
int inv_ascii[ASCII_SIZE];
int dawg_states;

int read_alphabet ( const char * );
#endif
