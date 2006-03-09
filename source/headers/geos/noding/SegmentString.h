/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006      Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_NODING_SEGMENTSTRING_H
#define GEOS_NODING_SEGMENTSTRING_H

#include <vector>

#include <geos/noding/SegmentNodeList.h>

#include <geos/inline.h>

namespace geos {
namespace noding { // geos.noding

/** \brief
 * Represents a list of contiguous line segments,
 * and supports noding the segments.
 *
 * The line segments are represented by a CoordinateSequence.
 *
 * TODO:
 * This should be changed to use a vector of Coordinate,
 * to optimize the noding of contiguous segments by
 * reducing the number of allocated objects.
 *
 * SegmentStrings can carry a context object, which is useful
 * for preserving topological or parentage information.
 * All noded substrings are initialized with the same context object.
 *
 * Final class.
 *
 * Last port: noding/SegmentString.java rev. 1.5 (JTS-1.7)
 */
class SegmentString {
public:
	typedef std::vector<const SegmentString*> ConstVect;
	typedef std::vector<SegmentString *> NonConstVect;

private:
	SegmentNodeList eiList;
	geom::CoordinateSequence *pts;
	unsigned int npts;
	const void* context;
	bool isIsolatedVar;

	void testInvariant() const;

public:

	//SegmentString(const geom::CoordinateSequence *newPts, const void* newContext)

	/// Construct a SegmentString.
	//
	/// @param newPts CoordinateSequence representing the string,
	/// externally owned
	///
	/// @param newContext the context associated to this SegmentString
	///
	SegmentString(geom::CoordinateSequence *newPts, const void* newContext);

	~SegmentString();

	const void* getContext() const;

	const void* getData() const;

	const SegmentNodeList& getNodeList() const;

	SegmentNodeList& getNodeList();

	unsigned int size() const;

	const geom::Coordinate& getCoordinate(unsigned int i) const;

	/// \brief
	/// Return a pointer to the CoordinateSequence associated
	/// with this SegmentString.
	//
	/// Note that the CoordinateSequence is not owned by
	/// this SegmentString!
	///
	geom::CoordinateSequence* getCoordinates() const;

	// Return a read-only pointer to this SegmentString CoordinateSequence
	//const CoordinateSequence* getCoordinatesRO() const { return pts; }

	void setIsolated(bool isIsolated);

	bool isIsolated() const;
	
	bool isClosed() const;

	/** \brief
	 * Gets the octant of the segment starting at vertex <code>index</code>.
	 *
	 * @param index the index of the vertex starting the segment. 
	 *              Must not be the last index in the vertex list
	 * @return the octant of the segment at the vertex
	 */
	int getSegmentOctant(unsigned int index) const;

	/** \brief
	 * Adds EdgeIntersections for one or both
	 * intersections found for a segment of an edge to the edge
	 * intersection list.
	 */
	void addIntersections(algorithm::LineIntersector *li, unsigned int segmentIndex,
			int geomIndex);

	/** \brief
	 * Add an SegmentNode for intersection intIndex.
	 *
	 * An intersection that falls exactly on a vertex
	 * of the SegmentString is normalized
	 * to use the higher of the two possible segmentIndexes
	 */
	void addIntersection(algorithm::LineIntersector *li, unsigned int segmentIndex,
			int geomIndex, int intIndex);

	/** \brief
	 * Add an EdgeIntersection for intersection intIndex.
	 *
	 * An intersection that falls exactly on a vertex of the
	 * edge is normalized
	 * to use the higher of the two possible segmentIndexes
	 */
	void addIntersection(const geom::Coordinate& intPt, unsigned int segmentIndex);

	static void getNodedSubstrings(const SegmentString::NonConstVect& segStrings,
			SegmentString::NonConstVect* resultEdgeList);

	static SegmentString::NonConstVect* getNodedSubstrings(
			const SegmentString::NonConstVect& segStrings);
};

} // namespace geos.noding
} // namespace geos

#ifdef USE_INLINE
# include "geos/noding/SegmentString.inl"
#endif

#endif

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/09 16:46:49  strk
 * geos::geom namespace definition, first pass at headers split
 *
 **********************************************************************/

