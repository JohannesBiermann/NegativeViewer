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

#ifndef DSLR_H
#define DSLR_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QtCore>
#include <QLabel>
#include <QTextBrowser>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-setting.h>

class DSLR
{
public:
    DSLR(Ui::MainWindow *ui = 0);

    bool openCamera(void* ref);

    bool closeCamera();

    void processFrame(cv::Mat &currentFrame);

    bool captureStillImage(cv::Mat &frame);

    void triggerAutofocus();

    cv::Mat getCurrentStill();

    bool isStillCaptured();

    void removeStill();

    // C callback functions
    static void cp_error_func (GPContext *context, const char *text, void *data);
    static void cp_message_func (GPContext *context, const char *text, void *data);

private:

    Ui::MainWindow *ui;

    Camera *camera;
    GPContext *context;

    QImage currentCapture;

    cv::Mat stillImage;

    cv::Mat qimage2cvMat(QImage &im);

    void handleError(const char *text);

    void handleMessage(const char *text);

    void flapCanonMirror(int value);

    bool canon;

};

#endif // DSLR_H
