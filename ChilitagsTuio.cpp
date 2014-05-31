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

ChilitagsTuio::ChilitagsTuio(int xRes, int yRes, int cameraIndex, const std::string &host, int port, bool showFeedback):
    tuioServer(host.c_str(), port),
    cvCapture(cameraIndex),
    showFeedback(showFeedback)
{
    if (cvCapture.isOpened())
    {
#ifdef OPENCV3
        cvCapture.set(cv::CAP_PROP_FRAME_WIDTH, xRes);
        cvCapture.set(cv::CAP_PROP_FRAME_HEIGHT, yRes);
#else
        cvCapture.set(CV_CAP_PROP_FRAME_WIDTH, xRes);
        cvCapture.set(CV_CAP_PROP_FRAME_HEIGHT, yRes);
#endif
    }
    else {
        // TODO: should actually quit the program...
        std::cerr << "Unable to initialise video capture.\n";
    }

    if (showFeedback) cv::namedWindow("ChilitagsTuio");
}

ChilitagsTuio::~ChilitagsTuio() {
    if (showFeedback) cv::destroyWindow("ChilitagsTuio");
    cvCapture.release();
}

namespace {
    // a helper function taking care of drawing the tags if a visual feedback
    // is wanted
    void drawFeedback(
            cv::Mat inputImage,
            const std::map<int, chilitags::Quad> &tags,
            double processingTime) {
        static const cv::Scalar COLOR(255, 0, 255);

        cv::Mat outputImage = inputImage.clone();

        for (auto tag : tags) {

            // borders
            const cv::Mat_<cv::Point2f> corners(tag.second);
            for (size_t i = 0; i < 4; ++i) {
                // sub-pixel precision drawing
                static const int SHIFT = 16;
                static const float PRECISION = 1<<SHIFT;
                cv::line(
                        outputImage,
                        PRECISION*corners(i),
                        PRECISION*corners((i+1)%4),
                        COLOR, 1, CV_AA, SHIFT);
            }

            // tag ID
            cv::Point2f center = 0.5*(corners(0) + corners(2));
            cv::putText(outputImage, cv::format("%d", tag.first), center,
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, COLOR);
        }

        // resolution and processing time
        cv::putText(outputImage,
                cv::format("%dx%d %4.0f ms (press q to quit)",
                    outputImage.cols, outputImage.rows,
                    processingTime),
                cv::Point(32,32),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, COLOR);

        cv::imshow("ChilitagsTuio", outputImage);
    }
}

void ChilitagsTuio::run() {

    // Main loop, exiting when 'q is pressed'
    // TODO: key are ignored when feedback is hidden
    for (; 'q' != (char) cv::waitKey(1); ) {

        cvCapture.read(inputImage);

        int64 startTime = cv::getTickCount();
        auto tags = chilitags.find(inputImage);
        int64 endTime = cv::getTickCount();
        double processingTime = 1000.0*((double) endTime - startTime)/cv::getTickFrequency();

        tuioServer.initFrame(TUIO::TuioTime::getSessionTime());

        // Handle detected tags
        for (auto tag: tags) {
            int tagId = tag.first;
            const cv::Mat_<cv::Point2f> corners(tag.second);
            cv::Point2f center = 0.5*(corners(0) + corners(2));
            float x = center.x / (float) inputImage.cols;
            float y = center.y / (float) inputImage.rows;
            cv::Point2f topLine = corners(1)-corners(0);
            float angle = std::atan2(topLine.y,topLine.x);

            auto tuioObject = tuioObjects.find(tagId);
            if(tuioObject == tuioObjects.end()) {
                // new chilitag has been detected
                std::cout << "[NEW TAG]: id " << tagId << std::endl;
                tuioObjects[tagId] = tuioServer.addTuioObject(tagId, x, y, angle);
            }
            else 
            {
                // chilitag already detected in the previous frame
                std::cout << "[UPDATE TAG]: id " << tagId << " " << x << " " << y << " " << angle << std::endl;
                tuioServer.updateTuioObject(tuioObject->second, x, y, angle);
            }
        }

        // Remove disappeared tags
        for (auto tuioObject = tuioObjects.begin(); tuioObject != tuioObjects.end(); ) {
            if (tags.find(tuioObject->first) == tags.end()) {
                // chilitag not detected anymore
                std::cout << "[REMOVE TAG]: id " << tuioObject->first << std::endl;
                tuioServer.removeTuioObject(tuioObject->second);
                tuioObjects.erase(tuioObject++);
            }
            else {
                ++tuioObject;
            }
        }

        tuioServer.commitFrame();

        if(showFeedback) drawFeedback(inputImage, tags, processingTime);
    }
}


int main(int argc, char* argv[])
{
    if (( argc != 1) && ( argc != 3) && ( argc != 4)  && ( argc != 6) && ( argc != 7) ) {
        std::cout << "usage: "<< argv[0] <<" [x-Cam-Resolution y-Cam-Resolution] [cameraIndex] [host port] [hideFeedback]\n";
        return 0;
    }

    int xRes = 640;
    int yRes = 480;
    int cameraIndex = 0;
    std::string host = "127.0.0.1";
    int port = 3333;
    bool showFeedback = true;
    if (argc >= 3) {
        xRes = std::atoi(argv[1]);
        yRes = std::atoi(argv[2]);
    }
    if (argc >= 4) {
        cameraIndex = atoi(argv[3]);
    }
    if( argc >= 6 ) {
        host = argv[4];
        port = atoi(argv[5]);
    };
    if( argc == 7 ) {
        showFeedback = false;
    }

    ChilitagsTuio(xRes, yRes, cameraIndex, host, port, showFeedback).run();

    return 0;
}
