/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/geom.h>
#include <geos/util.h>
#include <sstream>

using namespace std;

namespace geos {
namespace geom { // geos::geom

IntersectionMatrix::IntersectionMatrix()
{
	//matrix = new int[3][3];
	setAll(Dimension::False);
}

IntersectionMatrix::IntersectionMatrix(const string& elements)
{
	setAll(Dimension::False);
	set(elements);
}

IntersectionMatrix::IntersectionMatrix(const IntersectionMatrix &im)
{
	matrix[Location::INTERIOR][Location::INTERIOR] = im.matrix[Location::INTERIOR][Location::INTERIOR];
	matrix[Location::INTERIOR][Location::BOUNDARY] = im.matrix[Location::INTERIOR][Location::BOUNDARY];
	matrix[Location::INTERIOR][Location::EXTERIOR] = im.matrix[Location::INTERIOR][Location::EXTERIOR];
	matrix[Location::BOUNDARY][Location::INTERIOR] = im.matrix[Location::BOUNDARY][Location::INTERIOR];
	matrix[Location::BOUNDARY][Location::BOUNDARY] = im.matrix[Location::BOUNDARY][Location::BOUNDARY];
	matrix[Location::BOUNDARY][Location::EXTERIOR] = im.matrix[Location::BOUNDARY][Location::EXTERIOR];
	matrix[Location::EXTERIOR][Location::INTERIOR] = im.matrix[Location::EXTERIOR][Location::INTERIOR];
	matrix[Location::EXTERIOR][Location::BOUNDARY] = im.matrix[Location::EXTERIOR][Location::BOUNDARY];
	matrix[Location::EXTERIOR][Location::EXTERIOR] = im.matrix[Location::EXTERIOR][Location::EXTERIOR];
}

/**
 * Adds one matrix to another.
 * Addition is defined by taking the maximum dimension value of each position
 * in the summand matrices.
 *
 * @param im the matrix to add
 */
void
IntersectionMatrix::add(IntersectionMatrix *im)
{
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			setAtLeast(i,j,im->get(i,j));
		}
	}
}


bool
IntersectionMatrix::matches(int actualDimensionValue,
	char requiredDimensionSymbol)
{

	if (requiredDimensionSymbol=='*') return true;

	if (requiredDimensionSymbol=='T' && (actualDimensionValue >= 0 ||
		actualDimensionValue==Dimension::True))
	{
		return true;
	}

	if (requiredDimensionSymbol=='F' &&
		actualDimensionValue==Dimension::False)
	{
		return true;
	}

	if (requiredDimensionSymbol=='0' &&
		actualDimensionValue==Dimension::P)
	{
		return true;
	}

	if (requiredDimensionSymbol=='1' &&
		actualDimensionValue==Dimension::L)
	{
		return true;
	}

	if (requiredDimensionSymbol=='2' &&
		actualDimensionValue==Dimension::A)
	{
		return true;
	}

	return false;
}

bool
IntersectionMatrix::matches(const string& actualDimensionSymbols,
	const string& requiredDimensionSymbols)
{
	IntersectionMatrix m(actualDimensionSymbols);
	bool result=m.matches(requiredDimensionSymbols);
	return result;
}

void
IntersectionMatrix::set(int row, int column, int dimensionValue)
{
	matrix[row][column] = dimensionValue;
}

void
IntersectionMatrix::set(const string& dimensionSymbols)
{
	unsigned int limit = dimensionSymbols.length();

	for (unsigned int i = 0; i < limit; i++)
	{
		int row = i / 3;
		int col = i % 3;
		matrix[row][col] = Dimension::toDimensionValue(dimensionSymbols[i]);
	}
}

void
IntersectionMatrix::setAtLeast(int row, int column, int minimumDimensionValue)
{
	if (matrix[row][column] < minimumDimensionValue)
	{
		matrix[row][column] = minimumDimensionValue;
	}
}

void
IntersectionMatrix::setAtLeastIfValid(int row, int column,
	int minimumDimensionValue)
{
	if (row >= 0 && column >= 0) {
		setAtLeast(row, column, minimumDimensionValue);
	}
}

void
IntersectionMatrix::setAtLeast(string minimumDimensionSymbols)
{

	unsigned int limit = minimumDimensionSymbols.length();

	for (unsigned int i = 0; i < limit; i++)
	{
		int row = i / 3;
		int col = i % 3;
		setAtLeast(row, col, Dimension::toDimensionValue(minimumDimensionSymbols[i]));
	}
}

void
IntersectionMatrix::setAll(int dimensionValue)
{
	for (int ai = 0; ai < 3; ai++) {
		for (int bi = 0; bi < 3; bi++) {
			matrix[ai][bi] = dimensionValue;
		}
	}
}

int
IntersectionMatrix::get(int row, int column)
{
	return matrix[row][column];
}

bool
IntersectionMatrix::isDisjoint()
{
	return
		matrix[Location::INTERIOR][Location::INTERIOR]==Dimension::False
		&&
		matrix[Location::INTERIOR][Location::BOUNDARY]==Dimension::False
		&&
		matrix[Location::BOUNDARY][Location::INTERIOR]==Dimension::False
		&&
		matrix[Location::BOUNDARY][Location::BOUNDARY]==Dimension::False;
}

bool
IntersectionMatrix::isIntersects()
{
	return !isDisjoint();
}

bool
IntersectionMatrix::isTouches(int dimensionOfGeometryA,
	int dimensionOfGeometryB)
{
	if (dimensionOfGeometryA > dimensionOfGeometryB)
	{
		//no need to get transpose because pattern matrix is symmetrical
		return isTouches(dimensionOfGeometryB, dimensionOfGeometryA);
	}
	if ((dimensionOfGeometryA==Dimension::A && dimensionOfGeometryB==Dimension::A)
		||
		(dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::L)
		||
		(dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::A)
		||
		(dimensionOfGeometryA==Dimension::P && dimensionOfGeometryB==Dimension::A)
		||
		(dimensionOfGeometryA==Dimension::P && dimensionOfGeometryB==Dimension::L))
	{
		return matrix[Location::INTERIOR][Location::INTERIOR]==Dimension::False &&
			  (matches(matrix[Location::INTERIOR][Location::BOUNDARY], 'T') ||
			   matches(matrix[Location::BOUNDARY][Location::INTERIOR], 'T') ||
			   matches(matrix[Location::BOUNDARY][Location::BOUNDARY], 'T'));
	}
	return false;
}

bool
IntersectionMatrix::isCrosses(int dimensionOfGeometryA,
	int dimensionOfGeometryB)
{
	if ((dimensionOfGeometryA==Dimension::P && dimensionOfGeometryB==Dimension::L) ||
		(dimensionOfGeometryA==Dimension::P && dimensionOfGeometryB==Dimension::A) ||
		(dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::A)) {
		return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
			   matches(matrix[Location::INTERIOR][Location::EXTERIOR], 'T');
	}
	if ((dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::P) ||
		(dimensionOfGeometryA==Dimension::A && dimensionOfGeometryB==Dimension::P) ||
		(dimensionOfGeometryA==Dimension::A && dimensionOfGeometryB==Dimension::L)) {
		return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
			   matches(matrix[Location::EXTERIOR][Location::INTERIOR], 'T');
	}
	if (dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::L) {
		return matrix[Location::INTERIOR][Location::INTERIOR]==0;
	}
	return false;
}

bool
IntersectionMatrix::isWithin()
{
	return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
		   matrix[Location::INTERIOR][Location::EXTERIOR]==Dimension::False &&
		   matrix[Location::BOUNDARY][Location::EXTERIOR]==Dimension::False;
}

bool
IntersectionMatrix::isContains()
{
	return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
		   matrix[Location::EXTERIOR][Location::INTERIOR]==Dimension::False &&
		   matrix[Location::EXTERIOR][Location::BOUNDARY]==Dimension::False;
}

bool
IntersectionMatrix::isEquals(int dimensionOfGeometryA, int dimensionOfGeometryB)
{
	if (dimensionOfGeometryA != dimensionOfGeometryB) {
		return false;
	}
	return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
		   matrix[Location::EXTERIOR][Location::INTERIOR]==Dimension::False &&
		   matrix[Location::INTERIOR][Location::EXTERIOR]==Dimension::False &&
		   matrix[Location::EXTERIOR][Location::BOUNDARY]==Dimension::False &&
		   matrix[Location::BOUNDARY][Location::EXTERIOR]==Dimension::False;
}

bool
IntersectionMatrix::isOverlaps(int dimensionOfGeometryA,
	int dimensionOfGeometryB)
{
	if ((dimensionOfGeometryA==Dimension::P && dimensionOfGeometryB==Dimension::P) ||
		(dimensionOfGeometryA==Dimension::A && dimensionOfGeometryB==Dimension::A)) {
		return matches(matrix[Location::INTERIOR][Location::INTERIOR], 'T') &&
			   matches(matrix[Location::INTERIOR][Location::EXTERIOR], 'T') &&
			   matches(matrix[Location::EXTERIOR][Location::INTERIOR],'T');
	}
	if (dimensionOfGeometryA==Dimension::L && dimensionOfGeometryB==Dimension::L) {
		return matrix[Location::INTERIOR][Location::INTERIOR]==1 &&
			   matches(matrix[Location::INTERIOR][Location::EXTERIOR], 'T') &&
			   matches(matrix[Location::EXTERIOR][Location::INTERIOR], 'T');
	}
	return false;
}

/*public*/
bool
IntersectionMatrix::matches(const string& requiredDimensionSymbols)
{
	if (requiredDimensionSymbols.length() != 9) {
		ostringstream s;
		s << "IllegalArgumentException: Should be length 9, is "
			<< "[" << requiredDimensionSymbols << "] instead" << endl;
		throw util::IllegalArgumentException(s.str());
	}
	for (int ai = 0; ai < 3; ai++) {
		for (int bi = 0; bi < 3; bi++) {
			if (!matches(matrix[ai][bi],requiredDimensionSymbols[3*ai+bi])) {
				return false;
			}
		}
	}
	return true;
}

//Not sure
IntersectionMatrix*
IntersectionMatrix::transpose()
{
	int temp = matrix[1][0];
	matrix[1][0] = matrix[0][1];
	matrix[0][1] = temp;
	temp = matrix[2][0];
	matrix[2][0] = matrix[0][2];
	matrix[0][2] = temp;
	temp = matrix[2][1];
	matrix[2][1] = matrix[1][2];
	matrix[1][2] = temp;
	return this;
}

string
IntersectionMatrix::toString()
{
	string result("");
	for (int ai = 0; ai < 3; ai++) {
		for (int bi = 0; bi < 3; bi++) {
			result+=Dimension::toDimensionSymbol(matrix[ai][bi]);
		}
	}
	return result;
}

} // namespace geos::geom
} // namespace geos

/**********************************************************************
 * $Log$
 * Revision 1.19  2006/03/09 16:46:47  strk
 * geos::geom namespace definition, first pass at headers split
 *
 * Revision 1.18  2006/03/06 19:40:46  strk
 * geos::util namespace. New GeometryCollection::iterator interface, many cleanups.
 *
 * Revision 1.17  2006/03/03 10:46:21  strk
 * Removed 'using namespace' from headers, added missing headers in .cpp files, removed useless includes in headers (bug#46)
 *
 * Revision 1.16  2006/03/02 11:00:26  strk
 * Changed IntersectionMatrix funx taking strings to take const string& instead
 *
 * Revision 1.15  2006/02/09 15:52:47  strk
 * GEOSException derived from std::exception; always thrown and cought by const ref.
 *
 * Revision 1.14  2006/01/31 19:07:33  strk
 * - Renamed DefaultCoordinateSequence to CoordinateArraySequence.
 * - Moved GetNumGeometries() and GetGeometryN() interfaces
 *   from GeometryCollection to Geometry class.
 * - Added getAt(int pos, Coordinate &to) funtion to CoordinateSequence class.
 * - Reworked automake scripts to produce a static lib for each subdir and
 *   then link all subsystem's libs togheter
 * - Moved C-API in it's own top-level dir capi/
 * - Moved source/bigtest and source/test to tests/bigtest and test/xmltester
 * - Fixed PointLocator handling of LinearRings
 * - Changed CoordinateArrayFilter to reduce memory copies
 * - Changed UniqueCoordinateArrayFilter to reduce memory copies
 * - Added CGAlgorithms::isPointInRing() version working with
 *   Coordinate::ConstVect type (faster!)
 * - Ported JTS-1.7 version of ConvexHull with big attention to
 *   memory usage optimizations.
 * - Improved XMLTester output and user interface
 * - geos::geom::util namespace used for geom/util stuff
 * - Improved memory use in geos::geom::util::PolygonExtractor
 * - New ShortCircuitedGeometryVisitor class
 * - New operation/predicate package
 *
 * Revision 1.13  2005/11/09 08:57:07  strk
 * IntersectionMatrix made a concrete and final type.
 * Cleanups in class definition.
 *
 * Revision 1.12  2005/01/28 09:47:51  strk
 * Replaced sprintf uses with ostringstream.
 *
 * Revision 1.11  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.10  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

