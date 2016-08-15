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


#ifndef _QA_VIZSINK_S_H_
#define _QA_VIZSINK_S_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

namespace gr {
  namespace binviz {

    class qa_vizsink_b : public CppUnit::TestCase
    {
    public:
      CPPUNIT_TEST_SUITE(qa_vizsink_b);
      CPPUNIT_TEST(t1);
      CPPUNIT_TEST(t2);
      CPPUNIT_TEST(t3);
      CPPUNIT_TEST(t4);
      CPPUNIT_TEST(t5);
      CPPUNIT_TEST(t6);
      CPPUNIT_TEST(t7);
      CPPUNIT_TEST_SUITE_END();

    private:
      void t1();
      void t2();
      void t3();
      void t4();
      void t5();
      void t6();
      void t7();
    };

  } /* namespace binviz */
} /* namespace gr */

#endif /* _QA_VIZSINK_S_H_ */

