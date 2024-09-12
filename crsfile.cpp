#include "crsfile.h"

CRSFile::CRSFile(const char *_fileDir):fileDir(_fileDir)
{
    m_GDALDatasetUniquePtr = GDALDatasetUniquePtr((GDALDataset *) GDALOpenEx(_fileDir, GDAL_OF_VECTOR, NULL, NULL, NULL));
    if (m_GDALDatasetUniquePtr == nullptr) {
        qDebug() << "打开文件失败" << fileDir;
            return;
    }
    else{
        qDebug() << "加载成功 : " << fileDir;
    }
    needDraw = true;
    name = extractFileName(_fileDir);

    OGRLayer *poLayer = m_GDALDatasetUniquePtr->GetLayer(0);

    if (poLayer == nullptr) {
        qDebug() << "Failed to get layer.";
        return;
    }

    if (poLayer->GetExtent(&envelope, TRUE) != OGRERR_NONE) {
        qDebug() << "Failed to get layer extent.";
        return;
    }

    double minX = envelope.MinX;
    double maxX = envelope.MaxX;
    double minY = envelope.MinY;
    double maxY = envelope.MaxY;

    // 计算中心点
    centerPointF = {(minX + maxX) / 2.0,(minY + maxY) / 2.0};
    EdgeColor = randomColor();
    readExteriorRing();
    splitPoly2Tri();
    TriColor = randomColor();
}

CRSFile::~CRSFile()
{
    qDebug()<<"release CRSFile";
}

// 提取文件名的函数
QString CRSFile::extractFileName(const char* filePath) {
    QString path(filePath);
    // 查找最后一个路径分隔符的位置
    int pos = path.lastIndexOf(QChar('/'));
    if (pos == -1) {
        pos = path.lastIndexOf(QChar('\\'));
    }
    // 提取路径分隔符之后的子字符串
    QString fileName = path.mid(pos + 1);
    return fileName;
}

void CRSFile::readExteriorRing()
{
    OGRLayer *poLayer = m_GDALDatasetUniquePtr->GetLayer(0);
    if (poLayer == nullptr) {
        qDebug() << "无效图层";
        return;
    }
    EXteriorRing = new QVector<float>;
    EXteriorRingIndex = new QVector<int>;
    int count = 0;
    EXteriorRingIndex->push_back(count);
    for(auto& poFeature:poLayer){
        auto poGeometry = poFeature->GetGeometryRef();
        if(poGeometry == nullptr){
            delete EXteriorRing;
            EXteriorRing = nullptr;
            delete EXteriorRingIndex;
            EXteriorRingIndex = nullptr;
            return;
        }
        if(wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon){
            OGRPolygon *poPolygon = (OGRPolygon *) poGeometry;
            OGRLinearRing *poExteriorRing = poPolygon->getExteriorRing();
            for(auto& point:poExteriorRing){
                count+=2;
                EXteriorRing->push_back(point.getX());
                EXteriorRing->push_back(point.getY());
            }
            EXteriorRingIndex->push_back(count);
        }
        else if(wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon){
            OGRMultiPolygon* poMultiPolygon = (OGRMultiPolygon*)poGeometry;
            for(auto* poPolygon:poMultiPolygon){
                OGRLinearRing *poExteriorRing = poPolygon->getExteriorRing();
                for(auto& point:poExteriorRing){
                    count+=2;
                    EXteriorRing->push_back(point.getX());
                    EXteriorRing->push_back(point.getY());
                }
                EXteriorRingIndex->push_back(count);
            }
        }
        else{
            delete EXteriorRing;
            EXteriorRing = nullptr;
            delete EXteriorRingIndex;
            EXteriorRingIndex = nullptr;
            qDebug()<<"unknown geoType";
            qDebug()<<wkbFlatten(poGeometry->getGeometryType());
            return;
        }
    }
}

void CRSFile::splitPoly2Tri()
{
    splitTri = new QVector<float>;
    for(int i=0;i<EXteriorRingIndex->size()-1;i++){
        std::vector<p2t::Point*> polyline;
        for(int j = (*EXteriorRingIndex)[i];j<(*EXteriorRingIndex)[i+1]-2;j+=2){
            auto _newpoint = new p2t::Point({(*EXteriorRing)[j],(*EXteriorRing)[j+1]});
            p2t::Point* _oldpoint = nullptr;
            if(!polyline.empty()) _oldpoint = polyline[polyline.size()-1];
            if(_oldpoint!=nullptr){
                if(_oldpoint->x == _newpoint->x&&_oldpoint->y==_newpoint->y){
                    delete _newpoint;
                    continue;
                }
            }
            polyline.push_back(_newpoint);
        }
        p2t::CDT* cdt;
        try{
            cdt = new p2t::CDT{ polyline };
        }
        catch(const std::exception& e) {
            // 捕获并处理异常，然后跳过当前循环迭代
            //qDebug() << "Error: " << e.what();
            for (const auto p : polyline) {
                delete p;
            }
            continue;  // 跳到下一个循环迭代
        }
        try{
            cdt->Triangulate();
        }
        catch(const std::exception& e) {
            // 捕获并处理异常，然后跳过当前循环迭代
            //qDebug() << "Error: " << e.what();
            for (const auto p : polyline) {
                delete p;
            }
            delete cdt;
            continue;
        }

        const auto result = cdt->GetTriangles();
        for(auto tri:result){
            splitTri->push_back(tri->GetPoint(0)->x);
            splitTri->push_back(tri->GetPoint(0)->y);
            splitTri->push_back(tri->GetPoint(1)->x);
            splitTri->push_back(tri->GetPoint(1)->y);
            splitTri->push_back(tri->GetPoint(2)->x);
            splitTri->push_back(tri->GetPoint(2)->y);
        }
        for (const auto p : polyline) {
            delete p;
        }
        delete cdt;
    }
}

const QVector<float> *CRSFile::getEXteriorRing() const
{
    return EXteriorRing;
}

QPointF CRSFile::getCenterPointF() const
{
    return centerPointF;
}

const OGREnvelope& CRSFile::getEnvelope() const
{
    return envelope;
}

const QVector<int> *CRSFile::getEXteriorRingIndex() const
{
    return EXteriorRingIndex;
}

const QVector<float> *CRSFile::getSplitTri() const
{
    return splitTri;
}

const QVector<unsigned int> CRSFile::getVBOandVAO()
{
    return QVector<unsigned int>{vboedge,vaoedge,vbotri,vaotri};
}

void CRSFile::setVBOandVAO(const QVector<unsigned int> VBOandVAO)
{
    vboedge = VBOandVAO[0];
    vaoedge = VBOandVAO[1];
    vbotri = VBOandVAO[2];
    vaotri = VBOandVAO[3];
}

QVector3D CRSFile::getEdgeColor() const
{
    return EdgeColor;
}

QVector3D CRSFile::getTriColor() const
{
    return TriColor;
}

void CRSFile::setTriNum(unsigned int newTriNum)
{
    triNum = newTriNum;
}

unsigned int CRSFile::getTriNum() const
{
    return triNum;
}

const GDALDatasetUniquePtr& CRSFile::getDataUniquePtr() const
{
    return m_GDALDatasetUniquePtr;
}
