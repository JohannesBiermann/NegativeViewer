/*************************************************************************
* NegativeViewer                                                         *
* It is a tool for viewing photographic negatives and slides by using    *
* a webcam or digital single lens reflex camera.                         *
* You can apply various transformations on the live image like inverting *
* and rotating a picture, croping the frame and saving the result.       *
*                                                                        *
* Copyright (c) 2013 Johannes Biermann code(at)johannesb.eu              *
* ---------------------------------------------------------------------- *
*                                                                        *
* This file is part of NegativeViewer.                                   *
*                                                                        *
* NegativeViewer is free software: you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by   *
* the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                    *
*                                                                        *
* NegativeViewer is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of         *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
* GNU General Public License for more details.                           *
*                                                                        *
* You should have received a copy of the GNU General Public License      *
* along with NegativeViewer. If not, see <http://www.gnu.org/licenses/>. *
**************************************************************************/

#ifndef WEBCAM_H
#define WEBCAM_H

#include <QtCore>
#include <QLabel>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class WebCam
{

private:
    cv::VideoCapture capWebCam;
    QLabel *frame;
    bool openCamera(int cameraindex);

public:
    const static int DEFAULT_RESOLUTION_WIDTH = 640;
    const static int DEFAULT_RESOLUTION_HEIGHT = 640;
    WebCam(QLabel *frame = 0);
    bool openCamera();

    bool closeCamera();

    bool switchCamera(int index);

    bool switchResolution(int width, int height);

    bool autoFocusOnOff(bool off);

    bool changeFocus(float position);

    void processFrame(cv::Mat &currentFrame);

};

#endif // WEBCAM_H
