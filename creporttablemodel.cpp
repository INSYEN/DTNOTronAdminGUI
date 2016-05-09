#include "creporttablemodel.h"

CReportTableModel::CReportTableModel(CHTTPCommunicator* httpComm,QObject *parent):CJsonArrayTableModel(httpComm,parent)
{
    //Initialize column maps
    m_columnMap[COL_ID] = "id";
    m_columnMap[COL_NAME] = "name";
    m_columnMap[COL_TYPE] = "type";
    m_columnMap[COL_MIDS] = "requestedMIDs";
    m_columnMap[COL_PARAMETERS] = "reportConfig";

    m_addCommand = "addReportTemplate";
    m_delCommand = "removeReportTemplate";
}

QVariant CReportTableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::EditRole)
    {
        if(index.column() == COL_MIDS)
        {
            //Return a list of all available MIDS
            return m_midsList;
        }
        else if (index.column() == COL_TYPE)
        {
            return m_reportTypes;
        }
        else if (index.column() == COL_PARAMETERS)
        {
            return m_reportDefaults;
        }
    }
    //Default behavior: Call parent model
    return CJsonArrayTableModel::data(index,role);
}

bool CReportTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if((index.column() == COL_TYPE) & (value != index.data()))
    {
        //The report type has changed, update parameters
        setData(index.sibling(index.row(),COL_PARAMETERS),m_reportDefaults[value.toString()],Qt::EditRole);
    }
    return CJsonArrayTableModel::setData(index,value,role);

}

void CReportTableModel::TableChanged(QString& tableType,QJsonArray& json)
{
    if(tableType == "getReports") //Get the reporting table
    {
        //This can pass through to the parent
        CJsonArrayTableModel::TableChanged(tableType,json);
    }
    else if(tableType == "getReportingMids")
    {
        //This is only here because the application doesn't currently support a nested ADM view
        foreach(QJsonValue admItem,json[0].toObject())
        {
            m_midsList+=admItem.toArray().toVariantList(); //Ouch
        }
    }
    else if(tableType == "getReportDefaults")
    {
        m_reportDefaults = json[0].toObject().toVariantMap();
        m_reportTypes = m_reportDefaults.keys();
    }
}

bool CReportTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    //Call parent to initialize new row
    if(CJsonArrayTableModel::insertRows(row,count,parent) == false)
        return false;
    //Setup default values
    for(int curRow = row; curRow < row+count; curRow++)
    {
        QJsonObject curVal = m_jsonObj[row].toObject();
        //curVal.insert("protocols",QJsonObject());
        //curVal.insert("type",QString());
        curVal.insert("reportConfig",QJsonArray());
        m_jsonObj[row]=curVal;
    }
}


Qt::ItemFlags CReportTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = CJsonArrayTableModel::flags(index);
    if(index.column() == COL_ID) //Reports
    {
        return flags ^ Qt::ItemIsEditable;
    }
    return flags;
}

void CReportTableModel::refresh()
{
    //Get the reports table
    m_httpComm->MakeRequest("getReports");
    //And the complete list of MIDS
    m_httpComm->MakeRequest("getReportingMids");
    m_httpComm->MakeRequest("getReportDefaults");

    CJsonArrayTableModel::refresh();
}
