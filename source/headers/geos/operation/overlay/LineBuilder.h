/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_OP_OVERLAY_LINEBUILDER_H
#define GEOS_OP_OVERLAY_LINEBUILDER_H

#include <vector>

// Forward declarations
namespace geos {
	namespace geom {
		class GeometryFactory;
		class CoordinateSequence;
		class LineString;
	}
	namespace geomgraph {
		class DirectedEdge;
		class Edge;
	}
	namespace algorithm {
		class PointLocator;
	}
	namespace operation {
		namespace overlay {
			class OverlayOp;
		}
	}
}

namespace geos {
namespace operation { // geos::operation
namespace overlay { // geos::operation::overlay

/** \brief
 * Forms JTS LineStrings out of a the graph of geomgraph::DirectedEdge
 * created by an OverlayOp.
 *
 */
class LineBuilder {

public:

	LineBuilder(OverlayOp *newOp,
			const geom::GeometryFactory *newGeometryFactory,
			algorithm::PointLocator *newPtLocator);

	~LineBuilder();

	/**
	 * @return a list of the LineStrings in the result of the specified overlay operation
	 */
	std::vector<geom::LineString*>* build(int opCode);

	/**
	 * Find and mark L edges which are "covered" by the result area (if any).
	 * L edges at nodes which also have A edges can be checked by checking
	 * their depth at that node.
	 * L edges at nodes which do not have A edges can be checked by doing a
	 * point-in-polygon test with the previously computed result areas.
	 */
	void collectLineEdge(geomgraph::DirectedEdge *de,
			int opCode,
			std::vector<geomgraph::Edge*>* edges);

	/**
	 * Collect edges from Area inputs which should be in the result but
	 * which have not been included in a result area.
	 * This happens ONLY:
	 * 
	 *  -  during an intersection when the boundaries of two
	 *     areas touch in a line segment
	 *  -   OR as a result of a dimensional collapse.
	 * 
	 */
	void collectBoundaryTouchEdge(geomgraph::DirectedEdge *de,
			int opCode,
			std::vector<geomgraph::Edge*>* edges);

private:
	OverlayOp *op;
	const geom::GeometryFactory *geometryFactory;
	algorithm::PointLocator *ptLocator;
	std::vector<geomgraph::Edge*> lineEdgesList;
	std::vector<geom::LineString*>* resultLineList;
	void findCoveredLineEdges();
	void collectLines(int opCode);
	void buildLines(int opCode);
	void labelIsolatedLines(std::vector<geomgraph::Edge*> *edgesList);

	/**
	 * Label an isolated node with its relationship to the target geometry.
	 */
	void labelIsolatedLine(geomgraph::Edge *e, int targetIndex);

	/*
	 * If the given CoordinateSequence has mixed 3d/2d vertexes
	 * set Z for all vertexes missing it.
	 * The Z value is interpolated between 3d vertexes and copied
	 * from a 3d vertex to the end.
	 */
	void propagateZ(geom::CoordinateSequence *cs);
};


} // namespace geos::operation::overlay
} // namespace geos::operation
} // namespace geos

#endif // ndef GEOS_OP_OVERLAY_LINEBUILDER_H

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/17 13:24:59  strk
 * opOverlay.h header splitted. Reduced header inclusions in operation/overlay implementation files. ElevationMatrixFilter code moved from own file to ElevationMatrix.cpp (ideally a class-private).
 *
 **********************************************************************/

