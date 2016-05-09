#ifndef CJsonArrayTableModel_H
#define CJsonArrayTableModel_H

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include "chttpcommunicator.h"

class CJsonArrayTableModel : public QAbstractTableModel
{
   Q_OBJECT
public:
    //Typedefs and enums
    typedef QMap<int,QString> columnIndexMapT;
    typedef enum
    {
        OP_ADD,
        OP_CHANGE,
        OP_DEL
    } itemOperationT;

    typedef struct
    {
        itemOperationT operation;
        QJsonObject object;
    } dirtyItemT;

    typedef QList<dirtyItemT> dirtyItemsListT;

    CJsonArrayTableModel(CHTTPCommunicator* httpComm, QObject *parent = 0);
    //Model-related overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRow(int row, const QModelIndex &index = QModelIndex());
    //Connectivity-related stuff
    void SetConnUrl(QString url);

    //Save functionality
    dirtyItemsListT getDirtyItems();
    //TODO: Implement this if required
    //void SetColumnIndexMappingMap(QString& parentStr,columnIndexMapT& map);
protected:
    typedef QMap<int,itemOperationT> dirtyItemsVecT;
    columnIndexMapT m_columnMap;
    dirtyItemsVecT m_dirtyItems;
    CHTTPCommunicator* m_httpComm;

    QJsonArray m_jsonObj;
    int m_columnCount;
    int m_rowCount;
    QString m_addCommand;
    QString m_delCommand;

public slots:
    virtual void TableChanged(QString& tableType,QJsonArray& json);
    void refresh();
    void save();
    //void SaveData(QString& tableType,QJsonArray& json);
};

#endif // CJsonArrayTableModel_H
