// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @file
/// @date created   2013-11-08 12:15:07
/// @author         Marek <firstname.surname@utc.fr>
/// @author         Giovani Bernardes <firstname.surname@utc.fr>
/// @copyright      Copyright (c) UTC/CNRS Heudiasyc 2006 - 2013. All rights reserved.
/// @version        $Id: $
///
/// @todo Brief description of LidarViewer.h.
///
/// @todo Detailed description of LidarViewer.h.

#ifndef LIDARVIEWER_H
#define LIDARVIEWER_H

#include "LidarViewerConfig.h"
#include "structure/structure_velodyne.h"
#include <Pacpus/kernel/ComponentBase.h>
#include "PacpusTools/ShMem.h"
#include <structure/LineCloud.h>
#include "structure/GenericLidar.h"
#include <boost/scoped_ptr.hpp>
#include <QObject>
#include <QThread>
#include "opencv2/core/core.hpp"
#include <QSharedPointer>

namespace pacpus
{

struct LidarScan;

class LIDARVIEWER_API LidarViewer
    : public QObject
    , public ComponentBase // must be after QObject
{
    Q_OBJECT

public:
    LidarViewer(QString name);
    ~LidarViewer();

    /// Starts the component
    virtual void startActivity() /* override */;
    /// Stops the component
    virtual void stopActivity() /* override */;
    /// Configures components
    virtual ComponentBase::COMPONENT_CONFIGURATION configureComponent(XmlComponentConfig config) /* override */;
    void processOccgrid(cv::Mat const& scan);
    void processLines(LineCloud3D const& lines);
    void processScan(LidarScan const& scan);
		void processVelodyne(VelodynePolarData const& velodyne_rec);
	VelodynePolarData velodyne_rec;

//public Q_SLOTS:
protected:
    /// Adds component inputs
    virtual void addInputs() /* override */;
    /// Adds component outputs
    virtual void addOutputs() /* override */;
    
private:

private:
    class Impl;
    boost::scoped_ptr<Impl> mImpl;
	QThread mThread; 
	   double toRad ;

        double d;    // distance reçu par le velodyne
        double X, Y, Z;
        double dxy;  // distance XY
        double alpha;// angle azimuth
        double beta; // angle elevation
		double cosVertAngle ;
        double sinVertAngle ;
        double cosRotAngle ;
        double sinRotAngle ;
        // resolution de l'angle d'ouverture
        double resFov;
		LidarScan scan_rec;
        int intensity_m;
			void*  velodyne_mem;
		ShMem * shmem_velodyne; 	
		int counter;
		LidarLayer layer_m;
		LidarPoint point_m;

};

} // namespace pacpus

#endif // LIDARVIEWER_H
