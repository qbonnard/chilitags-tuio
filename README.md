Chilitags TUIO Server
=====================

This software is an application sending TUIO messages describing the Chilitags fiducial markers detected by a camera. Its goal is to facilitate the use of Chilitags in any TUIO capable application, even if it is written in a different language than the one used by the Chilitags library (C++). For further information about Chilitags, please visit [Chili's Webpage](http://chili.epfl.ch/software). For further information on the TUIO protocol and framework, please visit [TUIO.org](http://www.tuio.org/).

Dependencies
------------

This software depends on the [chilitags](https://github.com/chili-epfl/chilitags) library and OpenCV's `highgui` package.


Usage
-----

    chilitags-tuio [x-cam-resolution y-cam-resolution] [camera-index] [host port]

*x-cam-resolution y-cam-resolution* are the resolution parameters to give to the camera.
The default resolution is 640x480 pixels.

*camera-index* is the index of the camera to use to detect chilitags.
By default, it is 0, which corresponds to the first camera of the system.
If multiple cameras are attached to the computer, you can select them by increasing this index.

*host port* are the parameters to configure the connection between the tuio server and clients.
The defaults value correspond to the port 3333 on `localhost`.

Shortcomings
------------

The TUIO messages sent by this software describe chilitags as `2D Objects`, which means that they provide their identifier, position, orientation, speed, acceleration, rotational speed and rotational acceleration.

The TUIO messages do not contain the scale, width and height of the chilitags, even though they would be easily computed. Furthermore, assuming the user is willing to provide calibratiojn information, the chilitags could be defined in 3D too, albeit with a low precision on the depth.

You are welcome to manifest your interest in such features, or your own contribution implementing them!

Credits
-------

This software is based on the [TUIO C++ Reference Implementation](http://tuio.org/?software) and Demo Applications (Copyright 2005-2009 [Martin Kaltenbrunner](http://modin.yuri.at)), which is part of [reacTIVision](http://reactivision.sourceforge.net), an open source fiducial tracking and multi-touch framework based on computer vision. 

This program uses the [oscpack](http://www.audiomulch.com/~rossb/code/oscpack/) OpenSound Control library.

License
-------

Chilitags is released under the GPL.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.  

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA


We especially appreciate a reference in your paper, should you use Chilitags for a research project. Let us make that easy for you:

 * Chilitags: Robust Fiducial Markers for Augmented Reality. Q. Bonnard, G. Zufferey, A. Mazzei, S. Cuendet, N. Li, P. Dillenbourg. CHILI, EPFL, Switzerland. `http://chili.epfl.ch/`. 2013.

```
	@misc{chilitags,
		title = {Chilitags: Robust Fiducial Markers for Augmented Reality.},
		author={Bonnard, Quentin and Zufferey, Guillaume and Mazzei, Andrea and Cuendet, S\'{e}bastien and Li, Nan and Dillenbourg, Pierre},
		publisher={CHILI, EPFL, Switzerland},
		url={http://chili.epfl.ch/software},
		year={2013}
	}
```
