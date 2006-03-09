/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

//#include <geos/geosAlgorithm.h>
#include <geos/algorithm/SimplePointInRing.h>
#include <geos/algorithm/CGAlgorithms.h>
#include <geos/geom/LinearRing.h>

//using namespace geos::geom;

// Forward declarations
namespace geos {
	namespace geom {
		class Coordinate;
	}
}

namespace geos {
namespace algorithm { // geos.algorithm

SimplePointInRing::SimplePointInRing(geom::LinearRing *ring){
	pts=ring->getCoordinatesRO();
}

SimplePointInRing::~SimplePointInRing(){
}

bool SimplePointInRing::isInside(const geom::Coordinate& pt){
	return CGAlgorithms::isPointInRing(pt,pts);
}

} // namespace geos.algorithm
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.17  2006/03/09 16:46:46  strk
 * geos::geom namespace definition, first pass at headers split
 *
 * Revision 1.16  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.15  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.14  2004/03/17 02:00:33  ybychkov
 * "Algorithm" upgraded to JTS 1.4
 *
 * Revision 1.13  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.12  2003/10/16 08:50:00  strk
 * Memory leak fixes. Improved performance by mean of more calls to 
 * new getCoordinatesRO() when applicable.
 *
 **********************************************************************/

