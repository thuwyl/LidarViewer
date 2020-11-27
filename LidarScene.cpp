// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}

#include "LidarScene.h"

#include <Pacpus/kernel/Log.h>

#include <boost/foreach.hpp>
#include <QCheckBox>
#include <QColorDialog>
#include <QDialog>
#include <QGLWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QOpenGLFunctions>
#include <QPaintEngine>
#include <QPainter>
#include <QRectF>

#include <QTextDocument>

using namespace pacpus;

DECLARE_STATIC_LOGGER("pacpus.LidarViewer.Impl");

static const int kCameraTargetPointSize = 5;
static const int kDefaultPointSize = 2;

static const float kDefaultFOVX = 70;
static const float kDefaultZNear = 0.01f;
static const float kDefaultZFar = 1000;

static const int kFrameLineWidth = 3;
static const int kFrameLength = 5;

static const float kGridLength = 101;
static const float kGridStep = 10;
static const float kGridLineWidth = 0.5;

static const int kTranslateStep = 1;

static const QRgb kDefaultBackgroundColor = qRgb(0.5f,0.8f , 0.7f);

LidarScene::LidarScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_pointSize(kDefaultPointSize)
    , m_displayCamera(true)
    , m_displayGrid(true)
    , mDisplayLines(false)
    , m_displayLidar(true)
    , m_fovx(kDefaultFOVX)
    , m_znear(kDefaultZNear)
    , m_zfar(kDefaultZFar)
    , mControls(NULL)
{
    glEnable(GL_BLEND);

    resetView();

    setBackgroundColor(kDefaultBackgroundColor);

    for (int i = Qt::red; i <= Qt::darkYellow; ++i) {
        m_pointColors.append(QColor(Qt::GlobalColor(i)));
    }
    mControls.reset(createDialog(tr("Controls"), /*parent=*/ NULL));

    {
        QCheckBox* lidarCheckBox = new QCheckBox(tr("Show lidar"), /*parent=*/ mControls.get());
        lidarCheckBox->setChecked(m_displayLidar);
        connect(lidarCheckBox, &QCheckBox::toggled, this, &LidarScene::setLidarEnabled);
        mControls->layout()->addWidget(lidarCheckBox);
    }
    {
        QCheckBox* gridCheckBox = new QCheckBox(tr("Show grid"), /*parent=*/ mControls.get());
        gridCheckBox->setChecked(m_displayGrid);
        connect(gridCheckBox, &QCheckBox::toggled, this, &LidarScene::setGridEnabled);
        mControls->layout()->addWidget(gridCheckBox);
    }
    {
        QCheckBox* linesCheckBox = new QCheckBox(tr("Show lines"), /*parent=*/ mControls.get());
        linesCheckBox->setChecked(mDisplayLines);
        connect(linesCheckBox, &QCheckBox::toggled, this, &LidarScene::setShowLines);
        mControls->layout()->addWidget(linesCheckBox);
    }

    QGraphicsScene::addWidget(mControls.get());

    // Position Dialogue
    QPointF pos(5, 5);
    BOOST_FOREACH(QGraphicsItem * item, items()) {
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

        QRectF const rect = item->boundingRect();
        item->setPos(pos.x() - rect.x(), pos.y() - rect.y());
        pos += QPointF(0, 5 + rect.height());
    }
}

LidarScene::~LidarScene()
{
}

QDialog* LidarScene::createDialog(QString const& windowTitle, QWidget* parent) const
{
    QDialog* dialog = new QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    dialog->setWindowOpacity(0.8);
    dialog->setWindowTitle(windowTitle);
    dialog->setLayout(new QVBoxLayout(/*parent=*/ dialog));

    return dialog;
}

void LidarScene::setBackgroundColor(QColor const& color)
{
    LOG_INFO("setBackgroundColor: color=["
        << " R:" << color.red()
        << " G:" << color.green()
        << " B:" << color.blue()
        << "]"
    );

    //QColor const color = QColorDialog::getColor(m_backgroundColor);    
    m_backgroundColor = color;
    QBrush brush(m_backgroundColor);
    setBackgroundBrush(m_backgroundColor);
    update();
}


void LidarScene::setLines(LineCloud3D const& lines)
{
    mLines = lines;
}

void LidarScene::setScan(LidarScan const& scan)
{
    m_scan = scan;
}

void LidarScene::setShowLines(bool showLines)
{
    mDisplayLines = showLines;
}

void LidarScene::setGridEnabled(bool gridEnabled)
{
    m_displayGrid = gridEnabled;
}

void LidarScene::setLidarEnabled(bool lidarEnabled)
{
    m_displayLidar = lidarEnabled;
}

//void LidarScene::drawForeground(QPainter* /*painter*/, QRectF const& /*rect*/)
//{
//}

void LidarScene::drawBackground(QPainter* painter, QRectF const& /*rect*/)
{
    if ((painter->paintEngine()->type() != QPaintEngine::OpenGL)
        && (painter->paintEngine()->type() != QPaintEngine::OpenGL2)) {
        LOG_WARN("LidarScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view");
        return;
    }

    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), m_backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
        m_projection.setToIdentity();
        m_projection.perspective(m_fovx, width() / height(), m_znear, m_zfar);
        glLoadMatrixf(m_projection.data());

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
            m_modelView.setToIdentity();
            m_modelView.lookAt(/*eye=*/ m_cameraEye, /*center=*/ m_cameraRef, /*up=*/ m_cameraUp);
            glLoadMatrixf(m_modelView.data());

            // draw scale
            //drawScale(painter, kFrameLength);
            // draw XYZ-axis frame
            drawFrame(kFrameLineWidth, kFrameLength);
            // draw camera target point
            if (m_displayCamera) {
                drawCameraTargetPoint();
            }
            // draw grid
            if (m_displayGrid) {
                drawGrid(kGridLength, kGridStep, kGridLineWidth);
            }
            // draw lidar scan
            if (m_displayLidar) {
                drawScan();
            }
            if (mDisplayLines) {
                drawLines();
            }
        }
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
    }
    glPopMatrix();
}

void LidarScene::resetView()
{
    // camera eye above the center point
    m_cameraEye = QVector3D(0, 0, 50);
    // looking at the center point
    m_cameraRef = QVector3D(0, 0, 0);
    // FIXME: up vector?
    m_cameraUp = QVector3D(1, 0, 0);
}

void LidarScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mmEvent)
{
    QGraphicsScene::mouseMoveEvent(mmEvent);
    if (mmEvent->isAccepted()) {
        return;
    }

    Qt::MouseButtons handledButtons = Qt::LeftButton | Qt::RightButton | Qt::MiddleButton;
    if (!(mmEvent->buttons() & handledButtons)) {
        return;
    }
    
    QPointF const delta = mmEvent->scenePos() - mmEvent->lastScenePos();
    QMatrix4x4 rot;
    float anglex = delta.x() * -1;
    if (mmEvent->buttons() & Qt::LeftButton) {
        float angley = delta.y() * 1;

        QVector3D viewVector = m_cameraEye - m_cameraRef;
        rot.rotate(anglex, m_cameraUp);
        rot.rotate(angley, QVector3D::crossProduct(viewVector, m_cameraUp));

        m_cameraRef = m_cameraEye - rot * viewVector;
    }
    if (mmEvent->buttons() & Qt::RightButton) {
        float angley = delta.y() * -1;

        QVector3D viewVector = m_cameraRef - m_cameraEye;
        rot.rotate(anglex, m_cameraUp);
        rot.rotate(angley, QVector3D::crossProduct(viewVector, m_cameraUp));

        m_cameraEye = m_cameraRef - rot * viewVector;
    }
    if (mmEvent->buttons() & Qt::MiddleButton) {
        float ratio = std::pow(1.2, delta.y() / 80);
        zoomCamera(ratio);
        
        QVector3D viewVector = m_cameraRef - m_cameraEye;
        rot.rotate(anglex, viewVector);
    }
    m_cameraUp = rot * m_cameraUp;

    mmEvent->accept();
    update();
}

void LidarScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
    if (event->isAccepted()) {
        return;
    }

    m_displayCamera = true;
    event->accept();
    update();
}

void LidarScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    if (event->isAccepted()) {
        return;
    }

    m_displayCamera = false;
    event->accept();
    update();
}

void LidarScene::wheelEvent(QGraphicsSceneWheelEvent* wEvent)
{
    QGraphicsScene::wheelEvent(wEvent);
    if (wEvent->isAccepted()) {
        return;
    }

    float ratio = std::pow(1.2, -wEvent->delta() / 120);
    zoomCamera(ratio);

    wEvent->accept();
    update();
}

void LidarScene::keyPressEvent(QKeyEvent* kEvent)
{
    QGraphicsScene::keyPressEvent(kEvent);
    if (kEvent->isAccepted()) {
        return;
    }

    int keyCode = kEvent->key();
    LOG_DEBUG("key pressed: " << keyCode);

    switch (keyCode) {
    case Qt::Key_Up:
        moveCameraUpDown(CM_Up);
        break;
    case Qt::Key_Down:
        moveCameraUpDown(CM_Down);
        break;
    case Qt::Key_Left:
        moveCameraSideways(CM_Left);
        break;
    case Qt::Key_Right:
        moveCameraSideways(CM_Right);
        break;
    
    case Qt::Key_I:
    case Qt::Key_Equal:
    case Qt::Key_Plus:
        zoomCamera(0.9f);
        break;
    case Qt::Key_O:
    case Qt::Key_Minus:
        zoomCamera(1.1f);
        break;

    case Qt::Key_Home:
        resetView();
        break;

    default:
        // other key
        LOG_DEBUG("other key pressed:" << keyCode);
        break;
    }

    kEvent->accept();
    update();
}

void LidarScene::zoomCamera(float ratio)
{
    LOG_INFO("zoom camera by ratio: " << ratio);
    QVector3D viewVector = m_cameraRef - m_cameraEye;
    m_cameraEye = m_cameraRef - viewVector * ratio;
}

void LidarScene::moveCameraSideways(CameraMovement movement)
{
    int translationStep = 0;

    switch (movement) {
    case CM_Right:
        LOG_INFO("move camera right");
        translationStep = kTranslateStep;
        break;
    case CM_Left:
        LOG_INFO("move camera left");
        translationStep = -kTranslateStep;
        break;
    default:
        LOG_WARN("wrong agrument for moveCameraSideways(), can be CM_Left or CM_Right only");
        return;
    }
    
    QVector3D viewVector = m_cameraRef - m_cameraEye;
    m_cameraEye += QVector3D::crossProduct(viewVector, m_cameraUp).normalized() * translationStep;
    m_cameraRef += QVector3D::crossProduct(viewVector, m_cameraUp).normalized() * translationStep;
}

void LidarScene::moveCameraUpDown(CameraMovement movement)
{
    int translationStep = 0;

    switch (movement) {
    case CM_Up:
        LOG_INFO("move camera up");
        translationStep = kTranslateStep;
        break;
    case CM_Down:
        LOG_INFO("move camera down");
        translationStep = -kTranslateStep;
        break;
    default:
        LOG_WARN("wrong agrument for moveCameraUpDown(), can be CM_Up or CM_Down only");
        return;
    }

    m_cameraEye += m_cameraUp * translationStep;
    m_cameraRef += m_cameraUp * translationStep;
}

void LidarScene::drawCameraTargetPoint()
{
    glPointSize(kCameraTargetPointSize);

    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    {
        glVertex3f(m_cameraRef.x(), m_cameraRef.y(), m_cameraRef.z());
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}

void LidarScene::drawLines()
{
    // TODO
    //glLineWidth(kLineWidth);

    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINES);
    // set color for the layer
    //QColor const& color = m_pointColors[layer.id % m_pointColors.size()];
    //glColor3f(color.redF(), color.greenF(), color.blueF());
    BOOST_FOREACH(Line3D const& line, mLines) {
        {
            Point3D const& point = line.start;
            glVertex3f(point.x, point.y, point.z);
        }
        {
            Point3D const& point = line.end;
            glVertex3f(point.x, point.y, point.z);
        }
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}

void LidarScene::drawScan()
{
    glPointSize(m_pointSize);

    glEnable(GL_POINT_SMOOTH);
  
    BOOST_FOREACH(LidarLayer const& layer, m_scan.layers) {
        // set color for the layer
		  glBegin(GL_POINTS);
		  glShadeModel(GL_SMOOTH); 
			int col_id=layer.id%10;
			QColor const& color = m_pointColors[col_id];
			//LOG_INFO(col_id);
			glColor3f(color.redF(), color.greenF(), color.blueF());
			// draw points
			BOOST_FOREACH(LidarPoint const& point, layer.points) {
				
				glVertex3f(point.x, point.y, point.z);			
			}
		glEnd();
    }
    

	   /* for (int i=0;i<32;i++)
			{
				LidarLayer layer=m_scan.layers[i];
        // set color for the layer
		  glBegin(GL_POINTS);
		  glShadeModel(GL_SMOOTH); 
			int col_id=layer.id%10;
			QColor const& color = m_pointColors[col_id];
			//LOG_INFO(col_id);
			glColor3f(color.redF(), color.greenF(), color.blueF());
			// draw points
			for (int j=0;j<layer.points.size();j++) {
				LidarPoint point=layer.points[j];
				
				glVertex3f(point.x, point.y, point.z);			
			}
		glEnd();
    }


		*/
    glDisable(GL_POINT_SMOOTH);
}

void LidarScene::drawGrid(float length, float step, float lineWidth)
{
    // draw concentric circles
    glLineWidth(lineWidth);
    glColor3f(1.0, 1.0, 1.0);

    QVector3D center = QVector3D(0, 0, 0);
    QVector3D normal = QVector3D(0, 0, 1); // Z-axis (up)
    for (float radius = step; radius < length; radius += step) {
        drawCircle(radius, center, normal, /*num_segments=*/10);
    }

    // draw cross
    glBegin(GL_LINES);
    {
        // horizontal (in X axis)
        glVertex3f(-length - step, 0.0, 0.0);
        glVertex3f(length + step, 0.0, 0.0);
        // vertical (in Y axis)
        glVertex3f(0.0, -length - step, 0.0);
        glVertex3f(0.0, length + step, 0.0);
    }
    glEnd();
}

void LidarScene::drawCircle(float radius, QVector3D center, QVector3D normal, int num_segments)
{
    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_LOOP);
    {
        for (int i = 0; i < 360; i = i + 5) {
            double angle = 2 * 3.141 * i / 360;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            glVertex2d(x, y);
        }
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}

void LidarScene::drawFrame(float lineWidth, float length)
{
    glLineWidth(lineWidth);
    glBegin(GL_LINES);
    {
        // red X axis
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(length, 0.0, 0.0);
        // green Y axis
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, length, 0.0);
        // blue Z axis
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, length);
    }
    glEnd();
}

//void LidarScene::drawScale(QPainter* painter, float length)
//{
//    const int Margin = 11;
//    const int Padding = 6;
//
//    QTextDocument textDocument;
//    textDocument.setDefaultStyleSheet("* { color: #FFEFEF }");
//    textDocument.setHtml("<h4 align=\"center\">Scale: 1 [m]</h4>");
//    textDocument.setTextWidth(textDocument.size().width());
//
//    QRect rect(QPoint(0, 0),
//        textDocument.size().toSize() + QSize(2 * Padding, 2 * Padding));
//    QLine line(QPoint(0, 0), QPoint(length, 0));
//    
//    painter->translate(width() - rect.width() - Margin,
//                       height() - rect.height() - Margin);
//    painter->setPen(QColor(255, 239, 239));
//    painter->setBrush(QColor(255, 0, 0, 31));
//    
//    painter->drawRect(rect);
//    painter->drawLine(line);
//
//    painter->translate(Padding, Padding);
//    textDocument.drawContents(painter);
//}
