// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}

#include "LidarViewer.h"
#include "LidarViewerImpl.h"

#include <Pacpus/kernel/Log.h>
#include <structure/GenericLidar.h>
//#include "structure/structure_telemetre.h"

//using namespace boost;
using namespace pacpus;
//using namespace std;

DECLARE_STATIC_LOGGER("pacpus.LidarViewer.Impl");

//////////////////////////////////////////////////////////////////////////
LidarViewer::Impl::Impl(LidarViewer* parent)
    : mParent(parent)
{
	lidarScan = new LidarScan(4);
		
	lidarScan->layers[0].id = 1;
	lidarScan->layers[1].id = 2;
	lidarScan->layers[2].id = 3;
	lidarScan->layers[3].id = 4;	
	
}

//////////////////////////////////////////////////////////////////////////
void LidarViewer::Impl::start()
{
	m_isRunning = false;
    mView.show();
}

void LidarViewer::Impl::stop()
{	
    mView.setVisible(false);
	mView.close();

	while(m_isRunning == true) ;
		//msleep(10);

	LOG_INFO("stopped component '" << mParent->getName() << "'");
}

LidarViewer::Impl::~Impl()
{
    delete lidarScan;
}

//////////////////////////////////////////////////////////////////////////
//void LidarViewer::Impl::outputData()
//{
//    OutputType data = generateData();
//    checkedSend(mDataOutput, data);
//}

//////////////////////////////////////////////////////////////////////////
void LidarViewer::Impl::processScan(LidarScan const& scan)
{
    mView.display(scan);
}

void LidarViewer::Impl::processLines(LineCloud3D const& lines)
{
    mView.display(lines);
}

//////////////////////////////////////////////////////////////////////////
