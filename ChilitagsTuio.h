/*
 Chilitags TUIO Server

 Copyright 2013 Quentin Bonnard quentin@bonnard.eu

 This program is a modification of the

 TUIO Demo - part of the reacTIVision project
 http://reactivision.sourceforge.net/
 
 Copyright (c) 2005-2009 Martin Kaltenbrunner <mkalten@iua.upf.edu>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_ChilitagsTuio_H
#define INCLUDED_ChilitagsTuio_H

#include "TuioServer.h"
#include "TuioObject.h"
#include "SmoothCoordinates.h"

#include <opencv2/highgui/highgui.hpp>
#include <chilitags/chilitags.hpp>

using namespace TUIO;

class ChilitagsTuio { 
	
public:
	ChilitagsTuio(int xRes, int yRes, int cameraIndex, const char* host, int port);
	~ChilitagsTuio() {
		delete tuioServer;
	};
	
	void run();

	static const int firstTagId = 0;
	static const int nTags = 1024;

private:

	TuioServer *tuioServer;
	TuioObject *tuioObjects[nTags]; 
	SmoothCoordinates coordinates[nTags];
	
	cv::VideoCapture *cvCapture;
	int xRes;
	int yRes;
	cv::Mat inputImage;

	chilitags::Chilitags chilitags;
	bool showOutputImage;
};

#endif /* INCLUDED_ChilitagsTuio_H */
