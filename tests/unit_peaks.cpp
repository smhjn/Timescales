/** Test unit for peak-finding code
 * @file timescales/tests/unit_peaks.cpp
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified November 25, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of the Timescales library.
 * 
 * The Timescales library is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * The Timescales library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the Timescales library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../common/warnflags.h"

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_COARSEWARN
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

// Boost.Test uses C-style casts and non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Weffc++"
#endif

#include <boost/test/unit_test.hpp>

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

#include <vector>
#include <boost/lexical_cast.hpp>
#include "../timescales.h"
#include "../../common/cerror.h"
#include "../../common/lcio.h"
//#include "test.h"
#include "../../common/alloc.tmp.h"


using std::vector;
using boost::lexical_cast;
using boost::shared_ptr;
using kpfutils::checkAlloc;
using kpfutils::cError;
using kpfutils::fileCheckOpen;

namespace kpftimes { namespace test {

/** This function is a wrapper for a trusted approximate comparison method.
 */
bool isClose(double val1, double val2, double frac);

// Boost.Test uses non-virtual destructors
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

/** Test cases for peak-finding code
 * @class BoostTest::test_peaks
 */
BOOST_AUTO_TEST_SUITE(test_peaks)

/** Tests whether @ref kpftimes::peakFind() "peakFind()" matches 
 *	Ann Marie's original program
 *
 * @see @ref kpftimes::peakFind() "peakFind()"
 *
 * @test Consistent results with original IDL code
 *
 * @exceptsafe Does not throw exceptions
 *
 */
BOOST_AUTO_TEST_CASE(peakfind) {
	for(int i = 0; i <= 13; i++) {
		vector<double> times, mags;
		vector<double> peakTimes, peaks;
		
		try {
			{
				char fileName[80];
				if (sprintf(fileName, "idl_target_in_%i.txt", i) < 0) {
					cError("While testing peakFind(): ");
				}
				
				kpfutils::readMcLightCurve(fileName, times, mags);
			}
	
			{
				char fileName[80];
				if (sprintf(fileName, "idl_target_peak_%i.txt", i) < 0) {
					cError("While testing peakFind(): ");
				}
				
				kpfutils::readMcLightCurve(fileName, peakTimes, peaks);
			}
		} catch (const std::exception& e) {
			BOOST_FAIL(e.what());
		}
		
		vector<double> myTimes, myPeaks;
		BOOST_REQUIRE_NO_THROW(kpftimes::peakFind(times, mags, 0.05, 
			myTimes, myPeaks));
		
		BOOST_REQUIRE_EQUAL(myTimes.size(), peakTimes.size());
		BOOST_REQUIRE_EQUAL(myTimes.size(), myPeaks.size());
		
		// Allow for occasional deviations due to roundoff errors
		unsigned int nBad = 0;
		for(size_t j = 0; j < myTimes.size(); j++) {
			if (!isClose(myTimes[j], peakTimes[j], 1e-5) 
					|| !isClose(myPeaks[j], peaks[j], 1e-5)) {
				nBad++;
			}
		}
		BOOST_WARN(nBad == 0 || nBad >= peakTimes.size()/1000);
		BOOST_CHECK(nBad <= peakTimes.size()/1000);
	}	// end loop over examples
}

BOOST_AUTO_TEST_SUITE_END()

// Re-enable all compiler warnings
#ifdef GNUC_FINEWARN
#pragma GCC diagnostic pop
#endif

}}		// end kpftimes::test
