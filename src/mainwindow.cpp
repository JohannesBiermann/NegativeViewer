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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MatToQImage.h"

#include <QtCore>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent, int cameratype) : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // set the desired camera type (current webcam and DSLR
    this->cameraType = cameratype;
    switch (cameratype)
    {
        case CAMERA_WEBCAM: {
            this->webcam = WebCam(ui->frame);
            setupUIWebcam();
            bool ret = this->webcam.openCamera();
            if (ret == false) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("The camera cannot be opened, check device number or connection.");
                msgBox.exec();
                ui->frame->setText("error opening camera, check device number or if any other program is accessing the camera.");
                return;
            }
        }
        break;

        case CAMERA_DSLR: {
            this->dslr = DSLR(ui);
            setupUIDSLR();
            bool ret = this->dslr.openCamera(&this->dslr);
            if (ret == false) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setText("The camera cannot be opened! Please check if camera is in PTP mode and unmount memory card");
                msgBox.exec();
                ui->frame->setText("error opening camera, please check if camera is setup to PTP mode and unmount memory card from the system. Quit any application that browses the memory card. In doubt remove flash card and try again.");
                return;
            }
        }
        break;

        default:
            exit(1);
    }
    resizeFrame = false;
    resizeFrame100Percent = false;

    tmrTimer = new QTimer(this);

    // Start the main "loop" that processes the frame update
    connect(tmrTimer, SIGNAL(timeout()), this, SLOT(processFrameAndUpdateGUI()));

    startCapture();

}

// Configure UI depending on camera type
void MainWindow::setupUIWebcam()
{
    ui->dslr->hide();
    ui->toolbox->setCurrentIndex(0);
    ui->lbldslrlog->hide();
    ui->gplog->hide();
    ui->chkCropWhite->hide();
}

void MainWindow::setupUIDSLR()
{
    ui->webcam->hide();
    ui->toolbox->setCurrentIndex(1);
    ui->chkCropWhite->hide();
}

///////////////////////////////////
// Destructor, called on application quit, releases any open resources to the camera
MainWindow::~MainWindow()
{
    delete ui;
    switch (this->cameraType)
    {
        case CAMERA_WEBCAM:
            this->webcam.closeCamera();
        break;

        case CAMERA_DSLR:
            this->dslr.closeCamera();
        break;
    }
}

void MainWindow::startCapture()
{
    if (false == tmrTimer->isActive())
        tmrTimer->start(20);
}

void MainWindow::stopCapture()
{
    if (true == tmrTimer->isActive())
        tmrTimer->stop();
}

////////////////////////////////////


void MainWindow::processFrameAndUpdateGUI() {
    //capWebCam.read(cvmCurrentFrame);

    switch (this->cameraType)
    {
        case CAMERA_WEBCAM:
            this->webcam.processFrame(cvmCurrentFrame);
        break;

        case CAMERA_DSLR:
            // capture DSLR live view if no current still exists, else use current still to display and process
            if (this->dslr.isStillCaptured() == false) {
                this->dslr.processFrame(cvmCurrentFrame);
            }else{
                // if a still is capture always use the unmodified image
                // if not the various image manipulation are not able to undo a processing
                this->dslr.getCurrentStill().copyTo(cvmCurrentFrame);
            }
        break;
    }

    // return if there is a capture problem
    if (cvmCurrentFrame.empty() == true) return;

    cvmCurrentFrame = videoProcessing.process(cvmCurrentFrame);

    // fit image to window or show 1:1
    // convert from opencv to QT
    QImage qimgDisplay =  MatToQImage(cvmCurrentFrame);
    QPixmap pm = QPixmap::fromImage(qimgDisplay);
    if (resizeFrame == true) {
        ui->scrollArea->width();
        ui->scrollArea->height();

        //QPixmap qimgDisplay = QPixmap::fromImage(currentCapture);
        int w = ui->scrollArea->width() - 30;
        int h = ui->scrollArea->height() - 30;
        pm = pm.scaled(QSize(w, h), Qt::KeepAspectRatio, Qt::SmoothTransformation);


    }

    // and then display it on the QT widget
    ui->frame->setPixmap(pm);
    ui->frame->setFixedWidth(pm.width());
    ui->frame->setFixedHeight(pm.height());

    // if live view area is too small increase it
    if (size().width() < pm.width() && this->cameraType == CAMERA_DSLR) {
        resize(pm.width()+200, pm.height()+50);
    }

}

void MainWindow::updateFrameIfNecessary()
{
    // use two ifs to speed up calls
    if (cameraType == CAMERA_DSLR) {
        if (this->dslr.isStillCaptured()) {
            processFrameAndUpdateGUI();
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (resizeFrame == true) {
        updateFrameIfNecessary();
    }
}


///////////////////////////////////////////
void MainWindow::on_btnPauseOrResume_clicked()
{
    if (tmrTimer->isActive() == true) {
        stopCapture();
        ui->btnPauseOrResume->setText("Resume");
        ui->btnPauseOrResume->setIcon(QPixmap(":/appimages/led-icons/control_play.png"));
    }else{
        startCapture();
        ui->btnPauseOrResume->setText("Pause");
        ui->btnPauseOrResume->setIcon(QPixmap(":/appimages/led-icons/control_pause.png"));
    }

    // remove still if we enter live view again
    if (cameraType == CAMERA_DSLR) {
        this->dslr.removeStill();
    }
}

void MainWindow::on_btnInvert_clicked()
{
    if(videoProcessing.isPicInvert() == false) {
        videoProcessing.setPicInvert(true);
        ui->btnInvert->setText("normal");
        ui->btnInvert->setIcon(QPixmap(":/appimages/led-icons/invert_i.png"));
    }else{
        videoProcessing.setPicInvert(false);
        ui->btnInvert->setText("invert");
        ui->btnInvert->setIcon(QPixmap(":/appimages/led-icons/invert.png"));

    }
    updateFrameIfNecessary();
}

void MainWindow::on_btnFlip_clicked()
{
    if(videoProcessing.isPicFlip() == false) {
        videoProcessing.setPicFlip(true);
        ui->btnFlip->setIcon(QPixmap(":/appimages/led-icons/shape_flip_vertical_r.png"));
    }else{
        videoProcessing.setPicFlip(false);
        ui->btnFlip->setIcon(QPixmap(":/appimages/led-icons/shape_flip_vertical.png"));

    }
    updateFrameIfNecessary();
}


void MainWindow::on_btnSave_clicked()
{

    //QImage qCaptureImg((uchar*)cvmCurrentFrame.data, cvmCurrentFrame.cols, cvmCurrentFrame.rows, cvmCurrentFrame.step, QImage::Format_Indexed8);

    QImage qCaptureImg = MatToQImage(cvmCurrentFrame);

    stopCapture();

    if (filePath.isNull()) {
        filePath = QDir::currentPath();
    }

    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Save Capture"),
        filePath,
        tr("PNG (*.png);;TIF (*.tif);;JPG (*.jpg)"),
        &fileExtension);

    if( !filename.isNull() )
    {
        // save current path to set selected path in next save action
        QFileInfo fi = QFileInfo(filename);
        filePath = fi.absoluteDir().absolutePath();

        // Generate file path + file name without extension
        // this is done because the extension is set below
        // if not it could save file.png.png if a previous
        // file should be overwritten
        filename = filePath + QDir::separator() + fi.baseName();

        if (fileExtension == "PNG (*.png)") {
            filename += ".png";
        }else if(fileExtension == "TIF (*.tif)") {
            filename += ".tif";
        }else{
            filename += ".jpg";
        }
        qCaptureImg.save(filename);
    }
    startCapture();
}


void MainWindow::on_chkGreyscale_clicked()
{
    if (ui->chkGreyscale->isChecked()) {
        videoProcessing.setPicGreyscale(true);
    }else{
        videoProcessing.setPicGreyscale(false);
    }
    updateFrameIfNecessary();
}

void MainWindow::on_selectCamResolution_currentIndexChanged(int index)
{
    int width = 0;
    int height = 0;
    if (index == 1) {
        width = 1920;
        height = 1080;
    }else if (index == 2) {
        width = 1280;
        height = 720;
    }else if (index == 3 || index == 0) {
        width = 640;
        height = 480;
    }
    bool ret = this->webcam.switchResolution(width, height);
    if (ret == false) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The camera resolution cannot be changed. Maybe the camera does not support this resolution.");
        msgBox.exec();
        return;
    }else{

        // change UI size on success
        //ui->frame->setFixedWidth(width);
        //ui->frame->setFixedHeight(height);
        //ui->MainWindow->activate();
        resize(width+200, height+50);
    }

}

void MainWindow::on_camIndex_valueChanged(int index)
{
    bool ret = this->webcam.switchCamera(index);

    if (ret == false) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("The camera cannot be opened, check device number or connection.");
        msgBox.exec();
        ui->frame->setText("error opening camera, check device number or if any other program is accessing the camera.");
    }else{
        ui->frame->setFixedWidth(WebCam::DEFAULT_RESOLUTION_WIDTH);
        ui->frame->setFixedHeight(WebCam::DEFAULT_RESOLUTION_HEIGHT);
        //ui->mainlayout->activate();
        resize(WebCam::DEFAULT_RESOLUTION_WIDTH + 200, 0);
    }
}

void MainWindow::on_chkAutoFocus_clicked(bool checked)
{
    bool ret = false;
    if (checked == false) {
        ui->sldrFocus->setEnabled(true);
        ret = this->webcam.autoFocusOnOff(true);
    }else{
        ui->sldrFocus->setEnabled(false);
        ui->sldrFocus->setValue(0);
        ret = this->webcam.autoFocusOnOff(false);

    }

    if (ret == false) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Autofocus setting cannot be changed. Maybe the webcam does not support this control.");
        msgBox.exec();
    }

}

void MainWindow::on_sldrFocus_sliderMoved(int position)
{
    float pos = position / 100.0;

    bool ret = this->webcam.changeFocus(pos);

    if (ret == false) {
        /*QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Autofocus range cannot be changed. Maybe the webcam does not support this");
        msgBox.exec();*/

        // do nothing, the autofocus on off switch already produces an error
        return;
    }

}

void MainWindow::on_chkSharpen_clicked()
{
    if (ui->chkSharpen->isChecked()) {
        videoProcessing.setPicSharpen(true);
        ui->chkSharpen->setIcon(QPixmap(":/appimages/led-icons/stamp_s.png"));
    }else{
        videoProcessing.setPicSharpen(false);
        ui->chkSharpen->setIcon(QPixmap(":/appimages/led-icons/stamp.png"));
    }
    updateFrameIfNecessary();
}

void MainWindow::on_chkEqualize_clicked()
{
    if (ui->chkEqualize->isChecked()) {
        videoProcessing.setPicEqualize(true);
    }else{
        videoProcessing.setPicEqualize(false);
    }
    updateFrameIfNecessary();
}

void MainWindow::on_chkCrop_clicked()
{
    if (ui->chkCrop->isChecked()) {
        ui->chkCropWhite->show();
        videoProcessing.setPicCrop(true);
        ui->sldrCropThreshold->setEnabled(true);
        ui->lblCropThreshold->setEnabled(true);
    }else{
        ui->chkCropWhite->hide();
        videoProcessing.setPicCrop(false);
        ui->sldrCropThreshold->setEnabled(false);
        ui->lblCropThreshold->setEnabled(false);
    }
    updateFrameIfNecessary();
}


void MainWindow::on_sldrCropThreshold_sliderMoved(int position)
{
    videoProcessing.setPicCropThreshold(position);
    updateFrameIfNecessary();
}


void MainWindow::on_rotateBox_valueChanged(int angle)
{
    videoProcessing.setRotateAngle((double)angle);
    updateFrameIfNecessary();
}

void MainWindow::on_btnTakePhoto_clicked()
{
    ui->btnTakePhoto->setDisabled(true);
    stopCapture();
    // close and open the camera again, because canon dslr does not exit live view
    // and autofocus does not work
    //this->dslr.closeCamera();
    //this->dslr.openCamera(&this->dslr);
    this->dslr.captureStillImage(cvmCurrentFrame);
    ui->btnPauseOrResume->setText("Resume");
    ui->btnPauseOrResume->setIcon(QPixmap(":/appimages/led-icons/control_play.png"));
    ui->btnPauseOrResume->setChecked(true);

    ui->btnFitToScreen->setChecked(true);
    resizeFrame = true;
    updateFrameIfNecessary();
    ui->btnTakePhoto->setEnabled(true);

}

void MainWindow::on_btnFitToScreen_clicked()
{
    if (ui->btnFitToScreen->isChecked()) {
        resizeFrame = true;
    }else{
        resizeFrame = false;
    }
   updateFrameIfNecessary();
}

void MainWindow::on_btnAutofocus_clicked()
{
    ui->btnAutofocus->setDisabled(true);
    stopCapture();
    // remove still if we enter live view again
    if (cameraType == CAMERA_DSLR) {
        this->dslr.removeStill();
    }
    this->dslr.triggerAutofocus();
    this->on_btnPauseOrResume_clicked();
    ui->btnAutofocus->setEnabled(true);

    //stopCapture();
    //this->dslr.closeCamera();
    //this->dslr.openCamera(&this->dslr);


}

void MainWindow::on_chkCropWhite_clicked(bool checked)
{
    if (checked == true) {
        this->videoProcessing.setPicCropOnWhite(true);
    }else{
        this->videoProcessing.setPicCropOnWhite(false);
    }
    updateFrameIfNecessary();
}
