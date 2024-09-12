#ifndef CRSFILEMODEL_H
#define CRSFILEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "crsfile.h"

class CRSFileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CRSFileModel(QObject *parent = nullptr);
    ~CRSFileModel();
    void addCRSFile(CRSFile *file);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int getCRSFilesSize();
    CRSFile* getCRSFileByIndex(int index);

    QList<CRSFile *>* getCrsFiles();

private:
    QList<CRSFile*> crsFiles;
};

#endif // CRSFILEMODEL_H
