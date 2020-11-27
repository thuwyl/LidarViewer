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
/// @todo Brief description of LidarScene.h.
///
/// @todo Detailed description of LidarScene.h.

#ifndef LIDARSCENE_H
#define LIDARSCENE_H

#include <structure/GenericLidar.h>
#include <structure/LineCloud.h>

#include <boost/scoped_ptr.hpp>
#include <QGraphicsScene>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>


class QDialog;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QPainter;
class QRectF;
class QWidget;

namespace pacpus
{

class LidarScene
    : public QGraphicsScene
{
    Q_OBJECT

public:
    LidarScene(QObject* parent = 0);
    ~LidarScene();

    void setBackgroundColor(QColor const& color);

public Q_SLOTS:
    void setGridEnabled(bool gridEnabled);
    void setLidarEnabled(bool lidarEnabled);

    void setScan(LidarScan const& scan);
    void setLines(LineCloud3D const& lines);
    void setShowLines(bool showLines);

protected:
    QDialog* createDialog(QString const& windowTitle, QWidget* parent = 0) const;

    //void drawForeground(QPainter* painter, QRectF const& rect) /* override */;
    void drawBackground(QPainter* painter, QRectF const& rect) /* override */;

    void resetView();
    
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent);
    void keyPressEvent(QKeyEvent* event);

    void drawCameraTargetPoint();
    void drawLines();
    void drawScan();
	void drawLDMRS_Scan();
    void drawGrid(float length, float step, float lineWidth);
    void drawCircle(float radius, QVector3D center, QVector3D normal, int num_segments);
    void drawFrame(float lineWidth, float length);
    //void drawScale(QPainter* painter, float length);
    
private:
    enum CameraMovement {
        CM_None,
        CM_Up,
        CM_Down,
        CM_Left,
        CM_Right
    };
    void moveCameraUpDown(CameraMovement movement);
    void moveCameraSideways(CameraMovement movement);
    
    enum CameraZoom {
        CZ_None,
        CZ_In = 1,
        CZ_Closer = 1,
        CZ_Out = 2,
        CZ_Farer = 2
    };
    void zoomCamera(float ratio);

private:
    boost::scoped_ptr<QWidget> mControls;

    LineCloud3D mLines;
    LidarScan m_scan;
    QColor m_backgroundColor;
	
    float m_pointSize;
    bool m_displayCamera, m_displayGrid, m_displayLidar, mDisplayLines;

    int m_lastTime;
    int m_mouseEventTime;

    //QGraphicsRectItem *m_lightItem;

    QVector3D m_cameraEye, m_cameraRef, m_cameraUp;
    QMatrix4x4 m_projection, m_modelView;
    QList<QColor> m_pointColors;

    float m_fovx, m_znear, m_zfar;

    QVector2D m_camera_sensitivity;
};

} // namespace pacpus

#endif // LIDARSCENE_H
