/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_OPDISTANCE_H
#define GEOS_OPDISTANCE_H

#include <geos/platform.h>
#include <geos/operation.h>
#include <geos/geom.h>
#include <geos/algorithm/PointLocator.h>
#include <memory>
#include <vector>


namespace geos {
namespace operation { // geos.operation

/// Provides classes for computing the distance between geometries
namespace distance { // geos.operation.distance


/*
 * Represents the location of a point on a Geometry.
 * Maintains both the actual point location (which of course
 * may not be exact) as well as information about the component
 * and segment index where the point occurs.
 * Locations inside area Geometrys will not have an associated segment index,
 * so in this case the segment index will have the sentinel value of
 * INSIDE_AREA.
 */
class GeometryLocation {
private:
	const geom::Geometry *component;
	int segIndex;
	geom::Coordinate pt;
public:  
	/**
	 * Special value of segment-index for locations inside area geometries. These
	 * locations do not have an associated segment index.
	 */
	static const int INSIDE_AREA = -1;

	/**
	 * Constructs a GeometryLocation specifying a point on a geometry, as well as the 
	 * segment that the point is on (or INSIDE_AREA if the point is not on a segment).
	 */
	GeometryLocation(const geom::Geometry *newComponent, int newSegIndex, const geom::Coordinate &newPt);

	/**
	 * Constructs a GeometryLocation specifying a point inside an area geometry.
	 */  
	GeometryLocation(const geom::Geometry *newComponent, const geom::Coordinate &newPt);

	/**
	 * Returns the geometry associated with this location.
	 */
	const geom::Geometry* getGeometryComponent();

	/**
	 * Returns the segment index for this location. If the location is inside an
	 * area, the index will have the value INSIDE_AREA;
	 *
	 * @return the segment index for the location, or INSIDE_AREA
	 */
	int getSegmentIndex();

	/**
	 * Returns the location.
	 */
	geom::Coordinate& getCoordinate();

	/**
	 * Returns whether this GeometryLocation represents a point inside an area geometry.
	 */
	bool isInsideArea();
};


/*
 * Extracts a single point
 * from each connected element in a Geometry
 * (e.g. a polygon, linestring or point)
 * and returns them in a list
 */
class ConnectedElementPointFilter: public geom::GeometryFilter {

private:
	std::vector<const geom::Coordinate*> *pts;

public:
	/**
	 * Returns a list containing a Coordinate from each Polygon, LineString, and Point
	 * found inside the specified geometry. Thus, if the specified geometry is
	 * not a GeometryCollection, an empty list will be returned.
	 */
	static std::vector<const geom::Coordinate*>* getCoordinates(const geom::Geometry *geom);

	ConnectedElementPointFilter(std::vector<const geom::Coordinate*> *newPts)
		:
		pts(newPts)
	{}

	void filter_ro(const geom::Geometry *geom);

	void filter_rw(geom::Geometry *geom) {};
};

/** \brief
 * A ConnectedElementPointFilter extracts a single point
 * from each connected element in a Geometry
 * (e.g. a polygon, linestring or point)
 * and returns them in a list. The elements of the list are 
 * DistanceOp::GeometryLocation.
 */
class ConnectedElementLocationFilter: public geom::GeometryFilter {
private:

	std::vector<GeometryLocation*> *locations;

public:
	/**
	 * Returns a list containing a point from each Polygon, LineString, and Point
	 * found inside the specified geometry. Thus, if the specified geometry is
	 * not a GeometryCollection, an empty list will be returned. The elements of the list 
	 * are {@link com.vividsolutions.jts.operation.distance.GeometryLocation}s.
	 */  
	static std::vector<GeometryLocation*>* getLocations(const geom::Geometry *geom);

	ConnectedElementLocationFilter(std::vector<GeometryLocation*> *newLocations)
		:
		locations(newLocations)
	{}

	void filter_ro(const geom::Geometry *geom);
	void filter_rw(geom::Geometry *geom);
};


/*
 * Computes the distance and
 * closest points between two {@link Geometry}s.
 * <p>
 * The distance computation finds a pair of points in the input geometries
 * which have minimum distance between them.  These points may
 * not be vertices of the geometries, but may lie in the interior of
 * a line segment. In this case the coordinate computed is a close
 * approximation to the exact point.
 * <p>
 * The algorithms used are straightforward O(n^2)
 * comparisons.  This worst-case performance could be improved on
 * by using Voronoi techniques.
 *
 */
class DistanceOp {
public:
	/**
	 * Compute the distance between the closest points of two geometries.
	 * @param g0 a {@link Geometry}
	 * @param g1 another {@link Geometry}
	 * @return the distance between the geometries
	 */
	static double distance(const geom::Geometry *g0, const geom::Geometry *g1);

	/**
	 * Compute the the closest points of two geometries.
	 * The points are presented in the same order as the input Geometries.
	 *
	 * @param g0 a {@link Geometry}
	 * @param g1 another {@link Geometry}
	 * @return the closest points in the geometries
	 */
	static geom::CoordinateSequence* closestPoints(geom::Geometry *g0, geom::Geometry *g1);

	/**
	 * Constructs a DistanceOp that computes the distance and closest points between
	 * the two specified geometries.
	 */
	DistanceOp(const geom::Geometry *g0, const geom::Geometry *g1);

	~DistanceOp();

	/**
	 * Report the distance between the closest points on the input geometries.
	 *
	 * @return the distance between the geometries
	 */
	double distance();

	/**
	 * Report the coordinates of the closest points in the input geometries.
	 * The points are presented in the same order as the input Geometries.
	 *
	 * @return a pair of {@link Coordinate}s of the closest points
	 */
	geom::CoordinateSequence* closestPoints();

	/**
	 * Report the locations of the closest points in the input geometries.
	 * The locations are presented in the same order as the input Geometries.
	 *
	 * @return a pair of {@link GeometryLocation}s for the closest points
	 */
	std::vector<GeometryLocation*>* closestLocations();

private:

	algorithm::PointLocator ptLocator;
	std::vector<geom::Geometry const*> geom;
	std::vector<geom::Coordinate *> newCoords;
	std::vector<GeometryLocation*> *minDistanceLocation;
	double minDistance;
	void updateMinDistance(double dist);
	void updateMinDistance(std::vector<GeometryLocation*> *locGeom, bool flip);
	void computeMinDistance();
	void computeContainmentDistance();

	void computeInside(std::vector<GeometryLocation*> *locs,
			const geom::Polygon::ConstVect& polys,
			std::vector<GeometryLocation*> *locPtPoly);

	void computeInside(GeometryLocation *ptLoc,
			const geom::Polygon *poly,
			std::vector<GeometryLocation*> *locPtPoly);

	void computeLineDistance();

	void computeMinDistanceLines(
			const geom::LineString::ConstVect& lines0,
			const geom::LineString::ConstVect& lines1,
			std::vector<GeometryLocation*>& locGeom);

	void computeMinDistancePoints(
			const geom::Point::ConstVect& points0,
			const geom::Point::ConstVect& points1,
			std::vector<GeometryLocation*>& locGeom);

	void computeMinDistanceLinesPoints(
			const geom::LineString::ConstVect& lines0,
			const geom::Point::ConstVect& points1,
			std::vector<GeometryLocation*>& locGeom);

	void computeMinDistance(const geom::LineString *line0,
			const geom::LineString *line1,
			std::vector<GeometryLocation*>& locGeom);

	void computeMinDistance(const geom::LineString *line,
			const geom::Point *pt,
			std::vector<GeometryLocation*>& locGeom);
};

} // namespace geos.operation.distance
} // namespace geos.operation
} // namespace geos

#endif

/**********************************************************************
 * $Log$
 * Revision 1.9  2006/03/09 16:46:48  strk
 * geos::geom namespace definition, first pass at headers split
 *
 * Revision 1.8  2006/03/03 10:46:21  strk
 * Removed 'using namespace' from headers, added missing headers in .cpp files, removed useless includes in headers (bug#46)
 *
 * Revision 1.7  2006/02/20 10:14:18  strk
 * - namespaces geos::index::*
 * - Doxygen documentation cleanup
 *
 * Revision 1.6  2006/02/19 19:46:49  strk
 * Packages <-> namespaces mapping for most GEOS internal code (uncomplete, but working). Dir-level libs for index/ subdirs.
 *
 * Revision 1.5  2006/01/31 19:07:34  strk
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
 * Revision 1.4  2004/07/20 08:34:18  strk
 * Fixed a bug in opDistance.h.
 * Removed doxygen tags from obsoleted CoordinateList.cpp.
 * Got doxygen to run with no warnings.
 *
 * Revision 1.3  2004/07/19 13:19:31  strk
 * Documentation fixes
 *
 * Revision 1.2  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.1  2004/07/02 13:20:42  strk
 * Header files moved under geos/ dir.
 *
 * Revision 1.10  2004/05/14 13:42:46  strk
 * DistanceOp bug removed, cascading errors fixed.
 *
 * Revision 1.9  2004/04/13 12:29:21  strk
 * GeometryLocation const-correctness.
 *
 * Revision 1.8  2004/04/13 10:05:51  strk
 * GeometryLocation constructor made const-correct.
 * Fixed erroneus down-casting in DistanceOp::computeMinDistancePoints.
 *
 * Revision 1.7  2004/04/05 06:35:14  ybychkov
 * "operation/distance" upgraded to JTS 1.4
 *
 * Revision 1.6  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

