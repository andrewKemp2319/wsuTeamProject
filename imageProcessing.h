#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types_c.h>

//Code sourced from https://www.opencv-srf.com/p/introduction.html

using namespace std;
using namespace cv;

//Class for constructing coordinates of objects found on board
//Deprecated, no longer used
class Coordinate {
     public:
          int col;
          int row;

          Coordinate(int newY, int newX) {
               row = newY;
               col = newX;
          }
};

/*
Function: calibratePlayerColor
Purpose: get the HSV values for the color marker the player will be using.
Arguments:     N/A
Returns:       int* - pointer to the first HSV value
Side Notes:    the first and second values are lowHue and highHue
               the third and fourth values are lowSaturation and highSaturation
               the fifth and sixth values are lowVibrance and highVibrance
*/
int *calibratePlayerColor() {
     //Try to open camera
     VideoCapture cap(1); //might have to edit what cap is depending on what camera number your camera is 
     static int hsvArray[8]; //the hsv array, which also includes values for the size of the screen

     //if it cant open the camera, return immediately
     if (!cap.isOpened()) 
     {
          cout << "Cannot open the web cam" << endl;

          //set lowHue to be -1 so program can tell it failed.
          hsvArray[0] = -1;
          return hsvArray;
     }

     namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"

     //HSV values
     int iLowH = 0;
     int iHighH = 179;

     int iLowS = 0; 
     int iHighS = 255;

     int iLowV = 0;
     int iHighV = 255;

     //Trackbars for altering HSV values if needed
     //Create trackbars in "Control" window
     createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
     createTrackbar("HighH", "Control", &iHighH, 179);

     createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
     createTrackbar("HighS", "Control", &iHighS, 255);

     createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
     createTrackbar("HighV", "Control", &iHighV, 255);

     cout << "Move the trackbars to calibrate the color, then press ESC." << endl;
     //Process frame
     while (true) {
          //Get frame
          Mat imgOriginal;

          bool bSuccess = cap.read(imgOriginal); // read a new frame from video

          if (!bSuccess) //if not success, break loop
          {
               cout << "Cannot read a frame from video stream" << endl;
               hsvArray[0] = -1;
               return hsvArray;
          }

          //Create frame to be altered
          Mat imgHSV;

          cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

          Mat imgThresholded;

          inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
               
          //morphological opening (remove small objects from the foreground)
          erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
          dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

          //morphological closing (fill small holes in the foreground)
          dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
          erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

          
          //Show images
          imshow("Thresholded Image", imgThresholded); //show the thresholded image
          imshow("Original", imgOriginal); //show the original image

          //if frame looks good, press ESC to end function.
          if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
          {
               cout << "Calibration complete." << endl;
               destroyAllWindows();
               
               hsvArray[0] = iLowH;
               hsvArray[1] = iHighH;
               hsvArray[2] = iLowS;
               hsvArray[3] = iHighS;
               hsvArray[4] = iLowV;
               hsvArray[5] = iHighV;
               hsvArray[6] = imgThresholded.size().width;
               hsvArray[7] = imgThresholded.size().height;
               return hsvArray;
          }
     }

     destroyAllWindows();
}

/*
Function: getImage
Purpose: get an image of the game board, thresholded with HSV
Arguments:  int* - the list of HSV values and the size of the screen
Returns:     Mat - the new image of the game board
*/
Mat getImage(int* hsvPtr) {

     //Try to open the camera
     VideoCapture cap(1); //might have to edit what the cap is

     if ( !cap.isOpened() )  // if not success, exit program
     {
          cout << "Cannot open the web cam" << endl;
          return Mat();
     }

     //HSV values
     int iLowH = *hsvPtr;
     int iHighH = *(hsvPtr+1);

     int iLowS = *(hsvPtr+2); 
     int iHighS = *(hsvPtr+3);

     int iLowV = *(hsvPtr+4);
     int iHighV = *(hsvPtr+5);

     //Get frame
     while (true) {
          Mat imgOriginal;

          bool bSuccess = cap.read(imgOriginal); // read a new frame from video

          if (!bSuccess) //if not success, break loop
          {
               cout << "Cannot read a frame from video stream" << endl;
               return imgOriginal;
          }

          //Create frame to be altered
          Mat imgHSV;

          cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

          Mat imgThresholded;

          inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
               
          //morphological opening (remove small objects from the foreground)
          erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
          dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

          //morphological closing (fill small holes in the foreground)
          dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
          erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

          imshow("OG", imgOriginal);
          imshow("threshold", imgThresholded);

          //if frame looks good, press ESC to end function.
          if (waitKey(30) == 27) {
               destroyAllWindows();
               return imgThresholded;
          }
     }
}

/*
Function: findMoments
Purpose: subtract a new move from the previous one and find the center of the new mark.
Arguments:   Mat - the previous game state
             Mat - the new game state
Returns: Moments - the center of the new move
*/
Moments findMoments(Mat previousGameState, Mat newGameState) {
     //Make a new Mat image and subtract the previous game state from the new one
     Mat newMove;
     subtract(newGameState, previousGameState, newMove);

     //remove noise
     erode(newMove, newMove, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
     dilate(newMove, newMove, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


     //Show new move, press ESC when ready.
     imshow("New Move", newMove);
     cout << "Here's your move. Press ESC to continue." << endl;
     if (waitKey() == 27) {
          Moments moveMoments = moments(newMove);
          destroyAllWindows();
          return moveMoments;
     }
}

/*
Function: showImage
Purpose: display a Mat object and wait for the user to destroy it.
Arguments:  Mat - the image to display
Returns:    N/A
*/
void showImage(Mat img) {
     imshow("", img);
     cout << "Press any key to continue" << endl;
     waitKey();
     destroyAllWindows();
}