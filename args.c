#include <stdio.h>
#include <getopt.h>
#include "args.h"

static struct option long_options[] =
 {
   { "text", required_argument, NULL, 't' },
   { NULL,     0,               NULL, 0   }
 };

int decode_switches ( int argc, char * argv [], struct TArgs * args )
 {
   int                  oi; 
   int                  opt;

   args -> text = NULL;

   while ( ( opt = getopt_long ( argc, argv, "t:", long_options, &oi ) ) != -1 )
    {
      switch ( opt )
       {
         case 't':
           args -> text = optarg;
           break;
       }
    }

   return ( optind );
 }
