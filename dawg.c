#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dawg.h"
#include "args.h"

/** 
     fills in the ascii table with indexes for each letter of the alphabet used
     for building the input string s. Returns the number of unique symbols.
**/
int read_alphabet ( const char * s )
 {
   unsigned int         i;
   unsigned int         n;
   int                  nr_trans = 0;

   n = strlen ( s );

   for ( i = 0; i < ASCII_SIZE; ++ i )
    {
      ascii[i] = -1;
    }

   for ( i = 0; i < n; ++ i )
    {
      if ( ascii[ ( int ) s[i]] == -1 ) ascii[ ( int ) s[i]] = nr_trans ++;
    }

   for ( i = 0; i < ASCII_SIZE; ++ i )
    {
      if ( ascii[i] != -1 ) inv_ascii[ascii[i]] = i;
    }

   return ( nr_trans );
 }

void insert_data ( struct TState * state, int elm )
 {
   struct TIntList            * data;

   if ( state -> data && state -> data -> x == elm ) return;

   data = ( struct TIntList * ) malloc ( sizeof ( struct TIntList ) );
   data -> x    = elm;
   data -> Next = state -> data;
   
   state -> data = data;
 }

/**
     create a new a transition  between states from and to having type SOLID or 
     NON_SOLID.
**/
void create_edge ( struct TState * from, struct TState * to, int type, int a )
 {
   struct TEdge               * edge;

   edge = ( struct TEdge * ) malloc ( sizeof ( struct TEdge ) );

   edge -> from = from;
   edge -> to   = to;
   edge -> type = type;

   from -> edges[ascii[a]] = edge;
 }

/**
     create a new state and make a deep copy of the provided edges (transitions).
**/
struct TState * create_state ( struct TEdge ** edges, int alpha_size, int index )
 {
   struct TState              * state;
   static int                   id = 0;
   int                          i;

   state             = ( struct TState * ) malloc ( sizeof ( struct TState ) );
   state -> id       = id ++;
   state -> suf_link = NULL;
   state -> data     = NULL;
   state -> index    = index;
   state -> edges    = ( struct TEdge ** ) calloc ( alpha_size, sizeof ( struct TState * ) );
   if ( edges )
    {
      for ( i = 0; i < alpha_size; ++ i ) 
       {
         if ( edges[i] )
          {
            create_edge ( state, edges[i] -> to, edges[i] -> type, inv_ascii[i] );
          }
       }
    }
   dawg_states = id;

   return ( state );
 }

/**
     find the specific a-transition between states from and to.
**/
struct TEdge * find_edge ( struct TState * from, struct TState * to, int a )
 {
   if ( from -> edges[ascii[a]] && from -> edges[ascii[a]] -> to == to )
     return ( from -> edges[ascii[a]] );

   return ( NULL );
 }

/**
     Construct a directed acyclic word graph from the string s, and store
     the alphabet size in alpha_size.
**/
struct TState * create_dawg ( const char * s, int * alpha_size )
 {
   struct TState              * root;
   struct TState              * sink;
   struct TState              * new_sink;
   struct TState              * new_node;
   struct TState              * w;
   struct TState              * v;
   struct TEdge               * edge;
   int                          i, j;
   int                          n;
   int                          a;

   n           = strlen ( s );
   *alpha_size = read_alphabet ( s );
   dawg_states = 0;

   /* create the root state of the DAWG */
   root = create_state ( NULL, *alpha_size, 0 );

   sink             = root;
   root -> suf_link = NULL;

   /* on-line construct the DAWG */
   for ( i = 0; i < n; ++ i )
    {
      a = ( int ) s[i];
      new_sink = create_state ( NULL, *alpha_size, i + 1 );
      create_edge ( sink, new_sink, SOLID, a );
      insert_data ( new_sink, i + 1 );

      w = sink -> suf_link;
      while ( w && ! w -> edges[ascii[a]] )
       {
         create_edge ( w, new_sink, NON_SOLID, a );
         w = w -> suf_link;
       }

      v = ( w && w -> edges[ascii[a]] ) ? w -> edges[ascii[a]] -> to : NULL;
      if ( ! w ) 
       {
         new_sink -> suf_link = root;
       }
      else
       {
         if ( v )
          {
            edge = find_edge ( w, v, a );
            if ( edge -> type == SOLID )
             {
               new_sink -> suf_link = v;
             }
            else
             {
               new_node = create_state ( v -> edges, *alpha_size, 0 );

               for ( j = 0; j < *alpha_size; ++ j )
                 if ( new_node -> edges[j] ) new_node -> edges[j] -> type = NON_SOLID;

               /* change (w,v) into a solid edge (w, new_node) */
               edge -> to   = new_node;
               edge -> type = SOLID;

               new_sink -> suf_link = new_node;
               new_node -> suf_link = v -> suf_link;
               v -> suf_link        = new_node;

               w = w -> suf_link;

               while ( w && ( edge = find_edge ( w, v, a ) ) && edge -> type == NON_SOLID )
                {
                  edge -> to = new_node;
                  w = w -> suf_link;
                }
             }
          }
       }
      sink = new_sink;
    }

   return ( root );
 }

/**
     Print the id of a given state and all the transitions originating from it.
**/
void print_state ( struct TState * state, int alpha_size )
 {
   int                          i;

   printf ( " %5d:", state -> id );
   for ( i = 0; i < alpha_size; ++ i )
    {
      if ( state -> edges[i] ) printf ( " %c(%d)", inv_ascii[i], state -> edges[i] -> to -> id );
    }
   printf ( "\n" );
 }

/**
     Enqueue all the states of a DAWG in a breadth-first-search manner given its
     root state.
**/
struct TStateList * bfs_enqueue_states ( struct TState * state, int alpha_size )
 {
   struct TStateList          * root;
   struct TStateList          * current;
   struct TStateList          * tail;
   struct TStateList          * Tmp;
   int                        * arr;
   int                          i;

   arr = ( int * ) calloc ( dawg_states, sizeof ( int ) );

   root = ( struct TStateList * ) malloc ( sizeof ( struct TStateList ) );
   root -> state = state;
   root -> Next  = NULL;

   tail = current = root;
   
   while ( current )
    {
      for ( i = 0; i < alpha_size; ++ i )
       {
         if ( current -> state -> edges[i] && ! arr[ current -> state -> edges[i] -> to -> id ])
          {
            Tmp = ( struct TStateList * ) malloc ( sizeof ( struct TStateList ) );

            Tmp -> state = current -> state -> edges[i] -> to;
            Tmp -> Next  = NULL;
            tail -> Next = Tmp;
            tail = Tmp;
            arr[ current -> state -> edges[i] -> to -> id ] = 1;
          }
       }
      current = current -> Next;
    }

   free ( arr );
   
   return ( root );
 }

void print_state_data ( struct TState * state )
 {
   struct TIntList            * elm;

   printf ( " %5d: [", state -> id );
   elm = state -> data;

   if ( elm ) 
    {
      printf ( "%d", elm -> x );
      elm = elm -> Next;
      while ( elm )
       {
         printf ( ",%d", elm -> x );
         elm = elm -> Next;
       }
    }
   printf ( "]\n" );
 }

void dawg_index ( const char * s, struct TState * root )
 {
   int                          n;
   int                          i;
   struct TState              * current;
   struct TState              * suf_link;

   n = strlen ( s );

   current = root;
   for ( i = 0; i < n; ++ i )
    {
      current = current -> edges[ascii[ ( int ) s[i]]]  -> to;
      insert_data ( current, current -> index );
      suf_link = current -> suf_link;

      while ( suf_link != root )
       {
         insert_data ( suf_link, current -> index );
         suf_link = suf_link -> suf_link;
       }
    }
 }

/**
     Given the root state, print a constructed DAWG in a breadth-first-search manner.
**/
void print_dawg ( struct TState * state, int alpha_size, struct TArgs * args )
 {
   struct TStateList          * root;
   struct TStateList          * Tmp;

   root = bfs_enqueue_states ( state, alpha_size );

   printf ( "States:\n" );
   Tmp = root;
   while ( Tmp )
    {
      print_state ( Tmp -> state, alpha_size );
      Tmp = Tmp -> Next;
    }

   if ( args -> index )
    {
      printf ( "\nD-subsets:\n" );
      Tmp = root;
      while ( Tmp )
       {
         print_state_data ( Tmp -> state );
         Tmp = Tmp -> Next;
       }
    }

   if ( args -> suf_link )
    {
      printf ( "\nSuffix links:\n" );
      Tmp = root;
      while ( Tmp )
       {
         printf ( " %5d: ", Tmp -> state -> id );
         if ( Tmp -> state -> suf_link ) 
           printf ( "%d\n", Tmp -> state -> suf_link -> id );
         else
           printf ( "nil\n" );
         Tmp = Tmp -> Next;
       }
    }

   /* deallocate the queue */
   while ( root )
    {
      Tmp = root;
      root = root -> Next;
      free ( Tmp );
    }
 }


/**
     Deallocate the states and edges of a DAWG given its root state.
**/
void dawg_dealloc ( struct TState * state, int alpha_size )
 {
   struct TStateList          * root;
   struct TStateList          * Tmp;
   struct TIntList            * elm;
   int                          i;

   root = bfs_enqueue_states ( state, alpha_size );

   while ( root )
    {
      Tmp = root;
      root = root -> Next;
      for ( i = 0; i < alpha_size; ++ i )
       {
         if ( Tmp -> state -> edges[i] ) free ( Tmp -> state -> edges[i] );
       }
      free ( Tmp -> state -> edges );
      
      while ( Tmp -> state -> data )
       {
         elm = Tmp -> state -> data;
         Tmp -> state -> data = Tmp -> state -> data -> Next;
         free ( elm );
       }
      free ( Tmp -> state );
      free ( Tmp );
    }
 }

void usage ( void )
 {
   printf ( "Usage: dawg <options>\n" );
   printf ( "Standard (Mandatory):\n" );
   printf ( "  -t, --text               <str>           Input text string.\n" );
   printf ( "Optional:\n" );
   printf ( "  -i, --index                              Index the text string, i.e. the nodes of\n"
            "                                           the states of the resulting DAWG will contain\n"
            "                                           the ending positions at which the string resulting\n"
            "                                           by following the path leading to that state\n"
            "                                           occurs in the input text string\n"
            "  -s, --suffix-links                       Output suffix links\n\n" );
 }

int main ( int argc, char * argv [] )
 {
   struct TState              * root;
   int                          alpha_size;
   int                          i;
   struct TArgs                 args;

   i = decode_switches ( argc, argv, &args );

   if ( i < 3 || ! args . text )
    {
      usage ();
      return ( EXIT_FAILURE );
    }
   
   //root = create_dawg ( "abbaba", &alpha_size );
   printf ( "Input text: %s\n", args . text );
   printf ( "Input text size: %d\n", strlen ( args . text ) ); 

   root = create_dawg ( args . text, &alpha_size );
   if ( args . index )  dawg_index ( args . text, root );
   print_dawg ( root, alpha_size, &args );

   dawg_dealloc ( root, alpha_size );

   return ( EXIT_SUCCESS );
 }

