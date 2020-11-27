// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}

static const int DEFAULT_IMAGE_WIDTH = 320;
static const int DEFAULT_IMAGE_HEIGHT = 240;
static const int DEFAULT_IMAGE_DEPTH = 32;

#include "LidarScene.h"
#include "LidarView.h"

#include <Pacpus/kernel/Log.h>
#include <structure/GenericLidar.h>
#include <structure/LineCloud.h>

#include <boost/assert.hpp>
#include <cmath>
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLWidget>
#include <QResizeEvent>

using namespace pacpus;

DECLARE_STATIC_LOGGER("pacpus.LidarViewer.LidarView");

LidarView::LidarView(QWidget* parent)
    : QGraphicsView(parent)
{    
    PACPUS_LOG_FUNCTION();

    // create GL widget and set it as viewport
    QGLWidget* glWidget = new QGLWidget(QGLFormat(QGL::SampleBuffers), this);
    //QGLContext * glContext = glWidget->context();
    //glContext->makeCurrent();
	resize(680,360);
    BOOST_ASSERT(glWidget);
    setViewport(glWidget);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // create scene 
    mScene = new LidarScene(this);
    BOOST_ASSERT(mScene);
    setScene(mScene);
}

LidarView::~LidarView()
{
    PACPUS_LOG_FUNCTION();
}

void LidarView::display(LidarScan const& scan)
{
    PACPUS_LOG_FUNCTION();

    BOOST_ASSERT(mScene);
    mScene->setScan(scan);
    mScene->update();
}

void LidarView::display(LineCloud3D const& lines)
{
    PACPUS_LOG_FUNCTION();

    BOOST_ASSERT(mScene);
    mScene->setLines(lines);
    mScene->update();
}

void LidarView::resizeEvent(QResizeEvent* rEvent)
{
    if (scene()) {
        scene()->setSceneRect(QRect(QPoint(0, 0), rEvent->size()));
    }
    QGraphicsView::resizeEvent(rEvent);
}
