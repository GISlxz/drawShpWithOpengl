#include "mainwindow.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //支持中文路径
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "YES");
    //属性表字符编码
    CPLSetConfigOption("SHAPE_ENCODING", "");
    OGRRegisterAll();
    //w.geoData = new CRSFile("E:/宁夏/矢量文件/示范区.shp");
    w.geoData = new CRSFile("E:/课题组/垦利/后修正/gendi0815.shp");
    w.geoData->readExteriorRing();
    w.geoData->splitPoly2Tri();
    w.gisViewWindow->setCurrentCrsFile(w.geoData);
    w.gisViewWindow->readExteriorRingAsBuffer();
    w.gisViewWindow->readTriAsBuffer();
    w.gisViewWindow->setEnvelop();
    w.gisViewWindow->updateColor();
    w.gisViewWindow->updateProjection();
    w.gisViewWindow->update();
    return a.exec();
}
