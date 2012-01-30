#include <stdio.h>
#include <getopt.h>
#include "args.h"

static struct option long_options[] =
 {
   { "text",         required_argument, NULL, 't' },
   { "index",        no_argument,       NULL, 'i' },
   { "suffix-links", no_argument,       NULL, 's' },
   { NULL,           0,                 NULL, 0   }
 };

int decode_switches ( int argc, char * argv [], struct TArgs * args )
 {
   int                  oi; 
   int                  opt;

   args -> text     = NULL;
   args -> index    = 0;
   args -> suf_link = 0;

   while ( ( opt = getopt_long ( argc, argv, "t:is", long_options, &oi ) ) != -1 )
    {
      switch ( opt )
       {
         case 't':
           args -> text = optarg;
           break;
         case 'i':
           args -> index = 1;
           break;
         case 's':
           args -> suf_link = 1;
           break;
       }
    }

   return ( optind );
 }
