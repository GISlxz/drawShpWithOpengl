#include "gisview.h"

GISView::GISView(QWidget* parent):QOpenGLWidget(parent)
{
    //setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

GISView::~GISView()
{
    qDebug()<<"GISView release";
}

void GISView::initializeGL()
{
    qDebug()<<"initializeGL";
    this->initializeOpenGLFunctions();
    this->prepareShaderPrograms();
}

void GISView::resizeGL(int w, int h)
{

}

void GISView::paintGL()
{
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(this->crsfiles != nullptr){
        int crsNum = crsfiles->size();
        for(int index = crsNum-1;index>=0;index--){
            setCurrentCrsFile((*crsfiles)[index]);
            updateProjection();
            updateColor();
            drawTri();
            drawExteriorRing();
        }
    }
    glBindVertexArray(0);
}

void GISView::initialNewCrsFile(CRSFile *newCrsFile)
{
    setCurrentCrsFile(newCrsFile);
    readExteriorRingAsBuffer();
    readTriAsBuffer();
    newCrsFile->setVBOandVAO(QVector<unsigned int>{vboedge,vaoedge,vbotri,vaotri});
    newCrsFile->setTriNum(triNum);
    if(initialEnvelop == false){
        setEnvelop(newCrsFile);
        initialEnvelop = true;
    }
}

void GISView::readExteriorRingAsBuffer()
{
    //绑定vbo并灌入数据
    glGenBuffers(1,&vboedge);
    //checkError();
    glBindBuffer(GL_ARRAY_BUFFER,vboedge);
    //checkError();
    auto exteriorRing = currentCrsFile->getEXteriorRing();
    glBufferData(GL_ARRAY_BUFFER,exteriorRing->size()*sizeof(float),exteriorRing->data(),GL_STATIC_DRAW);

    //绑定vao
    glGenVertexArrays(1,&vaoedge);
    glBindVertexArray(vaoedge);
    glBindBuffer(GL_ARRAY_BUFFER, vboedge);

    //描述位置数据
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);

    //解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GISView::drawExteriorRing()
{
    glBindVertexArray(vaoedge);
    shaderProgramEdge.bind();
    auto ExtriorRingIndex = currentCrsFile->getEXteriorRingIndex();
    for(int i=0;i<ExtriorRingIndex->size()-1;i++){
        glDrawArrays(GL_LINE_LOOP, (*ExtriorRingIndex)[i]/2, ((*ExtriorRingIndex)[i+1]-(*ExtriorRingIndex)[i]-1)/2);
    }
}

void GISView::readTriAsBuffer()
{
    //绑定vbo并灌入数据
    glGenBuffers(1,&vbotri);
    //checkError();
    glBindBuffer(GL_ARRAY_BUFFER,vbotri);
    //checkError();
    auto Triangles = currentCrsFile->getSplitTri();
    triNum = Triangles->size();
    glBufferData(GL_ARRAY_BUFFER,Triangles->size()*sizeof(float),Triangles->data(),GL_STATIC_DRAW);

    //绑定vao
    glGenVertexArrays(1,&vaotri);
    glBindVertexArray(vaotri);
    glBindBuffer(GL_ARRAY_BUFFER, vbotri);

    //描述位置数据
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);

    //解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GISView::drawTri()
{
    glBindVertexArray(vaotri);
    shaderProgramTri.bind();
    glDrawArrays(GL_TRIANGLES, 0, triNum);
    glBindVertexArray(0);
}

void GISView::prepareShaderPrograms()
{
    qDebug()<<"start initial program";
    shaderProgramEdge.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/edge.vert");
    shaderProgramEdge.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/edge.frag");
    bool success = shaderProgramEdge.link();
    if(!success){
        shaderProgramEdge.log();
    }
    shaderProgramEdge.bind();

    shaderProgramTri.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/tri.vert");
    shaderProgramTri.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/tri.frag");
    success = shaderProgramTri.link();
    if(!success){
        shaderProgramTri.log();
    }
    shaderProgramTri.bind();
}

void GISView::setProjectionByExtent()
{

}

void GISView::setEnvelop(float, float, float, float)
{

}

void GISView::setEnvelop(const CRSFile *targetFile)
{
    envelope = targetFile->getEnvelope();
    centerPointF = targetFile->getCenterPointF();
    float minX = envelope.MinX; // 经度最小值
    float maxX = envelope.MaxX; // 经度最大值
    float minY = envelope.MinY;  // 纬度最小值
    float maxY = envelope.MaxY;  // 纬度最大值
    float aspectRatioX = (maxX-minX)/float(this->width());//x轴缩放比
    float aspectRatioY = (maxY-minY)/float(this->height());//Y轴缩放比
    aspectRatio = fmax(aspectRatioX,aspectRatioY);
    //如果X轴缩放比大
    if(aspectRatioX>aspectRatioY){
        envelope.MaxY = centerPointF.y() + (envelope.MaxY-centerPointF.y()) * (aspectRatioX/aspectRatioY);
        envelope.MinY = centerPointF.y() - (centerPointF.y()-envelope.MinY) * (aspectRatioX/aspectRatioY);
    }
    //如果Y轴缩放比大
    else if(aspectRatioX<aspectRatioY){
        envelope.MaxX = centerPointF.x() + (envelope.MaxX-centerPointF.x()) * (aspectRatioY/aspectRatioX);
        envelope.MinX = centerPointF.x() - (centerPointF.x()-envelope.MinX) * (aspectRatioY/aspectRatioX);
    }
}

void GISView::setEnvelop()
{
    envelope = currentCrsFile->getEnvelope();
    centerPointF = currentCrsFile->getCenterPointF();
    float minX = envelope.MinX; // 经度最小值
    float maxX = envelope.MaxX; // 经度最大值
    float minY = envelope.MinY;  // 纬度最小值
    float maxY = envelope.MaxY;  // 纬度最大值
    float aspectRatioX = (maxX-minX)/float(this->width());//x轴缩放比
    float aspectRatioY = (maxY-minY)/float(this->height());//Y轴缩放比
    aspectRatio = fmax(aspectRatioX,aspectRatioY);
    //如果X轴缩放比大
    if(aspectRatioX>aspectRatioY){
        envelope.MaxY = centerPointF.y() + (envelope.MaxY-centerPointF.y()) * (aspectRatioX/aspectRatioY);
        envelope.MinY = centerPointF.y() - (centerPointF.y()-envelope.MinY) * (aspectRatioX/aspectRatioY);
    }
    //如果Y轴缩放比大
    else if(aspectRatioX<aspectRatioY){
        envelope.MaxX = centerPointF.x() + (envelope.MaxX-centerPointF.x()) * (aspectRatioY/aspectRatioX);
        envelope.MinX = centerPointF.x() - (centerPointF.x()-envelope.MinX) * (aspectRatioY/aspectRatioX);
    }
}

void GISView::updateProjection()
{
    QMatrix4x4 projection;
    float minX = envelope.MinX; // 经度最小值
    float maxX = envelope.MaxX; // 经度最大值
    float minY = envelope.MinY;  // 纬度最小值
    float maxY = envelope.MaxY;  // 纬度最大值
    float minZ = -10.0f;   // 高度最小值
    float maxZ = 10.0f;  // 高度最大值
    projection.ortho(minX, maxX, minY, maxY, minZ, maxZ);
    shaderProgramEdge.bind();
    shaderProgramEdge.setUniformValue("projection",projection);
    shaderProgramTri.bind();
    shaderProgramTri.setUniformValue("projection",projection);
}

void GISView::updateColor()
{
    shaderProgramEdge.bind();
    shaderProgramEdge.setUniformValue("aColor",EdgeColor);
    shaderProgramTri.bind();
    shaderProgramTri.setUniformValue("aColor",TriColor);
}

void GISView::checkError()
{
    GLenum errorCode = glGetError();
    std::string error = "";
    if (errorCode != GL_NO_ERROR)
    {
        switch (errorCode)
        {
        case GL_INVALID_ENUM:error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:error = "OUT OF MEMORY"; break;
        default:
            error = "UNKNOWN";
            break;
        }
        qDebug() << error;
        // assert会根据传入的bool值，来决定程序是否停止
        //true：程序运行
        //false：程序会断死
        assert(false);
    }
}

void GISView::mousePressEvent(QMouseEvent *event)
{
    mousePressDown = true;
    lastMousePos = event->pos();
}

void GISView::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressDown = false;
}

void GISView::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressDown){
        auto currentPos = event->pos();
        auto deltaPos = currentPos-lastMousePos;
        lastMousePos = currentPos;
        envelope.MaxX -= deltaPos.x()*aspectRatio;
        envelope.MinX -= deltaPos.x()*aspectRatio;
        envelope.MaxY += deltaPos.y()*aspectRatio;
        envelope.MinY += deltaPos.y()*aspectRatio;
        centerPointF = {(envelope.MaxX+envelope.MinX)/2,(envelope.MaxY+envelope.MinY)/2};
        updateProjection();
        update();
    }
}

void GISView::wheelEvent(QWheelEvent *event)
{
    float currentAspectRatio = aspectRatio;
    aspectRatio *= (1-(float(event->angleDelta().y())/1200.0f));
    float scale = aspectRatio/currentAspectRatio;
    envelope.MaxY = centerPointF.y() + (envelope.MaxY-centerPointF.y()) * scale;
    envelope.MinY = centerPointF.y() - (centerPointF.y()-envelope.MinY) * scale;
    envelope.MaxX = centerPointF.x() + (envelope.MaxX-centerPointF.x()) * scale;
    envelope.MinX = centerPointF.x() - (centerPointF.x()-envelope.MinX) * scale;
    updateProjection();
    update();
}

void GISView::setCurrentCrsFile(CRSFile *newCurrentCrsFile)
{
    currentCrsFile = newCurrentCrsFile;
    auto VBOandVAO = currentCrsFile->getVBOandVAO();
    vboedge = VBOandVAO[0];
    vaoedge = VBOandVAO[1];
    vbotri = VBOandVAO[2];
    vaotri = VBOandVAO[3];
    triNum = currentCrsFile->getTriNum();
    EdgeColor=currentCrsFile->getEdgeColor();
    TriColor=currentCrsFile->getTriColor();
}

void GISView::setCrsfiles(QVector<CRSFile *> *newCrsfiles)
{
    crsfiles = newCrsfiles;
}


