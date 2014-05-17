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


#include "ChilitagsTuio.h"

#include <stdio.h>
#include <string.h>

// Tuio server sends for every frame the id, the center position(between 0.0 and 1.0) and the angle(between 180.0 and 180.0) of the found tags
ChilitagsTuio::ChilitagsTuio(int xRes, int yRes, int cameraIndex, const char* host, int port):
cvCapture(NULL),
xRes(xRes),
yRes(yRes),
showOutputImage(true)
{
	if ((strcmp(host,"default")==0) && (port==0)) tuioServer = new TuioServer();
	else tuioServer = new TuioServer(host, port);
	
	for (int tagId = firstTagId; tagId < firstTagId+nTags; ++tagId) {
		tuioObjects[tagId] = NULL;
	}

	
	// The source of input images
	cvCapture = new cv::VideoCapture(cameraIndex);

	// TODO: should actually quit the program...
	if (!cvCapture->isOpened())
	{
		std::cerr << "Unable to initialise video capture." << std::endl;
	}

	#ifdef OPENCV3
	    cvCapture->set(cv::CAP_PROP_FRAME_WIDTH, xRes);
	    cvCapture->set(cv::CAP_PROP_FRAME_HEIGHT, yRes);
	#else
	    cvCapture->set(CV_CAP_PROP_FRAME_WIDTH, xRes);
	    cvCapture->set(CV_CAP_PROP_FRAME_HEIGHT, yRes);
	#endif


	// The detection is not perfect, so if a tag is not detected during one frame,
	// the tag will shortly disappears, which results in flickering.
	// To address this, Chilitags "cheats" by keeping tags for n frames
	// at the same position. When tags disappear for more than 5 frames,
	// Chilitags actually removes it.
	// Here, we cancel this to show the raw detection results.
	chilitags.setFilter(0, 0.);

	cv::namedWindow("DisplayChilitags");
	
}

void ChilitagsTuio::run() {

	// Main loop, exiting when 'q is pressed'
    	for (; 'q' != (char) cv::waitKey(1); ) {
		
		// Capture a new image.
        	cvCapture->read(inputImage);

		tuioServer->initFrame(TuioTime::getSessionTime());

		// Start measuring the time needed for the detection
        	int64 startTime = cv::getTickCount();

		// Detect tags on the current image;
        	// The resulting map associates tag ids (between 0 and 1023)
        	// to four 2D points corresponding to the corners positions
        	// in the picture.
        	std::map<int, chilitags::Quad> tags = chilitags.find(inputImage);

        	// Measure the processing time needed for the detection
        	int64 endTime = cv::getTickCount();
        	double processingTime = 1000.0*((double) endTime - startTime)/cv::getTickFrequency();

		for (int tagId = firstTagId; tagId < firstTagId+nTags; ++tagId) 
		{
			TuioObject *tuioObject = tuioObjects[tagId];

			std::map<int, chilitags::Quad>::iterator it = tags.find(tagId);
			if(it != tags.end())
			{
				const cv::Mat_<cv::Point2f> corners(it->second);
				cv::Point2f center = 0.5*(corners(0) + corners(2));
				float x = center.x / (float) xRes;
				float y = center.y / (float) yRes;
				CvPoint2D32f topLine = corners(1)-corners(0);
				float angle = std::atan2(topLine.y,topLine.x)*180.0/M_PI;
				if(tuioObject == NULL) // new chilitag has been detected
				{
					coordinates[tagId].init(x, y, angle);
					tuioObjects[tagId] = tuioServer->addTuioObject(tagId, x, y, angle);
					std::cout << "[NEW TAG]: id " << tagId << std::endl;
				}
				else // chilitag already detected in the previous frame
				{
					coordinates[tagId].update(x, y, angle);
					tuioServer->updateTuioObject(tuioObject,
						coordinates[tagId].x(),
						coordinates[tagId].y(),
						coordinates[tagId].angle());
					std::cout << "[UPDATE TAG]: id " << tagId << " " << coordinates[tagId].x() << " " << coordinates[tagId].y() << " " << coordinates[tagId].angle() << std::endl;
				}
			}
			else if (tuioObject != NULL) { // chilitag not detected anymore
				std::cout << "[REMOVE TAG]: id " << tagId << std::endl;
				tuioServer->removeTuioObject(tuioObject);
				tuioObjects[tagId] = NULL;
			}
		}
		tuioServer->commitFrame();


		// show image
		if(showOutputImage)
		{

			// First, we set up some constants related to the information overlaid
			// on the captured image
			const static cv::Scalar COLOR(255, 0, 255);
			// OpenCv can draw with sub-pixel precision with fixed point coordinates
			static const int SHIFT = 16;
			static const float PRECISION = 1<<SHIFT;

			// We dont want to draw directly on the input image, so we clone it
			cv::Mat outputImage = inputImage.clone();

			for (const std::pair<int, chilitags::Quad> & tag : tags) {

			    int id = tag.first;
			    // We wrap the corner matrix into a datastructure that allows an
			    // easy access to the coordinates
			    const cv::Mat_<cv::Point2f> corners(tag.second);

			    // We start by drawing the borders of the tag
			    for (size_t i = 0; i < 4; ++i) {
				cv::line(
				    outputImage,
				    PRECISION*corners(i),
				    PRECISION*corners((i+1)%4),
				    COLOR, 1, CV_AA, SHIFT);
			    }

			    // Other points can be computed from the four corners of the Quad.
			    // Chilitags are oriented. It means that the points 0,1,2,3 of
			    // the Quad coordinates are consistently the top-left, top-right,
			    // bottom-right and bottom-left corners.
			    // (i.e. clockwise, starting from top-left)
			    // Using this, we can compute (an approximation of) the center of
			    // tag.
			    cv::Point2f center = 0.5*(corners(0) + corners(2));
			    cv::putText(outputImage, cv::format("%d", id), center,
				        cv::FONT_HERSHEY_SIMPLEX, 0.5, COLOR);
			}

			// Some stats on the current frame (resolution and processing time)
			cv::putText(outputImage,
				    cv::format("%dx%d %4.0f ms (press q to quit)",
				               outputImage.cols, outputImage.rows,
				               processingTime),
				    cv::Point(32,32),
				    cv::FONT_HERSHEY_SIMPLEX, 0.5, COLOR);

			// Finally...
			cv::imshow("DisplayChilitags", outputImage);
		}

	} 
	cv::destroyWindow("DisplayChilitags");
    	cvCapture->release();
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
	const char *host = "127.0.0.1";
	int port = 3333;
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
	
	ChilitagsTuio(xRes, yRes, cameraIndex, host, port).run();

	return 0;
}


