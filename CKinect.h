



#ifndef INC_CKinect_h
#define INC_CKinect_h

#include "Kinect.h"

static const int        cDepthWidth = 512;
static const int        cDepthHeight = 424;
static const int        cColorWidth = 1920;
static const int        cColorHeight = 1080;

//buffer
extern CameraSpacePoint kinectVerticies[cDepthHeight*cDepthWidth];

HRESULT InitializeDefaultSensor();
void update();
void get_head_position(CameraSpacePoint* &ret_val);

#endif