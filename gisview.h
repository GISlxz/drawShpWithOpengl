#ifndef GISVIEW_H
#define GISVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include "crsfile.h"
#include "gdal_priv.h"
#include <QMouseEvent>

class GISView : public QOpenGLWidget,QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit GISView(QWidget* parent = nullptr);
    ~GISView();

// QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

public:
    void initialNewCrsFile(CRSFile *newCrsFile);
    void readExteriorRingAsBuffer();
    void drawExteriorRing();
    void readTriAsBuffer();
    void drawTri();
    void prepareShaderPrograms();
    void setProjectionByExtent();
    void setEnvelop(float,float,float,float);
    void setEnvelop();
    void setEnvelop(const CRSFile* targetFile);
    void updateProjection();
    void updateColor();
    void setCurrentCrsFile(CRSFile *newCurrentCrsFile);

    void setCrsfiles(QList<CRSFile *> *newCrsfiles);

private:
    QList<CRSFile*>* crsfiles = nullptr;
    GLuint vboedge,vaoedge,vbotri,vaotri;
    int triNum = 0;
    QOpenGLShaderProgram shaderProgramEdge;
    QOpenGLShaderProgram shaderProgramTri;
    bool initialEnvelop = false;
    OGREnvelope envelope;//当前显示范围
    QPointF centerPointF;//当前显示中心点
    float aspectRatio;//当前缩放比（一个像素对应多少地理坐标）
    QVector3D EdgeColor;//边缘颜色
    QVector3D TriColor;//三角形颜色
    void checkError();
    CRSFile* currentCrsFile = nullptr;
    QPoint lastMousePos;
    bool mousePressDown = false;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // GISVIEW_H
