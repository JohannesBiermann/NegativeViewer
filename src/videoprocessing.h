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

#ifndef VIDEOPROCESSING_H
#define VIDEOPROCESSING_H

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <webcam.h>

class VideoProcessing
{
private:
    bool picInvert;
    bool picFlip;
    bool picGreyscale;
    bool picSharpen;
    bool picEqualize;
    bool picCrop;
    int picCropThreshold;
    bool picCropOnWhite;
    double picRotateAngle; // angle in degrees, can be negative

    cv::Mat frame;

    void invertFrame();
    void convertFrame2Grey();
    void flipFrame();
    void rotateFrame();
    void cropFrame();
    void sharpenFrame();
    void equalizeFrame();


public:
    VideoProcessing();

    // main method to call from outside to process the current frame
    cv::Mat process(cv::Mat &frame);

    // getter and setters here //////////////////////////////////////////////
    bool isPicCrop() const {
        return picCrop;
    }

    void setPicCrop(bool picCrop) {
        this->picCrop = picCrop;
    }

    int getPicCropThreshold() const {
        return picCropThreshold;
    }

    void setPicCropThreshold(int picCropThreshold) {
        this->picCropThreshold = picCropThreshold;
    }

    bool isPicEqualize() const {
        return picEqualize;
    }

    void setPicEqualize(bool picEqualize) {
        this->picEqualize = picEqualize;
    }

    bool isPicFlip() const {
        return picFlip;
    }

    void setPicFlip(bool picFlip) {
        this->picFlip = picFlip;
    }

    double rotateAngle() const {
        return picRotateAngle;
    }

    void setRotateAngle(double angle) {
        this->picRotateAngle = angle;
    }

    bool isPicGreyscale() const {
        return picGreyscale;
    }

    void setPicGreyscale(bool picGreyscale) {
        this->picGreyscale = picGreyscale;
    }

    bool isPicInvert() const {
        return picInvert;
    }

    void setPicInvert(bool picInvert) {
        this->picInvert = picInvert;
    }

    bool isPicSharpen() const {
        return picSharpen;
    }

    void setPicSharpen(bool picSharpen) {
        this->picSharpen = picSharpen;
    }

    bool isPicCropOnWhite() const {
        return picCropOnWhite;
    }

    void setPicCropOnWhite(bool picCropOnWhite) {
        this->picCropOnWhite = picCropOnWhite;
    }


};

#endif // VIDEOPROCESSING_H
