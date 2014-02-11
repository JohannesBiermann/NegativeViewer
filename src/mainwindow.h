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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "dslr.h"
#include "webcam.h"
#include <videoprocessing.h>

// constants
const int CAMERA_WEBCAM = 0;
const int CAMERA_DSLR = 1;

////////////////////////////
namespace Ui {
class MainWindow;
}
///////////////////////////

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0, int cameratype = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;

    // camera libs
    WebCam webcam;
    DSLR dslr;

    int cameraType;

    cv::Mat cvmCurrentFrame;

    VideoProcessing videoProcessing;

    QTimer* tmrTimer;

    // saves the selected file extension for the current session
    QString fileExtension;
    QString filePath;

    void startCapture();
    void stopCapture();

    void setupUIWebcam();
    void setupUIDSLR();

    void updateFrameIfNecessary();

    bool resizeFrame;
    bool resizeFrame100Percent;

protected:
    void resizeEvent(QResizeEvent *event);



public slots:
    void processFrameAndUpdateGUI();
    
private slots:
    void on_btnPauseOrResume_clicked();
    void on_btnInvert_clicked();
    void on_btnSave_clicked();
    void on_btnFlip_clicked();
    void on_chkGreyscale_clicked();
    void on_selectCamResolution_currentIndexChanged(int index);
    void on_camIndex_valueChanged(int arg1);
    void on_chkSharpen_clicked();
    void on_chkEqualize_clicked();
    void on_chkCrop_clicked();
    void on_sldrCropThreshold_sliderMoved(int position);
    void on_rotateBox_valueChanged(int angle);

    void on_chkAutoFocus_clicked(bool checked);
    void on_sldrFocus_sliderMoved(int position);
    void on_btnTakePhoto_clicked();
    void on_btnFitToScreen_clicked();
    void on_btnAutofocus_clicked();
    void on_chkCropWhite_clicked(bool checked);

signals:
};

#endif // MAINWINDOW_H
