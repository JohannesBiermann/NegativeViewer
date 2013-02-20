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

#include "videoprocessing.h"

VideoProcessing::VideoProcessing()
{

    picInvert = false;
    picFlip = false;
    picGreyscale = false;
    picSharpen = false;
    picEqualize = false;
    picCrop = false;
    picCropThreshold = 200;
    picCropOnWhite = true;
    //sframe;
}

/*
  Auto crop the displayed image by the black borders
*/
void VideoProcessing::cropFrame() {
    /*cv::Mat gray;
    cv::cvtColor(frame, gray, CV_BGR2GRAY);

    std::vector<cv::Point> points;
    cv::Mat_<uchar>::iterator it = gray.begin<uchar>();
    cv::Mat_<uchar>::iterator end = gray.end<uchar>();
    for (; it != end; ++it)
    {
        if (*it) points.push_back(it.pos());
    }
    // Compute minimal bounding box
    cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

    // Set Region of Interest to the area defined by the box
    cv::Rect roi;
    roi.x = box.center.x - (box.size.width / 2);
    roi.y = box.center.y - (box.size.height / 2);
    roi.width = box.size.width;
    roi.height = box.size.height;
    NO NO
    */

    cv::Mat binary;
    frame.copyTo(binary);
    // convert image to greyscale if not already done
    if (this->picGreyscale == false) {
        cv::cvtColor(frame, binary, CV_BGR2GRAY);
    }

    // Crop white or black background?
    if (this->picCropOnWhite == true) {
        // on white background negate the image if not already
        if (this->picInvert == false) {
            cv::bitwise_not(binary, binary);
        }
    }else {
        // on black background do not negate image
        if (this->picInvert == true) {
            // if invert make it undo
            cv::bitwise_not(binary, binary);
        }
    }

    cv::threshold(binary, binary, this->picCropThreshold, 255, CV_THRESH_BINARY);

    std::vector<cv::Point> points;
    cv::Mat_<uchar>::iterator it = binary.begin<uchar>();
    cv::Mat_<uchar>::iterator end = binary.end<uchar>();
    for (; it != end; ++it)
      if (*it)
        points.push_back(it.pos());

    if (points.capacity() <= 0) {
        return;
    }

    cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

    cv::Size box_size = box.size;

    // do not crop if box size is too small
    if (box_size.width < 10 || box_size.height < 10)
        return;

    // add some spacing to the cropped box to make it not too narrow
    box_size.width += 10;
    box_size.height += 10;

    if (box.angle < -45.)
      std::swap(box_size.width, box_size.height);
    cv::Mat cropped;
    cv::getRectSubPix(frame, box_size, box.center, cropped);

    frame = cropped;

    binary.release();
    cropped.release();
    //cropped.copyTo(frame);

    // qDebug() << " x " << roi.x << " y " << roi.y << " h " << roi.height << " w " << roi.width;

    // Crop the original image to the defined ROI
    //cv::Mat crop = frame(roi);
    //frame = crop;

    /*cv::threshold(tmp, tmp, 1, 255, CV_THRESH_BINARY);
    std::vector<cv::Mat> countours;
    cv::findContours(tmp, countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    cv::Rect rect = cv::boundingRect(countours[0]);
    qDebug() << " x " << rect.x << " y " << rect.y << " h " << rect.height << " w " << rect.width;
    */
}

/*
  performs auto tone correction on history
  this tries to distribute the colors evenly on the histogram
*/
void VideoProcessing::equalizeFrame() {
    // Equalize histogram, we have to do this for each color channel
    if (this->picGreyscale == false) {
        cv::Mat hsvImage;
        // convert to hsv color space to have brightness seperated from the color channels
        // auto equalize of R, G, B is not good, it causes massive color shift
        cv::cvtColor(frame, hsvImage, CV_BGR2HSV);
        std::vector<cv::Mat> channels;
        // split of the three channels
        cv::split(frame, channels);
        cv::Mat v_channel = channels[2];
        // auto equalize the brightness channel
        cv::equalizeHist(v_channel, v_channel);
        // combine the channels again and overwrite the original image
        cv::merge(channels, frame);
        // convert back the color space
        cv::cvtColor(frame, hsvImage, CV_HSV2BGR);
    }else{
        // for greyscale images it is much easier
        cv::equalizeHist(frame, frame);
    }
}

/*
  rotates the frame by 180 degrees
*/
void VideoProcessing::flipFrame() {
    cv::flip(frame, frame, -1);
}

/*
   inverts the picture
*/
void VideoProcessing::invertFrame() {
    // works but slower
    //frame = frame.inv();
    //for(int i=0;i<frame.rows;i++) for(int j=0;j<frame.cols;j++) for(int k=0;k<frame.channels();k++)
    //  frame[i+j*frame.channels()+k]=255-frame[i+j*frame.channels()+k];

    // works but it can take some time to loop through each pixel and each channel
    /*for(int y = 0; y < frame.rows; y++){
        for(int x = 0; x < frame.cols; x++){
            for(int c = 0; c < frame.channels(); c++){
                int output = 255 - frame.at<cv::Vec3b>(y,x)[c];
                frame.at<cv::Vec3b>(y,x)[c] = output;
            }
        }
    }*/
    // this is faster, invert every bit of the matrix to have a negative image
     cv::bitwise_not(frame, frame);
}

/*
  applies unsharp mask to the frame to sharp the capture
  implemented as described on http://en.wikipedia.org/wiki/Unsharp_masking
*/
void VideoProcessing::sharpenFrame() {
    cv::Mat tmp;
    // make gaussion blur first of radius 5 and save it to temporay matrix
    cv::GaussianBlur(frame, tmp, cv::Size(5,5), 5);
    // substract the sharpened image from the original with some factors
    cv::addWeighted(frame, 1.6, tmp, -0.6, 0, frame);
}

/*
  Performs the greyscale conversion
*/
void VideoProcessing::convertFrame2Grey() {
    cv::cvtColor(this->frame, this->frame, CV_BGR2GRAY);
}

/*
  public callable function
  to apply image effects set the getters on this class
*/
cv::Mat VideoProcessing::process(cv::Mat &frame) {
    // save reference to the current frame, do NOT copy it into the memory
    this->frame = frame;

    if (picGreyscale == true) {
        convertFrame2Grey();
    }

    if (picInvert == true) {
        invertFrame();
    }

    if (picFlip == true) {
        flipFrame();
    }

    if (picEqualize == true) {
        equalizeFrame();
    }

    if (picSharpen == true) {
        sharpenFrame();
    }

    if (picCrop == true) {
        cropFrame();
    }


    return this->frame;
}


