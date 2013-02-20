# NegativeViewer #

It is a tool for viewing photographic negatives and slides by using a webcam or digital single lens reflex camera. You can apply various transformations on the live image like inverting and rotating a picture, croping the frame and saving the result.

# Abstract #

The aim of this project is to construct an easy to use station for viewing photographic negatives and slides. 
The station requires a digital camera but should be independent of a certain kind of camera. Originally it had been planned to be used with the common HDMI connection. But because of defective hardware it had to be changed to USB.

A computer program has been developed which works with webcams as well as with digital single lens reflex cameras. This program enables the user to invert the picture and to apply different functions like automatic cropping and rotating. For example the image can be saved for a photo database or high resolution digital copy.

# About #

This project uses OpenCV 2.4 and gPhoto 2.5 and runs on Linux. The GUI uses QT4. QT Creator has been used as IDE. 

Tested cameras: Logitech C920 HD webcam, Nikon D300 and Canon EOS 5D Mark II.

# Documentation #
[See the PDF](https://github.com/JohannesBiermann/NegativeViewer/blob/master/documentation/2013-NegativeViewer-Documentation.pdf) (only in German).

# Construction and Software #
The construction consists of a light box and a repro tripod.

![Test set-up](https://raw.github.com/JohannesBiermann/NegativeViewer/master/documentation/img/test_set-up.jpg)
Test set-up: light box, repro tripod. On the light box there is a photographic negative and on the screen you can see the inverted live image.

![Screenshot](https://raw.github.com/JohannesBiermann/NegativeViewer/master/documentation/img/Screenshot-Software.png)
Screenshot of the application


# VMware Image #
You can download a complete VMware image with everything preinstalled including development environment [here](http://johannesb.eu/files/NegativeViewerVMWareImage.zip) (Lubuntu 12.10, 64 Bit, 1,14 GB download size).
Please connect the webcam or DSLR into VMware.

![](https://raw.github.com/JohannesBiermann/NegativeViewer/master/documentation/img/vmdevice.png)
Connect camera to VMware

# Install #
You need OpenCV 2.4.x, gPhoto 2.5.x including development packages and also QT4 SDK (4.8) packages. 

To compile it, use QT Creator and open the project file or on command line just simply execute

*make*

# License #
Released under GPL v3.

# Perspective #
HDR functionality, support more DSLR camera features, HDMI support, Mac OS X port
