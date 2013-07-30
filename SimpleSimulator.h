/*
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

#ifndef INCLUDED_SimpleSimulator_H
#define INCLUDED_SimpleSimulator_H

#include "TuioServer.h"
#include "TuioObject.h"

#include <chilitags/DetectChilitags.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace TUIO;

class SimpleSimulator { 
	
public:
	SimpleSimulator(int xRes, int yRes, int cameraIndex, const char* host, int port);
	~SimpleSimulator() {
		delete tuioServer;
	};
	
	void run();

	static const int firstTagId = 0;
	static const int nTags = 1024;

	class SmoothCoordinates {
	public:
		void init(float x, float y, float angle){
			oldestPostion = 0;
			for (unsigned int i=0; i<windowSize; ++i) {
				xs[i]=x;
				ys[i]=y;
				angles[i]=angle;
			}
		}
		void update(float x, float y, float angle){
			oldestPostion %= windowSize;
			xs[oldestPostion] = x;
			ys[oldestPostion] = y;
			angles[oldestPostion]=angle;
			++oldestPostion;
		}

		float x() { return average(xs);}
		float y() { return average(ys);}
		float angle() { return average(angles);}

	private:
		static const unsigned int windowSize = 5;

		unsigned int oldestPostion;
		float xs[windowSize];
		float ys[windowSize];
		float angles[windowSize];

		float average(float a[windowSize]){
			float sum = 0.0f;
			for (unsigned int i=0; i<windowSize; ++i) {
				sum += a[i];
			}
			return sum/(float) windowSize;
		}
	};

private:

	TuioServer *tuioServer;
	TuioObject *tuioObjects[nTags]; 
	SmoothCoordinates coordinates[nTags];
	
	CvCapture *cvCapture;
	int xRes;
	int yRes;
	IplImage *inputImage;
	chilitags::DetectChilitags detectChilitags;
};

#endif /* INCLUDED_SimpleSimulator_H */
