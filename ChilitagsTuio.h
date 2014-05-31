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

#include <string>
#include "TuioServer.h"
#include "TuioObject.h"
#include <opencv2/highgui/highgui.hpp>
#include <chilitags/chilitags.hpp>

class ChilitagsTuio { 
	
public:
	ChilitagsTuio(int xRes, int yRes, int cameraIndex, const std::string &host, int port, bool showFeedback);
	~ChilitagsTuio();
	
	void run();

private:

	TUIO::TuioServer tuioServer;
	std::map<int, TUIO::TuioObject *> tuioObjects; 
	
	cv::VideoCapture cvCapture;
	cv::Mat inputImage;

	chilitags::Chilitags chilitags;
	bool showFeedback;
};

#endif /* INCLUDED_ChilitagsTuio_H */
