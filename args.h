#ifndef ARGS_H
#define ARGS_H

struct TArgs
 {
   char               * text;
   int                  index;
   int                  suf_link;
 };

int decode_switches ( int argc, char * argv [], struct TArgs * args );

#endif
