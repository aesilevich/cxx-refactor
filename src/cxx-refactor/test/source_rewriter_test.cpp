// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_rewriter_test.cpp
/// Contains unit tests for the source_rewriter class.

#include "../source_rewriter.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(source_rewriter_test)


/// Simple rewriter test
BOOST_AUTO_TEST_CASE(simple_test) {
    std::istringstream istr{"test\ninput string\nlast line"};
    std::ostringstream ostr;

    single_source_modifications mods;
    mods.add(source_modification{{{1, 3}, {2, 5}}, "inserted string"});

    source_rewriter rw;
    rw.rewrite(mods, istr, ostr);

    BOOST_CHECK_EQUAL(ostr.str(), "teinserted stringt string\nlast line");
}


BOOST_AUTO_TEST_SUITE_END()
