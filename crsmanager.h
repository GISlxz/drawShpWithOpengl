#ifndef CRSMANAGER_H
#define CRSMANAGER_H
#include "crsfile.h"

class CRSManager
{
public:
    CRSManager();

private:
    QVector<CRSFile*> crsFiles;
};

#endif // CRSMANAGER_H
