#include "crsfilemodel.h"

CRSFileModel::CRSFileModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

CRSFileModel::~CRSFileModel()
{

}

void CRSFileModel::addCRSFile(CRSFile* file){
    beginInsertRows(QModelIndex(), crsFiles.size(), crsFiles.size());
    crsFiles.append(file);
    endInsertRows();
}

int CRSFileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return crsFiles.count();
}

QVariant CRSFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= crsFiles.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        return crsFiles.at(index.row())->name;
    }

    return QVariant();
}

int CRSFileModel::getCRSFilesSize()
{
    return crsFiles.size();
}

CRSFile *CRSFileModel::getCRSFileByIndex(int index)
{
    return crsFiles[index];
}

QList<CRSFile *>* CRSFileModel::getCrsFiles()
{
    return &crsFiles;
}

