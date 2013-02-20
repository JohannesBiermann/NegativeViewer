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

#include "webcam.h"

#include <linux/kernel.h>
#include <linux/videodev2.h>

WebCam::WebCam(QLabel *frame)
{

    this->frame = frame;
/*
    void VideoProcessing::setLogitechManualFocus(int focus, int camera) {
        CResult ret;
        ret = c_init();
        if(ret) fprintf(stderr, "Unable to c_init (%d).\n", ret);
        CHandle hDevice = c_open_device("video0");

        CControlValue value;
        value.value = focus;
        ret = c_set_control(hDevice, CC_FOCUS_ABSOLUTE, &value);
        if(ret) {
            printf("Failed to set focus absolute. (ret = %d)\n", ret);
        }
        else {
            //printf("Successfully set focus absolute to = %d\n", value.value);
        }

        c_close_device(hDevice);

        c_cleanup();
    }

    void VideoProcessing::setLogitechAutoFocusOnOff(bool off, int camera) {
        CResult ret;
        ret = c_init();
        if(ret) fprintf(stderr, "Unable to c_init (%d).\n", ret);
        CHandle hDevice = c_open_device("video0");

        CControlValue value;
        value.value = off;
        ret = c_set_control(hDevice, CC_AUTO_FOCUS, &value);
        if(ret) {
            printf("Failed to set focus absolute. (ret = %d)\n", ret);
        }
        else {
            //printf("Successfully set focus absolute to = %d\n", value.value);
        }

        c_close_device(hDevice);

        c_cleanup();
    }
    */
}

bool WebCam::openCamera(int cameraindex)
{
    capWebCam.open(cameraindex);

    if (capWebCam.isOpened() == false) {
        return false;
    }else{
        capWebCam.set(CV_CAP_PROP_FRAME_WIDTH, DEFAULT_RESOLUTION_WIDTH);
        capWebCam.set(CV_CAP_PROP_FRAME_HEIGHT, DEFAULT_RESOLUTION_HEIGHT);
        frame->setFixedWidth(DEFAULT_RESOLUTION_WIDTH);
        frame->setFixedHeight(DEFAULT_RESOLUTION_HEIGHT);
    }
    return true;
}

bool WebCam::openCamera()
{
    return openCamera(0);
}

bool WebCam::closeCamera()
{
    capWebCam.release();
    return true;
}

bool WebCam::switchCamera(int index)
{
    closeCamera();

    return openCamera(index);
}

bool WebCam::switchResolution(int width, int height)
{

    double old_w = capWebCam.get(CV_CAP_PROP_FRAME_WIDTH);
    double old_h = capWebCam.get(CV_CAP_PROP_FRAME_HEIGHT);

    capWebCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
    capWebCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);

    double new_w = capWebCam.get(CV_CAP_PROP_FRAME_WIDTH);
    double new_h = capWebCam.get(CV_CAP_PROP_FRAME_HEIGHT);

    if (new_w == (double)width && new_h == (double)height) {
        return true;
    }else{
        capWebCam.set(CV_CAP_PROP_FRAME_WIDTH, old_w);
        capWebCam.set(CV_CAP_PROP_FRAME_HEIGHT, old_h);
    }
    return false;
}

bool WebCam::autoFocusOnOff(bool off)
{
    int value = 0;
    if (off == false)
        value = 1;

    capWebCam.set(V4L2_CID_FOCUS_AUTO, value);

    if (capWebCam.get(V4L2_CID_FOCUS_AUTO) != (double) value)
        return false;

    return true;
}

bool WebCam::changeFocus(float position)
{
    capWebCam.set(V4L2_CID_FOCUS_ABSOLUTE, position);

    if (capWebCam.get(V4L2_CID_FOCUS_ABSOLUTE) != (double) position)
        return false;

    return true;
}

void WebCam::processFrame(cv::Mat &currentFrame)
{
   capWebCam.read(currentFrame);
   //capWebCam >> currentFrame;
}


