#ifndef CRSFILE_H
#define CRSFILE_H
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "poly2tri.h"
#include <QString>
#include <QDebug>
#include <QVector>
#include <QPointF>

class CRSFile
{
public:
    CRSFile(const char* _fileDir);
    ~CRSFile();

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

private:
    QString fileDir;//文件位置
    QString name;//变量名（图层名）
    GDALDatasetUniquePtr m_GDALDatasetUniquePtr;
    bool needDraw;
    QVector<float>* EXteriorRing = nullptr;//外环
    QVector<int>* EXteriorRingIndex = nullptr;//外环索引（每个geometry起点）
    QVector<float>* splitTri;
    QPointF centerPointF;//中心点
    OGREnvelope envelope;//外包矩形
};

#endif // CRSFILE_H
