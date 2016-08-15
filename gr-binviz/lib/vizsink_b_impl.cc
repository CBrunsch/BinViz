/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <gnuradio/io_signature.h>
#include "vizsink_b_impl.h"
#include "binimg.h"


namespace gr {
  namespace binviz {

    vizsink_b::sptr
    vizsink_b::make(int width, int height, const std::string &start_pattern, const std::string &end_pattern, const std::string &drop_pattern, bool skip_zero_bytes)
    {
      return gnuradio::get_initial_sptr
        (new vizsink_b_impl(width, height, start_pattern, end_pattern, drop_pattern, skip_zero_bytes));
    }

    /*
     * The private constructor
     */
    vizsink_b_impl::vizsink_b_impl(int width, int height, const std::string &start_pattern, const std::string &end_pattern, const std::string &drop_pattern, bool skip_zero_bytes)
      : gr::sync_block("vizsink_b",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(0, 0, 0)),
              img(width, height, start_pattern, end_pattern, drop_pattern),
              skip_zero_bytes(skip_zero_bytes)
    {
        // what else?
    }

    /*
     * Our virtual destructor.
     */
    vizsink_b_impl::~vizsink_b_impl()
    {
    }

    int
    vizsink_b_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        int consumed = 0;

        // loop over vector of input items
        for (consumed; consumed < input_items.size(); consumed++) {

          // get input item
          const unsigned char *in = (const unsigned char *) input_items[consumed];

          if (skip_zero_bytes) {
              if (*in != 0) {
                  // process data only if byte was not 0
                  img.consume(*in);
              }
          }
          else {
              // process data in any case
              img.consume(*in);
          }

          // on debug wait for user input (mouse move, key hits
          // img.wait();
        }

        // Tell runtime system how many output items we produced.
        return consumed;
    }

  } /* namespace binviz */
} /* namespace gr */

