// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}

#include "LidarViewer.h"
#include "LidarViewerImpl.h"

#include <Pacpus/kernel/ComponentFactory.h>
#include <Pacpus/kernel/Log.h>
#include <structure/GenericLidar.h>
#include <structure/LineCloud.h>

using namespace pacpus;
using namespace std;

DECLARE_STATIC_LOGGER("pacpus.LidarViewer");

/// Constructs a static component factory
static ComponentFactory<LidarViewer> sFactory("LidarViewer");

//////////////////////////////////////////////////////////////////////////
LidarViewer::LidarViewer(QString name)
    : ComponentBase(name)
{   
    LOG_TRACE("constructor(" << name << ")");

    mImpl.reset(new Impl(this));
    string velodyne_source="velodynedbtply"; 
	shmem_velodyne = new pacpus::ShMem(velodyne_source.c_str(), sizeof(VelodynePolarData));
	velodyne_mem = malloc(sizeof(VelodynePolarData)); 
	

    //addParameters()
    //("parameter-name", value<ParameterType>(&mImpl->mParameterVariable)->required(), "parameter description")
    //("parameter-name", value<ParameterType>(&mImpl->mParameterVariable)->default_value(0), "parameter description")
    //;
	counter=1;
}

LidarViewer::~LidarViewer()
{
    LOG_TRACE("destructor");
}

//////////////////////////////////////////////////////////////////////////
void LidarViewer::addInputs()
{
    // must inherit from QObject to use addInput
    addInput<LidarScan, LidarViewer>("points", &LidarViewer::processScan);
    addInput<LidarScan, LidarViewer>("scan", &LidarViewer::processScan);
    addInput<LineCloud3D, LidarViewer>("lines", &LidarViewer::processLines);
	addInput<VelodynePolarData, LidarViewer>("velodyne", &LidarViewer::processVelodyne);
	addInput<cv::Mat, LidarViewer>("occgrid", &LidarViewer::processOccgrid);
	//addInput<TYPE_GEN, LidarViewer>("scan_ldmrs", &LidarViewer::processLDMRS_Scan);
	//addInput<TYPE_GEN, LidarViewer>("scan_lms511", &LidarViewer::processLDMRS_Scan);
}

void LidarViewer::addOutputs()
{
    // no outputs
}

//////////////////////////////////////////////////////////////////////////
void LidarViewer::startActivity()
{
    mImpl->start();
    moveToThread(&mThread);
    mThread.start();
}

void LidarViewer::stopActivity()
{
    mImpl->stop();
	QMetaObject::invokeMethod(&mThread, "quit");
	delete shmem_velodyne;
}

//////////////////////////////////////////////////////////////////////////
ComponentBase::COMPONENT_CONFIGURATION LidarViewer::configureComponent(XmlComponentConfig config)
{
    return ComponentBase::CONFIGURED_OK;
}

//////////////////////////////////////////////////////////////////////////
void LidarViewer::processScan(LidarScan const& scan)
{
    mImpl->processScan(scan);
}

void LidarViewer::processOccgrid(cv::Mat const& scan)
{
   // mImpl->processScan(scan);
}
//////////////////////////////////////////////////////////////////////////
void LidarViewer::processLines(LineCloud3D const& lines)
{
    mImpl->processLines(lines);
}


void LidarViewer::processVelodyne(VelodynePolarData const& velodyne_re)
{
	counter++;
	if (counter==30)
	{
		//shmem_velodyne->read(velodyne_mem,sizeof(VelodynePolarData));
	//VelodynePolarData lidardata;
	//memcpy( &lidardata, velodyne_mem, sizeof(VelodynePolarData));
	
	
	   toRad = 3.1415926/180.0;

     
       resFov = 1.33 * toRad;
	    beta = 10.67* toRad - resFov * 32;
    for (int j=0; j < 32; ++j)
     {
				
	   
        for (int i=0; i < velodyne_re.range; ++i)
        {
            alpha = ((velodyne_re.polarData[i].angle) / 100.0) * toRad;
		///if (j==10)	cout<<"angle  "<<i<<"  :  "<<velodyne_re.polarData[i].angle/ 100.0<<endl;
		
               
            
            // boucle pour les 32 impacts
           
            
				//cout<<"distance"<<lidardata.polarData[i].rawPoints[j].distance<<endl;
				
			 
			 d = (velodyne_re.polarData[i].rawPoints[j].distance) / 500.0 ; // increments de 2mm => /500 pour avoir des m + correction (en cm)
			//LOG_INFO(d);
			//LOG_INFO(i);
                // suppression des points trop proches
                if (d < 1.5)
                    continue;

                // Application des corrections du LIDAR (cf. doc velodyne)  
                 cosVertAngle = cos(beta);
                 sinVertAngle = sin(beta);

                 cosRotAngle = cos(alpha );
                 sinRotAngle = sin(alpha);

                //double hOffsetCorr = m_hOffsetCor[j+k] / 100.0;
               // double vOffsetCorr = m_vOffsetCor[j+k] / 100.0;
	
                dxy = d * cosVertAngle;
                X = dxy * sinRotAngle ;  // x
                Y = dxy * cosRotAngle  ; // y
                Z = d   * sinVertAngle ; // z
                // add point
            

                // on met a jour beta
             

			intensity_m=velodyne_re.polarData[j].rawPoints[i].intensity;
		
			point_m.x=X;
			point_m.y=Y;
			point_m.z=Z;
			point_m.intensity=intensity_m;
			layer_m.points.push_back(point_m);
			layer_m.angle=beta;
			layer_m.id=j;
		  

            }

		 beta += resFov; // 26.8 FOV vertical
		scan_rec.layers.push_back(layer_m);

		//cout<<"layers number:  "<<scan_rec.getLayerCount()<<endl;
        }
		

	}

		processScan(scan_rec);
		//LOG_INFO("point yyyyyy=  "<<scan_rec.layers[2].points[6].x);	
	///mImpl->processScan(scan_rec);
	
	//	std::cout << "start angle is " << scan_rec.layers[5].points[4].x << std::endl;

}