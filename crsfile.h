#ifndef CRSFILE_H
#define CRSFILE_H
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "poly2tri.h"
#include "qvectornd.h"
#include <QString>
#include <QDebug>
#include <QVector>
#include <QPointF>
#include <QTime>

class CRSFile
{
public:
    CRSFile(const char* _fileDir);
//    CRSFile(const CRSFile&) = delete;
//    CRSFile& operator=(const CRSFile&) = delete;
//    CRSFile(CRSFile&&) noexcept = default;
//    CRSFile& operator=(CRSFile&&) noexcept = default;
    ~CRSFile();
public:
    static QVector3D randomColor(){
        // Seed the random number generator with the current time (milliseconds)
        srand(static_cast<uint>(QTime::currentTime().msec()));

        // Generate random values for R, G, B components between 0 and 1
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;

        // Return the QVector3D representing the random color
        return QVector3D(r, g, b);
    }

public:
    QString extractFileName(const char* filePath);//获取文件名
    const GDALDatasetUniquePtr& getDataUniquePtr() const;//获取GDALDatasetUniquePtr
    void readExteriorRing();
    void splitPoly2Tri();
    const QVector<float>* getEXteriorRing() const;
    QPointF getCenterPointF() const;
    const OGREnvelope& getEnvelope() const;
    const QVector<int>* getEXteriorRingIndex() const;
    const QVector<float> *getSplitTri() const;
    const QVector<unsigned int> getVBOandVAO();
    void setVBOandVAO(const QVector<unsigned int> VBOandVAO);

public:
    QString name;//变量名（图层名）

    QVector3D getEdgeColor() const;
    QVector3D getTriColor() const;

    void setTriNum(unsigned int newTriNum);

    unsigned int getTriNum() const;

private:
    unsigned int vboedge,vaoedge,vbotri,vaotri,triNum;
    QVector3D EdgeColor;//边缘颜色
    QVector3D TriColor;//三角形颜色
    QString fileDir;//文件位置
    GDALDatasetUniquePtr m_GDALDatasetUniquePtr;
    bool needDraw;
    QVector<float>* EXteriorRing = nullptr;//外环
    QVector<int>* EXteriorRingIndex = nullptr;//外环索引（每个geometry起点）
    QVector<float>* splitTri = nullptr;
    QPointF centerPointF;//中心点
    OGREnvelope envelope;//外包矩形
};

#endif // CRSFILE_H
