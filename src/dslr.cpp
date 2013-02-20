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

#include "dslr.h"
#include <fcntl.h>
#include <stdio.h>
#include <cstring>


DSLR::DSLR(Ui::Dialog *ui)
{

    this->ui = ui;
    this->canon = false;
}


bool DSLR::openCamera(void *ref)
{
    // init cam
    gp_camera_new (&camera);
    context = gp_context_new();
    // set callbacks for camera messages
    // void gp_context_set_message_func   (GPContext *context,
    //GPContextMessageFunc func,  void *data);
    //typedef void (* GPContextMessageFunc)  (GPContext *context, const char *text, void *data);

    gp_context_set_error_func(context,  DSLR::cp_error_func, this);
    gp_context_set_message_func(context, DSLR::cp_message_func, this);

    int ret = gp_camera_init (camera, context);

    if (ret < GP_OK) {
        return false;
    }

    CameraAbilities ca;
    ret = gp_camera_get_abilities (camera, &ca);

    if (ret < GP_OK) {
        return false;
    }

    QString msg = "Found: ";
    msg.append(ca.model);
    ui->gplog->append(msg);

    if (msg.contains("Canon", Qt::CaseInsensitive) == true) {
        this->canon = true;
    }

    return true;
}

bool DSLR::closeCamera()
{
    // close camera
    gp_camera_exit(camera, context);
    gp_camera_unref(camera);
    gp_context_unref(context);

    return true;
}

void DSLR::processFrame(cv::Mat &currentFrame)
{

    CameraFile *cf;
    gp_file_new(&cf);

    gp_camera_capture_preview (camera, cf, context);

    unsigned long datasize = 0;
    const char *data = NULL;

    gp_file_get_data_and_size(cf, &data, &datasize);

    if (datasize == 0 )
        return;


    // Load gphoto data into QPixmap and convert it to QImage

    /* Remark: for some reason loading data directly into cv::Mat does not work
      no error message but also no picture
      I have tried various conversion from uchar to vector, but no succes
      the only thing that works right now is to load the gp data to QPixmap and then
      to QImage and then load the image into opencv via cv::Mat. For some reason
      opencv can't decode the image in the memory, but QT can.
      This is not the best solution.

    std::vector<char> vec;
    const char* end = data + strlen( data );
    vec.insert( vec.end(), data, end );

    cv::Mat imgbuf = cv::Mat(vec);
    cv::Mat imgMat = cv::imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);
    */

    QPixmap pm;
    pm.loadFromData( (uchar*) data,  (uint) datasize);
    QImage im = pm.toImage();

    // convert qimage to cv::Mat
    currentFrame = qimage2cvMat(im);

    gp_file_free(cf);

}

bool DSLR::captureStillImage(cv::Mat &frame)
{
    // if it is a canon try to flap down the mirror
    // if not the camera will not focus correctly for whatever reason
    if (this->canon == true) {
        flapCanonMirror(0);
    }

    CameraFile *cf;
    CameraFilePath camera_file_path;
    int ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
    if (ret < GP_OK)
        return false;
    gp_file_new(&cf);

    gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, cf, context);

    // Load gphoto data into QPixmap and convert it to QImage
    unsigned long datasize = 0;
    const char *data = NULL;

    gp_file_get_data_and_size(cf, &data, &datasize);

    QPixmap pm;
    pm.loadFromData( (uchar*) data,  (uint) datasize);
    QImage im = pm.toImage();

    frame = qimage2cvMat(im);

    gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);
    gp_file_free(cf);

    frame.copyTo(stillImage);

    //this code waits until the events from the camera have been processed
    // else the camera may hang
    int waittime = 2000;
    CameraEventType type;
    void *datavoid;
    while(1) {
        gp_camera_wait_for_event(camera, waittime, &type, &datavoid, context);
        if(type == GP_EVENT_TIMEOUT) {
            break;
        }
        else if (type == GP_EVENT_CAPTURE_COMPLETE) {
            ui->gplog->append("Capture complete");
            waittime = 100;
        }
        else if (type != GP_EVENT_UNKNOWN) {
            printf("Unexpected event received from camera: %d\n", (int)type);
            ui->gplog->append("Unexpected event: ");
            ui->gplog->append(QString::number(type));
        }
    }

    return true;
}

bool DSLR::isStillCaptured()
{
    if (stillImage.empty() == true)
        return false;
    return true;
}

void DSLR::removeStill()
{
    this->stillImage.release();
}



cv::Mat DSLR::getCurrentStill()
{
    return this->stillImage;
}

void DSLR::triggerAutofocus()
{

    if (this->canon == true) {
        flapCanonMirror(1);
    }

    CameraWidget *widgettree, *xwidget;
    int value;
    gp_camera_get_config(camera, &widgettree, context);

    int ret = gp_widget_get_child_by_name (widgettree, "autofocusdrive", &xwidget);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widgettree, "autofocusdrive", &xwidget);

    //gp_widget_get_child_by_name (widgettree, "autofocusdrive", &xwidget);
    value = 1;
    gp_widget_set_value (xwidget, &value);
    gp_camera_set_config(camera, widgettree, context);

    gp_widget_free (widgettree);

    //this code waits until the events from the camera have been processed
    // else the camera may hang
    CameraEventType evttype;
    void *evtdata;
    int	retval;
    do {
        retval = gp_camera_wait_for_event (camera, 1, &evttype, &evtdata, context);
    } while ((retval == GP_OK) && (evttype != GP_EVENT_TIMEOUT));

}

cv::Mat DSLR::qimage2cvMat(QImage &im) {

    // Populate cv::Mat from QImage
    // found this on http://opencv-users.1802565.n2.nabble.com/QImage-to-OpenCV-td5121181.html
    // it looks like Qimage has an alpha channel that needs to be cut off
    cv::Mat mat = cv::Mat(im.height(), im.width(), CV_8UC4, (uchar*)im.bits(), im.bytesPerLine());
    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}

void DSLR::handleError(const char *text)
{
    //char buffer[256];
    //vsprintf (buffer,format, args);
    //QString error = QString::fromAscii(buffer);
    //QString::fro
    //error.append(args);
    //ui->gplog->append(buffer);
    //this->ui->gplog->append("text");
    qDebug() << "### Error ###";
    qDebug() << text;
}

void DSLR::cp_error_func(GPContext *context, const char *text, void *data)
{
    // since gphoto2.5 the reinterpret cast does not work
    //
    //DSLR* dslr = reinterpret_cast<DSLR*>(data);
    //dslr->handleError(text);
    //dslr->handleError(text);
    //qDebug() << text;
    fprintf(stderr, "*** Camera error ***\n");
    fprintf(stderr, text, "%s");
    //vfprintf(stderr, format, args);
    //fprintf(stderr, "\n");


}

void DSLR::cp_message_func(GPContext *context, const char *text, void *data)
{
    // since gphoto2.5 the reinterpret cast does not work
    //DSLR* dslr = reinterpret_cast<DSLR*>(data);
    //dslr->handleMessage(format, args);
    //    qDebug() << text;
    //dslr->handleMessage(text);
    fprintf(stdout, "*** Camera message ***\n");
    fprintf(stdout, text, "%s");
}

void DSLR::handleMessage(const char *text)
{
    //char buffer[256];
    //vsprintf (buffer,format, args);
    //ui->gplog->append(buffer);
    //ui->gplog->append(text);

}

void DSLR::flapCanonMirror(int value)
{
    CameraWidget *parent, *child;
    int val;
    gp_camera_get_config(camera, &parent, context);
    gp_widget_get_child_by_name (parent, "eosviewfinder", &child);
    gp_widget_get_value (child, &val);
    qDebug() << "EOSVIEWFINDER FLAP Current value";
    qDebug() << val;
    qDebug() << "New value";
    val = value;
    qDebug() << value;
    qDebug() << gp_widget_set_value (child, &val);
    int vret = gp_camera_set_config(camera, parent, context);
    // And sometimes the camera may hang, so re-init the camera
    if (vret < 0) {
        closeCamera();
        openCamera(this);
    }
    gp_widget_free (parent);
}
