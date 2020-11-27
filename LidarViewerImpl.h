// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @file
/// @date created   2013-11-08 12:15:07
/// @author         Marek Kurdej <firstname.surname@utc.fr>
/// @author         Giovani Bernardes <firstname.surname@utc.fr>
/// @copyright      Copyright (c) UTC/CNRS Heudiasyc 2006 - 2013. All rights reserved.
/// @version        $Id: $
///
/// @todo Brief description of LidarViewerImpl.h.
///
/// @todo Detailed description of LidarViewerImpl.h.

#ifndef LIDARVIEWERIMPL_H
#define LIDARVIEWERIMPL_H

#include "LidarView.h"
#include "LidarViewer.h"
//#include <datatypes/Scan.hpp>
#include <QSharedPointer>
namespace pacpus
{

class LidarViewer::Impl
    : public QObject
{
    Q_OBJECT

public:
    Impl(LidarViewer* parent);
    ~Impl();

    void start();
    void stop();

    void processLines(LineCloud3D const& lines);
    void processScan(LidarScan const& scan);


private:
    LidarViewer* mParent;
    LidarView mView;

	//QSharedPointer<LidarScan> lidarScan;
	LidarScan *lidarScan;

	bool m_isRunning;
};

} // namespace pacpus

#endif // LIDARVIEWERIMPL_H
