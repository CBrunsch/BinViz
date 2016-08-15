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


#ifndef INCLUDED_BINVIZ_VIZSINK_S_H
#define INCLUDED_BINVIZ_VIZSINK_S_H

#include <string>
#include <binviz/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace binviz {

    /*!
     * \brief Provides a GUI that visualizes bits
     * \ingroup binviz
     *
     * \details
     * The binary visualizer block provides a GUI component that allows
     * arrangement and display of binary sequences and aims to provide help
     * in reversing and detecting differences between multiple ocurrences
     * of bit streams.
     *
     * Feed the GUI with a decoded stream of 0s and 1s and it will display
     * the relevant bits as white (on) and black (off). Use /p width and
     * /p height to set the dimension of your display. In any case the
     * stream will continue at the upper left corner if it hits the bottom
     * edge.
     *
     * Parameters /p start_pattern, /p end_pattern, /p drop_pattern allow
     * for justification of how streams are displayed and aligned. These
     * parameters take strings composed of 0s and 1s e.g. 01010101 as a
     * preamble or /p start_pattern. The display will start on a new line
     * for each occurence of the /p start_pattern. Moreover, on detection
     * of the /p end_pattern the display will wrap to a new line. In case
     * both, the /p start_pattern and /p end_pattern are defined, the
     * display will drop any out-of-bounds bits and only display streams
     * from /p start_pattern to /p end_pattern on a single line each.
     * Moreover, once the /p start_pattern is being detected additional
     * occurences of the /p start_pattern will be meanless until the
     * /p end_pattern ist detected.
     *
     * To get rid of long sequences of zero bytes or arbitrary unwanted
     * bit sequences set the /p skip_zero_bytes to true or define a string
     * of 0s and 1s for /p drop_pattern to be removed. Note, the params
     * /p drop_pattern and /p skip_zero_bytes have precedence over start
     * and end detection patterns
     *
     * The display itself allows for some semi-live adjustments and manual
     * analyis. E.g. the mouse wheel on the display allows to zoom-in and
     * zoom-out while new bits are being displayed instantly. Once the
     * display is clicked it will stop painting new bits and display a
     * cursor and its x/y-position. In that mode, one could easily count
     * bits or select part of the bitsream for magnification and closer
     * inspection. Press ESC in order to release and let Binviz paint
     * further bits. During inspection bits are not dropped but held and
     * being painted once ESC is hit.
     *
     * Hint: Binviz runs as thread. Thus, closing the Binviz window will
     * not stop GRC but stopping GRC will close the Binviz window. Take
     * your screenshots before.
     */
    class BINVIZ_API vizsink_b : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<vizsink_b> sptr;

      /*!
       * \brief make Creates the Binviz display
       * \param width The width of the display (in dots to be displayed
       * which is not equivalent to pixels, a hard coded resize factor
       * of four applies)
       * \param height The height of the display
       * \param start_pattern A string of 0s and 1s used to detect the
       * start of a bit sequence. The display will wrap to a new line for
       * any occurence of the start sequence. Note, that in case the start
       * and the end pattern are defined, additional occurences of the
       * start pattern right after the start pattern but before an end
       * pattern will be ignored.
       * \param end_pattern A string of 0s and 1s used to detect the end
       * of a bit sequence. The display will wrap to a new line after any
       * occurence of the end sequence.
       * \param drop_pattern A string of 0s and 1s used to be dropped.
       * Dropping the string will have precedence over start and end
       * detection.
       * \param skip_zero_bytes If set to true will cause that any byte
       * compsed of zero's will be ignored. Ignorance of zero bytes will
       * have precedence over start, end and drop detections.
       * \return The number of bytes consumed.
       */
      static sptr make(int width, int height, const std::string &start_pattern = "", const std::string &end_pattern = "", const std::string &drop_pattern = "", bool skip_zero_bytes = false);
    };

  } // namespace binviz
} // namespace gr

#endif /* INCLUDED_BINVIZ_VIZSINK_S_H */

