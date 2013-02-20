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

#include <QMessageBox>
#include <QSplashScreen>
#include <QtGui/QApplication>
#include "dialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/appimages/splash.png"));
    splash->show();

    Qt::Alignment bottom = Qt::AlignCenter | Qt::AlignBottom;
    splash->showMessage(QObject::tr("Please select camera type"),
                           bottom, Qt::white);

    QMessageBox msgBox;
    QPushButton *webcamButton = msgBox.addButton("Webcam", QMessageBox::ActionRole);
    QPushButton *dslrButton = msgBox.addButton("DSLR", QMessageBox::ActionRole);
    msgBox.setStandardButtons(QMessageBox::Cancel);

    msgBox.setText("Welcome to the positive/negative slide viewer application. Please select the connected camera.");
    msgBox.setInformativeText("Do you want to capture images from webcam or DSLR?");
    msgBox.setWindowTitle("Please select camera type");

    QSize mSize = msgBox.sizeHint();
    msgBox.move(mSize.width()-100,mSize.height());
    msgBox.exec();

    int camera;

    if (msgBox.clickedButton() == (QAbstractButton*) webcamButton) {
        // webcam
        camera = CAMERA_WEBCAM;
    } else if (msgBox.clickedButton() == (QAbstractButton*) dslrButton) {
        // dslr
        camera = CAMERA_DSLR;


    } else {
        exit(0);

    }

    splash->showMessage(QObject::tr("Initializing camera ..."),
                           bottom, Qt::white);

    Dialog w(0,camera);

    w.show();
    splash->finish(&w);
    delete splash;
    
    return a.exec();
}
