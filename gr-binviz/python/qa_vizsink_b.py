#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2016 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import binviz_swig as binviz

class qa_vizsink_b (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):

        # setup source data vector
        src_data = (0b00000000, 0b01010101, 0b01010101)

        # setup blocks
        src = blocks.vector_source_b(src_data)
        dst = binviz.vizsink_b(100,100,"","","", True)
        self.tb.connect(src, dst)

        # run the stuff
        self.tb.run ()

        # check data => visually


if __name__ == '__main__':
    gr_unittest.run(qa_vizsink_b, "qa_vizsink_b.xml")
