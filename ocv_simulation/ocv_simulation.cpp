#include <stdio.h>
#include<iostream>
#include <opencv2/opencv.hpp>
#include "spline.h"

using namespace cv;
using namespace std;

const int alpha_slider_max = 80;
int alpha_slider;

const int left_slider_max = 600;
const int right_slider_max = 600;

int left_slider1;
int left_slider2;
int left_slider3;
int left_slider4;

int right_slider1;
int right_slider2;
int right_slider3;
int right_slider4;

//callbecks from trackbars
void left_trackbar( int, void* )
{
   left_slider1=left_slider1;
   left_slider2=left_slider2;
   left_slider3=left_slider3;
   left_slider4=left_slider4;
}
void right_trackbar(int, void*)
{
    right_slider1=right_slider1;
    right_slider2=right_slider2;
    right_slider3=right_slider3;
    right_slider4=right_slider4;
}


int main(int argc, char** argv )
{
    VideoCapture cap(0); // open the default camera
        if(!cap.isOpened())  // check if we succeeded
            return -1;

    Mat frame2;

        //camera params
    int dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    int dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    double fps = cap.get(CV_CAP_PROP_FPS);
       // cout<<"Rozdzielczosc: "<<dWidth<<"x"<<dHeight <<endl<<"liczba fps: "<<fps<<endl;;//wyswietl parametry ramki


        //trackbars definitions
    namedWindow("Mateuszek", 1);

    namedWindow("left border line", WINDOW_NORMAL);
    createTrackbar( "x1", "left border line", &left_slider1, left_slider_max, left_trackbar );
    createTrackbar( "x2", "left border line", &left_slider2, left_slider_max, left_trackbar );
    createTrackbar( "x3", "left border line", &left_slider3, left_slider_max, left_trackbar );
    createTrackbar( "x4", "left border line", &left_slider4, left_slider_max, left_trackbar );

    namedWindow("right border line", WINDOW_NORMAL);
    createTrackbar( "x1", "right border line", &right_slider1, right_slider_max, right_trackbar );
    createTrackbar( "x2", "right border line", &right_slider2, right_slider_max, right_trackbar );
    createTrackbar( "x3", "right border line", &right_slider3, right_slider_max, right_trackbar );
    createTrackbar( "x4", "right border line", &right_slider4, right_slider_max, right_trackbar );


           //detected points
    vector<Point> detected;
            //lewa line
    detected.push_back(Point(300,5));
    detected.push_back(Point(180,160));
    detected.push_back(Point(200,320));
    detected.push_back(Point(300,470));

           //right line
    detected.push_back(Point(400,5));
    detected.push_back(Point(280,160));
    detected.push_back(Point(300,320));
    detected.push_back(Point(300,470));


    vector<Point> trajectory;
    for(int i =0;i<4;i++){
        trajectory.push_back(Point((detected[i+4].x+detected[i].x),detected[i].y));
    }

    vector<Point> path_planner;
    for(int i =0;i<2;i++){
         path_planner.push_back(Point(1,1));
    }

    path_planner.push_back(Point(dWidth/2,480));//middle of view


    //polyfill variable
     Point fill[1][2*dHeight];
     const Point* ppt[1] = { fill[0] };
     int npt[] = { 2*dHeight };
     Mat poly = Mat::zeros( 480, 640, CV_8UC3 );
     Mat dst;

     //line drawer
     Point pom;
     Vec3b  color;

            //spline approx variable
     std::vector<double> X_r(4), Y_r(4);
     std::vector<double> X_l(4), Y_l(4);
     std::vector<double> X_m(4),Y_m(4);
     std::vector<double> X_t(3),Y_t(3);

     //spline classes
     tk::spline right_spline;
     tk::spline left_spline;
     tk::spline middle_spline;
     tk::spline path_planner_spline;


    //tangent variables
     double middle_spline_deriv;
     double middle_tangent_b;
     double path_planner_deriv;
     double path_planner_tangent_b;

     double trajectory_angle_deg;
     double trajectory_angle;
     double path_planner_angle;
     double path_planner_angle_deg;

     string label;
//main loop -------------------------------------------------------------------------------------------------------------
        for(;;)
        {
            //get sliders positions
            detected[0].x = left_slider1+20;
            detected[1].x = left_slider2+20;
            detected[2].x = left_slider3+20;
            detected[3].x = left_slider4+20;
            detected[4].x = right_slider1+30;
            detected[5].x = right_slider2+30;
            detected[6].x = right_slider3+30;
            detected[7].x = right_slider4+30;


            for(int i =0;i<4;i++){
                trajectory[i].x=(detected[i+4].x+detected[i].x)/2;
                trajectory[i].y=detected[i].y;

            }

            for(int i =0;i<2;i++){
                path_planner[i].x = trajectory[i].x;
                 path_planner[i].y = trajectory[i].y;
             }

            cap>> frame2;



            //
            for(int n=0;n<detected.size();n++){
                for(int i=0;i<8;i++){
                    pom.y = detected[n].y-1+i;

                    for(int j=0;j<3;j++){
                         pom.x=detected[n].x-1+j;

                         color = frame2.at<Vec3b>(pom);
                         color.val[0] = 0;
                         color.val[1] = 255;
                         color.val[2] =255;
                         frame2.at<Vec3b>(pom) = color;
                     }
                 }
             }




             for(int i=0;i<4;i++){
                X_l[i]=detected[i].y;
                X_r[i]=detected[i+4].y;

                Y_l[i]=detected[i].x;
                Y_r[i]= detected[i+4].x;

                X_m[i]=trajectory[i].y;
                Y_m[i]=trajectory[i].x;
             }

            for(int i=0;i<3;i++){
                X_t[i]=path_planner[i].y;
                Y_t[i]=path_planner[i].x;
            }

            left_spline.set_points(X_l,Y_l);    // currently it is required that X is already sorted
            right_spline.set_points(X_r,Y_r);
            middle_spline.set_points(X_m,Y_m);
            path_planner_spline.set_points(X_t,Y_t);


            //draw left spline
            for (int i=0;i<dHeight;i++){
               pom.y = i;
               pom.x=left_spline(i);//a*i*i+b*i+c+200;
               fill[0][i].y=pom.y;
               fill[0][i].x=pom.x;

               color = frame2.at<Vec3b>(pom);
               color.val[0] = 0;
               color.val[1] = 255;
               color.val[2] =255;
               frame2.at<Vec3b>(pom) = color;
            }
           //draw right spline
              for (int i=0;i<dHeight;i++){
                pom.y = i;
                pom.x=right_spline(i);//a*i*i+b*i+c+200;
                fill[0][2*dHeight-i-1].y=pom.y;
                fill[0][2*dHeight-i-1].x=pom.x;

                 color = frame2.at<Vec3b>(pom);
                 color.val[0] = 0;
                 color.val[1] = 255;
                 color.val[2] =255;
                 frame2.at<Vec3b>(pom) = color;
               }

                      //draw trajectory
               for (int i=0;i<dHeight;i++){
                 pom.y = i;
                 pom.x=middle_spline(i);//a*i*i+b*i+c+200;

                 color = frame2.at<Vec3b>(pom);
                 color.val[0] = 250;
                 color.val[1] = 15;
                 color.val[2] =10;
                 frame2.at<Vec3b>(pom) = color;
                }

                      //draw path planner
                for (int i=0;i<dHeight;i++){
                  pom.y = i;
                  pom.x=path_planner_spline(i);//a*i*i+b*i+c+200;

                  color = frame2.at<Vec3b>(pom);
                  color.val[0] = 0;
                  color.val[1] = 0;
                  color.val[2] =255;
                  frame2.at<Vec3b>(pom) = color;
                 }

                //tangent calc
                 middle_spline_deriv = middle_spline.deriv(1,trajectory[1].y);
                 middle_tangent_b =-middle_spline_deriv*trajectory[1].y+middle_spline(trajectory[1].y);

                 path_planner_deriv = path_planner_spline.deriv(1,path_planner[1].y);
                 path_planner_tangent_b =-path_planner_deriv*path_planner[1].y+path_planner_spline(path_planner[1].y);

                      //angle calc
                 trajectory_angle = atan2( trajectory[1].x - (middle_spline_deriv*dHeight+middle_tangent_b),trajectory[1].y - dHeight-1);

                 if (trajectory_angle>=0)
                    trajectory_angle_deg = 90 - trajectory_angle * 90 / 3.14159265358979323846;
                 else
                    trajectory_angle_deg = (-trajectory_angle * 90 / 3.14159265358979323846)-90;

                 path_planner_angle = atan2( path_planner[1].x - (path_planner_deriv*dHeight+path_planner_tangent_b),path_planner[1].y - dHeight-1);

                 if(path_planner_angle >=0)
                    path_planner_angle_deg = 90 - path_planner_angle * 90 / 3.14159265358979323846;
                 else
                     path_planner_angle_deg = (-path_planner_angle * 90 / 3.14159265358979323846)-90;



                 //draw angle result
                 label = "traj_ang: "+ std::to_string(trajectory_angle_deg);
                 putText(frame2, label, Point(450, 40), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0,255,0), 1.0);
                 label = "path_ang: "+ std::to_string(path_planner_angle_deg);
                 putText(frame2, label, Point(450, 60), FONT_HERSHEY_SIMPLEX, 0.5 , CV_RGB(0,255,250), 1.0);

                 //draw trajectory tangent
                 for (int i=0;i<dHeight;i++){
                          pom.y = i;
                          pom.x=middle_spline_deriv*i+middle_tangent_b;//a*i*i+b*i+c+200;

                          color = frame2.at<Vec3b>(pom);
                          color.val[0] = 0;
                          color.val[1] = 255;
                          color.val[2] =0;
                          frame2.at<Vec3b>(pom) = color;
                   }


                    //draw path planner angle
                  for (int i=0;i<dHeight;i++){
                          pom.y = i;
                          if(path_planner_deriv*i+path_planner_tangent_b <0)
                          pom.x=0;
                          else
                          pom.x = path_planner_deriv*i+path_planner_tangent_b;

                          color = frame2.at<Vec3b>(pom);
                          color.val[0] = 250;
                          color.val[1] = 255;
                          color.val[2] =0;
                          frame2.at<Vec3b>(pom) = color;
                   }

//------------------------------------------------

                    //poly fill
                   poly = Mat::zeros( 480, 640, CV_8UC3 );
                   fillPoly( poly,ppt, npt, 1, Scalar( 5, 100,100), 4 ); //fill poly
                   addWeighted(frame2, 0.6, poly, 0.4, 0.0, dst); //transparency of polyfill

                    imshow("Mateuszek", dst);
                    if(waitKey(30) >= 0) break;
        }

    return 0;
}


//----------------------------------------------------------------------------------------------------------------------------------
