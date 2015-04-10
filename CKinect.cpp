/*
* File:   main.cpp
* Author: LW520
*
* Created on November 23, 2014, 5:13 PM
*/

#include <cstdlib>
#include <iostream>
#include "Kinect.h"
#include "CKinect.h"

using namespace std;

// Current Kinect
IKinectSensor*          m_pKinectSensor;
ICoordinateMapper*      m_pCoordinateMapper;
DepthSpacePoint*        m_pDepthCoordinates;


// Frame reader
IMultiSourceFrameReader*m_pMultiSourceFrameReader;

// Depth reader
IDepthFrameReader*      m_pDepthFrameReader;

//body reader
IBodyFrameReader * body_rdr;
IBodyFrameSource * body_src;

RGBQUAD * m_pDepthRGBX[cDepthWidth * cDepthHeight];

//kinect vertex buffer
CameraSpacePoint kinectVerticies[cDepthHeight*cDepthWidth];

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

void get_head_position(CameraSpacePoint* &ret_val){

	CameraSpacePoint* temp = ret_val;
	ret_val = 0;//starts as null

	IBodyFrame* pBodyFrame = NULL;

	HRESULT hr = body_rdr->AcquireLatestFrame(&pBodyFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);

		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			if (_countof(ppBodies) > 0){

				BOOLEAN bTracked = false;
				hr = ppBodies[0]->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					Joint joints[JointType_Count];
					hr = ppBodies[0]->GetJoints(_countof(joints), joints);

					if (SUCCEEDED(hr))
					{
						*temp = joints[JointType_Head].Position;
						ret_val = temp;//becomes not null if success
					}
				}
			}
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
	}

	SafeRelease(pBodyFrame);

}

void update()
{


	if (!m_pDepthFrameReader)
	{
		return;
	}

	IDepthFrame* pDepthFrame = NULL;

	HRESULT hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		USHORT nDepthMinReliableDistance = 0;
		USHORT nDepthMaxDistance = 0;
		UINT nBufferSize = 0;
		UINT16 *pBuffer = NULL;

		hr = pDepthFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
		}

		if (SUCCEEDED(hr))
		{
			// In order to see the full range of depth (including the less reliable far field depth)
			// we are setting nDepthMaxDistance to the extreme potential depth threshold
			nDepthMaxDistance = 256;

			// Note:  If you wish to filter by reliable depth distance, uncomment the following line.
			//// hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxDistance);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		}

		if (SUCCEEDED(hr))
		{


			m_pCoordinateMapper->MapDepthFrameToCameraSpace(cDepthHeight*cDepthWidth, pBuffer, cDepthHeight*cDepthWidth, kinectVerticies);

			//int i = 0;
			//cout << "(" << kinectVerticies[i].X << "," << kinectVerticies[i].Y << "," << kinectVerticies[i].Z << ")" << endl;
			//cout << "NEW FRAME" << endl;

		}

		SafeRelease(pFrameDescription);
	}

	SafeRelease(pDepthFrame);
}



HRESULT InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get the depth reader
		IDepthFrameSource* pDepthFrameSource = NULL;

		m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);

		hr = m_pKinectSensor->Open();

		//depth

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//body

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&body_src);
		}

		if (SUCCEEDED(hr))
		{
			hr = body_src->OpenReader(&body_rdr);
		}

		SafeRelease(pDepthFrameSource);
	}

	return hr;
}

