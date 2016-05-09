#ifndef CREPORTTABLEMODEL_H
#define CREPORTTABLEMODEL_H
#include "cjsonarraytablemodel.h"

class CReportTableModel : public CJsonArrayTableModel
{
    Q_OBJECT
public:
    typedef enum
    {
        COL_ID,
        COL_NAME,
        COL_TYPE,
        COL_MIDS,
        COL_PARAMETERS
    } ReportColumns;

    CReportTableModel(CHTTPCommunicator* httpComm, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent);

    //bool hasChildren(const QModelIndex &parent) const;
public slots:
    void TableChanged(QString& tableType,QJsonArray& json);
    void refresh();
protected:
    QVariantList m_midsList;
    QVariantMap m_reportDefaults;
    QStringList m_reportTypes;
};

#endif // CREPORTTABLEMODEL_H
