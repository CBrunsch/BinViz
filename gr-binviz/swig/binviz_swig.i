/* -*- c++ -*- */

#define BINVIZ_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "binviz_swig_doc.i"

%{
#include "binviz/vizsink_b.h"
%}


%include "binviz/vizsink_b.h"
GR_SWIG_BLOCK_MAGIC2(binviz, vizsink_b);
