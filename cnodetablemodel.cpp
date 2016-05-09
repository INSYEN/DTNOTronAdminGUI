#include "cnodetablemodel.h"
#include "QColor"
CNodeTableModel::CNodeTableModel(CHTTPCommunicator* httpComm,QObject *parent):CJsonArrayTableModel(httpComm,parent)
{
    //Initialize column mapping functions
    m_columnMap[COL_NODENUM] = "nodeNum";
    m_columnMap[COL_HOSTNAME] = "hostName";
    m_columnMap[COL_AMPEID] = "ampEid";
    //m_columnMap[COL_VALIDREPORTS] = "validReports";
    m_columnMap[COL_REPORTSTORUN] = "reportsToRun";
    m_columnMap[COL_PROTOCOLS]="protocols";
    m_columnMap[COL_COMMANDGENERATOR]="commandGenerator";

    //Set up add and delete command
    m_addCommand = "addNode";
    m_delCommand = "removeNode";
}

QVariant CNodeTableModel::data(const QModelIndex &index, int role) const
{

    if (role == Qt::EditRole)
    {
        if(index.column() == COL_COMMANDGENERATOR) // Provide full list of command generators
        {
            //Create a list of all command generators
            return QVariant(m_commandProducers);
        }

        else if(index.column() == COL_PROTOCOLS) //Provide CLA defaults
        {
            return m_protocolDefaults;
        }
        else if (index.column() == COL_REPORTSTORUN) //Provide valid reports
        {
            const QJsonValue report = m_jsonObj[index.row()].toObject().value("validReports");
            if(report != QJsonValue::Undefined)
                return report.toVariant();
        }
    }

    //Default behavior: Call parent model
    return CJsonArrayTableModel::data(index,role);
}

void CNodeTableModel::refresh()
{
    //We need to make an additional request to get the full list of command generators
    m_httpComm->MakeRequest("getCommandGenerators");
    //Now, get the full nodes list
    m_httpComm->MakeRequest("getNodes");
    //We also require the list of CLA defaults
    m_httpComm->MakeRequest("getProtocolDefaults");

    CJsonArrayTableModel::refresh();

}
Qt::ItemFlags CNodeTableModel::flags(const QModelIndex &index) const
{
    if(index.column() == COL_REPORTSTORUN) //Reports
    {
        //If the row doesn't have a "validreports" partner, then we won't allow the setting of this column
        if(!m_jsonObj[index.row()].toObject().contains("validReports"))
        {
            return CJsonArrayTableModel::flags(index) & !Qt::ItemIsEditable;
        }
    }

    return CJsonArrayTableModel::flags(index);
}
void CNodeTableModel::TableChanged(QString& tableType,QJsonArray& json)
{
    //Update lists, if we care about them.
    //We still emit the layout change messages just in case something changes in the report tab which we care about.
    //We do this because the layoutchanged messages will force a UI refresh
    emit layoutAboutToBeChanged();
    if(tableType == "getNodes")
    {
        m_jsonObj = json;
        m_columnCount = m_columnMap.size();
        m_rowCount = m_jsonObj.size();
    }
    else if(tableType == "getCommandGenerators")
    {

        m_commandProducers = json.toVariantList();
    }

    else if(tableType == "getProtocolDefaults")
    {
        m_protocolDefaults = json[0].toObject().toVariantMap();
    }
    emit layoutChanged();
}

bool CNodeTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    //Call parent to initialize new row
    if(CJsonArrayTableModel::insertRows(row,count,parent) == false)
        return false;
    //Setup default values
    for(int curRow = row; curRow < row+count; curRow++)
    {
        QJsonObject curVal = m_jsonObj[row].toObject();
        //curVal.insert("protocols",QJsonObject());
        curVal.insert("commandGenerator","default");
        curVal.insert("reportsToRun",QJsonArray());
        m_jsonObj[row]=curVal;
    }
}
