// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @file
/// @date created   2013-11-08 15:39:37
/// @author         Marek Kurdej <firstname.surname@utc.fr>
/// @author         Giovani Bernardes <firstname.surname@utc.fr>
/// @copyright      Copyright (c) UTC/CNRS Heudiasyc 2006 - 2013. All rights reserved.
/// @version        $Id: $
///
/// @todo Brief description of LidarView.h.
///
/// @todo Detailed description of LidarView.h.

#ifndef LIDARVIEW_H
#define LIDARVIEW_H

#include <structure/LineCloud.h>

#include <QGraphicsView>
#include <QMatrix4x4>
#include <QVector3D>

class QResizeEvent;

namespace pacpus
{
    
class LidarScene;
struct LidarScan;

class LidarView
    : public QGraphicsView
{
    Q_OBJECT

public:
    LidarView(QWidget* parent = 0);
    ~LidarView();

public Q_SLOTS:
    void display(LidarScan const& scan);
    void display(LineCloud3D const& lines);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    LidarScene* mScene;
};

} // namespace pacpus

#endif // LIDARVIEW_H
