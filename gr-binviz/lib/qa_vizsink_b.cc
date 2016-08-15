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

#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include <binviz/vizsink_b.h>
#include <ctime>

#include "qa_vizsink_b.h"
#include "binimg.h"

#include "CImg.h"

namespace gr {
    namespace binviz {

        /**
         * @brief qa_vizsink_b::t1 mainly aims to check simple
         * functionality such as creating an image and switching pixels.
         * The function will create a sequence of a white, black, white,
         * black pixels in the upper left corner on a gray background.
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t1()
        {
            int counter = 0;
            int width = 50;
            int height = (width * 1.414);
            bool onoff = true;

            BinImg img(width, height);

            for (int i=0; i<4; i++) {

                if (onoff) {
                    img.on(counter++);
                }
                else {
                    img.off(counter++);
                }

                onoff = !onoff;
                sleep(1);
            }

            img.on(width-1,height-1);
            img.wait();
        }

        /**
         * @brief qa_vizsink_b::t2 aims to check the consume method.
         * Basically, whether feedeing of unsigned chars into the image
         * works properly. It will display a sequence of 8 white n' black
         * pixels in the upper left corner.
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t2()
        {
            unsigned char test_in = 0b10101010;
            int width = 50;
            int height = (width * 1.414);

            BinImg img(width, height);
            sleep(2);
            img.consume(test_in);
            sleep(2);
            img.wait();
        }

        /**
         * @brief qa_vizsink_b::t3 tests whether start detection works
         * properly. It will display all bits but start new lines with
         * each occurence of the start pattern white, black, white black.
         * Apart from the first line there shouldn't be any lines starting
         * with something else. The test will require user interaction
         * (mouse move, click or key press) for each of the bytes consumed.
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t3()
        {
            BinImg img(50,70, "1010");

            img.consume(0b00000011);
            img.wait();

            sleep(1);
            img.consume(0b10101011);
            img.wait();

            sleep(1);
            img.consume(0b00000011);
            img.wait();

            sleep(1);
            img.consume(0b10101011);
            img.wait();

            sleep(1);
            img.consume(0b00010101);
            img.wait();

            sleep(1);
            img.consume(0b11111111);
            img.wait();

            sleep(1);
            img.flush();
            img.wait();
        }

        /**
         * @brief qa_vizsink_b::t4 tests whether end detection works
         * properly. Moreover, it tests whole image wrapping and
         * overriding of lines
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t4()
        {
            BinImg img(50,5, "", "1111");

            img.consume(0b11110001);
            img.wait();

            sleep(1);
            img.consume(0b10110001);
            img.wait();

            sleep(1);
            img.consume(0b00111101);
            img.wait();

            sleep(1);
            img.consume(0b11100001);
            img.wait();

            sleep(1);
            img.consume(0b11110001);
            img.wait();

            sleep(1);
            img.consume(0b10110001);
            img.wait();

            sleep(1);
            img.consume(0b00111101);
            img.wait();

            sleep(1);
            img.consume(0b11100001);
            img.wait();
            sleep(1);
            img.consume(0b11110001);
            img.wait();

            sleep(1);
            img.consume(0b10110001);
            img.wait();

            sleep(1);
            img.flush();
            img.wait();
        }

        /**
         * @brief qa_vizsink_b::t5 check functionality when start and end
         * patterns are defined. The image should wrap to new a line on
         * occurence of the start pattern and should ommit any bits in
         * between the end pattern and the start pattern. On top, the
         * function does some limited performance testing.
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t5()
        {
            time_t now = time(0);
            char* dt = ctime(&now);
            std::cout << dt;

            BinImg img(50,10, "10101010", "1111");
            img.consume(0b00001010);
            img.consume(0b00001010);
            sleep(1);

            img.consume(0b10101001);
            img.consume(0b10010110);
            sleep(1);

            img.consume(0b01100111);
            img.consume(0b10010100);
            sleep(1);

            img.consume(0b00101010);
            img.consume(0b10101001);
            sleep(1);

            img.consume(0b10010110);
            img.consume(0b01100111);
            sleep(1);

            img.consume(0b10010100);
            img.consume(0b10010100);
            sleep(1);
            img.wait();

            now = time(0);
            dt = ctime(&now);
            std::cout << dt;

            now = time(0);
            dt = ctime(&now);
            std::cout << dt;
        }

        /**
         * @brief qa_vizsink_b::t6 aims to check the drop pattern
         * functionality which should be applied recursively and only if
         * not both, the start and stop pattern, are defined
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t6()
        {
            /**
             * @brief img should display 5 pixels (b,w,b,w,b)
             */
            BinImg img(50,10, "", "", "000");
            img.consume(0b00001010);
            img.consume(0b00000010);
            img.flush();
            sleep(1);
            img.wait();

            /**
             * @brief img2 should display 3 lines. First line empty,
             * second line (b,w,b,w), third line (b,w,b,b,w,w,b,b,ww)
             */
            BinImg img2(50,10, "010", "", "000");
            img2.consume(0b00001010);
            img2.consume(0b00000001);
            img2.consume(0b00110011);
            img2.flush();
            sleep(1);
            img2.wait();

            /**
             * @brief img3 should display 3 lines. First line (b,w,b),
             * second line (w,b,b,w,b), third line (b,w,w,b,b,w,w)
             */
            BinImg img3(50,10, "", "010", "000");
            img3.consume(0b00001010);
            img3.consume(0b00000001);
            img3.consume(0b00110011);
            img3.flush();
            sleep(1);
            img3.wait();

            /**
             * @brief img4 single line displayed where 0 is b and 1 is w.
             * This should acutually not drop any 000 sequences
             */
            BinImg img4(50,10, "010010101", "1111", "000");
            img4.consume(0b00001010);
            img4.consume(0b00000001);
            img4.consume(0b00110011);
            img4.flush();
            sleep(1);
            img4.wait();
        }

        /**
         * @brief qa_vizsink_b::t7 aims to check the drop pattern that
         * should not end in an endless loop while removing all occurences.
         *
         * Note, there will be two dummy pixels in the lower right corner.
         */
        void
        qa_vizsink_b::t7()
        {
            /**
             * There should be 4 pixels (w, b, b, b). If the display hangs
             * it's likely an off-by-one bug in the remove/drop functions
             * which causes the pattern to be detected over and over again
             * but got never removed due to wrong bounds checking.
             */
            BinImg img(50,10, "", "", "0000");
            img.consume(0b00000000);
            img.consume(0b10000000);
            img.flush();
            sleep(1);
            img.wait();
        }
    } /* namespace binviz */
} /* namespace gr */
