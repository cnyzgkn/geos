/**********************************************************************
 * $Id$
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 * $Log$
 * Revision 1.2  2004/07/01 14:12:44  strk
 * Geometry constructors come now in two flavors:
 * 	- deep-copy args (pass-by-reference)
 * 	- take-ownership of args (pass-by-pointer)
 * Same functionality is available through GeometryFactory,
 * including buildGeometry().
 *
 * Revision 1.1  2004/03/18 10:42:44  ybychkov
 * "IO" and "Util" upgraded to JTS 1.4
 * "Geometry" partially upgraded.
 *
 *
 **********************************************************************/


#include "../headers/geom.h"
#include "../headers/util.h"
#include "stdio.h"

namespace geos {

/**
* Create a shape factory which will create shapes using the given
* {@link GeometryFactory}.
*
* @param geomFact the factory to use
*/
GeometricShapeFactory::GeometricShapeFactory(GeometryFactory* newGeomFact){
	dim=new Dimensions();
	nPts=100;
	geomFact=newGeomFact;
}

/**
* Sets the location of the shape by specifying the base coordinate
* (which in most cases is the
* lower left point of the envelope containing the shape).
*
* @param base the base coordinate of the shape
*/
void GeometricShapeFactory::setBase(const Coordinate& base)  {
	dim->setBase(base);
}
/**
* Sets the location of the shape by specifying the centre of
* the shape's bounding box
*
* @param centre the centre coordinate of the shape
*/
void GeometricShapeFactory::setCentre(const Coordinate& centre)  {
	dim->setCentre(centre);
}

/**
* Sets the total number of points in the created Geometry
*/
void GeometricShapeFactory::setNumPoints(int nNPts) {
	nPts=nNPts;
}

/**
* Sets the size of the extent of the shape in both x and y directions.
*
* @param size the size of the shape's extent
*/
void GeometricShapeFactory::setSize(double size) { 
	dim->setSize(size);
}

/**
* Sets the width of the shape.
*
* @param width the width of the shape
*/
void GeometricShapeFactory::setWidth(double width) {
	dim->setWidth(width);
}

/**
* Sets the height of the shape.
*
* @param height the height of the shape
*/
void GeometricShapeFactory::setHeight(double height) { 
	dim->setHeight(height);
}

/**
* Creates a rectangular {@link Polygon}.
*
* @return a rectangular Polygon
*
*/
Polygon* GeometricShapeFactory::createRectangle(){
	int i;
	int ipt = 0;
	int nSide = nPts / 4;
	if (nSide < 1) nSide = 1;
	double XsegLen = dim->getEnvelope()->getWidth() / nSide;
	double YsegLen = dim->getEnvelope()->getHeight() / nSide;

	CoordinateList* pts=CoordinateListFactory::internalFactory->createCoordinateList(4 * nSide + 1);
	Envelope* env=dim->getEnvelope();

	double maxx = env->getMinX() + nSide * XsegLen;
	double maxy = env->getMinY() + nSide * XsegLen;

	for (i = 0; i < nSide; i++) {
		double x = env->getMinX() + i * XsegLen;
		double y = env->getMinY();
		//pts->setAt(*(new Coordinate(x, y)),ipt++);
		pts->setAt(Coordinate(x, y),ipt++);
	}
	for (i = 0; i < nSide; i++) {
		double x = env->getMaxX();
		double y = env->getMinY() + i * YsegLen;
		//pts->setAt(*(new Coordinate(x, y)),ipt++);
		pts->setAt(Coordinate(x, y),ipt++);
	}
	for (i = 0; i < nSide; i++) {
		double x = env->getMaxX() - i * XsegLen;
		double y = env->getMaxY();
		//pts->setAt(*(new Coordinate(x, y)),ipt++);
		pts->setAt(Coordinate(x, y),ipt++);
	}
	for (i = 0; i < nSide; i++) {
		double x = env->getMinX();
		double y = env->getMaxY() - i * YsegLen;
		//pts->setAt(*(new Coordinate(x, y)),ipt++);
		pts->setAt(Coordinate(x, y),ipt++);
	}
	//pts->setAt(*(new Coordinate(pts->getAt(0))),ipt++);
	pts->setAt(Coordinate(pts->getAt(0)),ipt++);
	LinearRing* ring=geomFact->createLinearRing(pts);
	Polygon* poly=geomFact->createPolygon(ring, NULL);
	return poly;
}

/**
* Creates a circular {@link Polygon}.
*
* @return a circle
*/
Polygon* GeometricShapeFactory::createCircle() {
	Envelope* env = dim->getEnvelope();
	double xRadius = env->getWidth() / 2.0;
	double yRadius = env->getHeight() / 2.0;

	double centreX = env->getMinX() + xRadius;
	double centreY = env->getMinY() + yRadius;

	CoordinateList* pts=CoordinateListFactory::internalFactory->createCoordinateList(nPts + 1);
	int iPt = 0;
		for (int i = 0; i < nPts; i++) {
		double ang = i * (2 * 3.14159265358979 / nPts);
		double x = xRadius * cos(ang) + centreX;
		double y = yRadius * sin(ang) + centreY;
		Coordinate* pt=new Coordinate(x, y);
		pts->setAt(*pt,iPt++);
	}
	pts->setAt(pts->getAt(0),iPt++);
	LinearRing* ring = geomFact->createLinearRing(pts);
	Polygon* poly=geomFact->createPolygon(ring,NULL);
	return poly;
}

/**
* Creates a elliptical arc, as a LineString.
*
* @return an elliptical arc
*/
LineString* GeometricShapeFactory::createArc(double startAng,double endAng){
	Envelope* env = dim->getEnvelope();
	double xRadius = env->getWidth() / 2.0;
	double yRadius = env->getHeight() / 2.0;

	double centreX = env->getMinX() + xRadius;
	double centreY = env->getMinY() + yRadius;

	double angSize = (endAng - startAng);
	if (angSize <= 0.0 || angSize > 2 * 3.14159265358979)
	angSize = 2 * 3.14159265358979;
	double angInc = angSize / nPts;

	CoordinateList* pts=CoordinateListFactory::internalFactory->createCoordinateList(nPts);
	int iPt = 0;
	for (int i = 0; i < nPts; i++) {
		double ang = startAng + i * angInc;
		double x = xRadius * cos(ang) + centreX;
		double y = yRadius * sin(ang) + centreY;
		Coordinate* pt = new Coordinate(x, y);
		geomFact->getPrecisionModel()->makePrecise(pt);
		pts->setAt(*pt,iPt++);
	}
	LineString* line = geomFact->createLineString(pts);
	return line;
}

void GeometricShapeFactory::Dimensions::setBase(const Coordinate& newBase)  {
	base=newBase;
}

void GeometricShapeFactory::Dimensions::setCentre(const Coordinate& newCentre)  {
	centre=newCentre;
}

void GeometricShapeFactory::Dimensions::setSize(double size){
	height = size;
	width = size;
}

void GeometricShapeFactory::Dimensions::setWidth(double nWidth) {
	width=nWidth;
}
void GeometricShapeFactory::Dimensions::setHeight(double nHeight) {
	height=nHeight;
}

Envelope* GeometricShapeFactory::Dimensions::getEnvelope() {
	if (!(base==Coordinate::nullCoord)) {
		return new Envelope(base.x, base.x + width, base.y, base.y + height);
	}
	if (!(centre==Coordinate::nullCoord)) {
		return new Envelope(centre.x - width/2, centre.x + width/2,centre.y - height/2, centre.y + height/2);
	}
	return new Envelope(0, width, 0, height);
}
}
