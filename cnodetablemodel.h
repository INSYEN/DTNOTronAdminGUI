#ifndef CNODETABLEMODEL_H
#define CNODETABLEMODEL_H
#include "cjsonarraytablemodel.h"

class CNodeTableModel : public CJsonArrayTableModel
{
    Q_OBJECT
public:
    //Typedefs and enums
    typedef enum
    {
        COL_NODENUM,
        COL_AMPEID,
        COL_HOSTNAME,
        COL_COMMANDGENERATOR,
        COL_PROTOCOLS,
        COL_REPORTSTORUN,
        COL_VALIDREPORTS
    } NodeColumns;

    CNodeTableModel(CHTTPCommunicator* httpComm, QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:
    void refresh();
    //void save();
    //Overridden slots
    void TableChanged(QString& tableType,QJsonArray& json);
protected:
    //Cached elements, updated on refresh
    QVariantList m_commandProducers;
    QVariantMap m_protocolDefaults;

};

#endif // CNODETABLEMODEL_H
