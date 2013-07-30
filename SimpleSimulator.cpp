/*
	Chilitags TUIO Server

	Copyright 2013 Quentin Bonnard quentin@bonnard.eu

	This program is a modification of the

	TUIO C++ Server Demo - part of the reacTIVision project
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

#include <chilitags/Chilitag.hpp>
#include "SimpleSimulator.h"

SimpleSimulator::SimpleSimulator(int xRes, int yRes, int cameraIndex, const char* host, int port):
cvCapture(cvCaptureFromCAM(cameraIndex)),
xRes(xRes),
yRes(yRes),
detectChilitags(&inputImage){
	if ((strcmp(host,"default")==0) && (port==0)) tuioServer = new TuioServer();
	else tuioServer = new TuioServer(host, port);
	
	for (int tagId = firstTagId; tagId < firstTagId+nTags; ++tagId) {
		tuioObjects[tagId] = NULL;
	}
	//tuioServer->enablePeriodicMessages();

	if (!cvCapture)
	{
		std::cerr << "unable to initialise CVCapture" << std::endl;
	} else {
		cvSetCaptureProperty(cvCapture, CV_CAP_PROP_FRAME_WIDTH, xRes);
		cvSetCaptureProperty(cvCapture, CV_CAP_PROP_FRAME_HEIGHT, yRes);
		cvSetCaptureProperty(cvCapture, CV_CAP_PROP_FPS, cameraIndex);
		cvSetCaptureProperty(cvCapture, CV_CAP_PROP_MODE, 1);
	}
}

void SimpleSimulator::run() {
	for (;;) {
		inputImage = cvQueryFrame(cvCapture);
		detectChilitags.update();

		tuioServer->initFrame(TuioTime::getSessionTime());

		for (int tagId = firstTagId; tagId < firstTagId+nTags; ++tagId) {
			chilitags::Chilitag chilitag(tagId, 5);
			TuioObject *tuioObject = tuioObjects[tagId];

			if (chilitag.isPresent()) {
				chilitags::Quad corners = chilitag.getCorners();
				CvPoint2D32f center = corners.getCenter();
				float x = center.x / (float) xRes;
				float y = center.y / (float) yRes;
				float angle = corners.getAngle();
				if (angle < 0.0f) angle = 2.0*M_PI + angle;

				if (tuioObject == NULL) {
					coordinates[tagId].init(x, y, angle);
					tuioObjects[tagId] = tuioServer->addTuioObject(tagId, x, y, angle);
				} else {
					coordinates[tagId].update(x, y, angle);
					tuioServer->updateTuioObject(tuioObject,
						coordinates[tagId].x(),
						coordinates[tagId].y(),
						coordinates[tagId].angle());
				}
			} else if (tuioObject != NULL) {
				tuioServer->removeTuioObject(tuioObject);
				tuioObjects[tagId] = NULL;
			}
		}

		tuioServer->commitFrame();
	} 
}

int main(int argc, char* argv[])
{
	if (( argc != 1) && ( argc != 3) && ( argc != 4)  && ( argc != 6) ) {
        	std::cout << "usage: "<< argv[0] <<" [x-Cam-Resolution y-Cam-Resolution] [cameraIndex] [host port]\n";
        	return 0;
	}

	int xRes = 640;
	int yRes = 480;
	int cameraIndex = 0;
	const char *host = "default";
	int port = 0;
	if (argc >= 3) {
		xRes = std::atoi(argv[1]);
		yRes = std::atoi(argv[2]);
	}
	if (argc >= 4) {
		cameraIndex = atoi(argv[3]);
	}
	if( argc == 6 ) {
		host = argv[4];
		port = atoi(argv[5]);
	};
	
	SimpleSimulator(xRes, yRes, cameraIndex, host, port).run();

	return 0;
}


