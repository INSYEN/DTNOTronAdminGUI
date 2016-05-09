#include "cjsonarraytablemodel.h"
#include <QtDebug>
#include <QColor>
CJsonArrayTableModel::CJsonArrayTableModel(CHTTPCommunicator* httpComm, QObject *parent):QAbstractTableModel(parent)
{
    m_rowCount = 0;
    m_columnCount = 0;
    m_httpComm = httpComm;
    connect(m_httpComm,SIGNAL(NewTableArrived(QString& ,QJsonArray&)),SLOT(TableChanged(QString& ,QJsonArray&)));
}

int CJsonArrayTableModel::rowCount(const QModelIndex &/* parent */) const
{
    return m_rowCount;
}

int CJsonArrayTableModel::columnCount(const QModelIndex &/* parent*/ ) const
{
    return m_columnCount;
}

QVariant CJsonArrayTableModel::data(const QModelIndex &index, int role) const
{
    //qDebug()<<"data";
    if (role== Qt::DisplayRole)
    {
        // Check if there's a valid document and we're looking to a valid point
        if (index.row() < m_rowCount)
        {
            QJsonObject rowObject = m_jsonObj[index.row()].toObject();
            return ((QJsonValue)rowObject[m_columnMap[index.column()]]).toVariant();
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if(index.column() == 0)
        {
            //If an operation is pending, then decorate
            dirtyItemsVecT::const_iterator it = m_dirtyItems.find(index.row());
            if(it == m_dirtyItems.end())
                return QVariant();
            QColor retColor;
            switch(*it)
            {
                case OP_ADD:
                    retColor.setRgb(0,255,0);
                break;
                case OP_CHANGE:
                    retColor.setRgb(0,128,128);
                break;
                case OP_DEL:
                    retColor.setRgb(255,0,0);
                break;
                default:
                    return QVariant();

            }
        return retColor;
        }
    }
    else if (role == Qt::BackgroundColorRole)
    {
        if(index.data().isNull()) //No data for this element
        {
            return QColor(Qt::darkRed);
        }
    }

    return QVariant();
}

bool CJsonArrayTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role!=Qt::EditRole)
        return false;

    QJsonObject rowObject = m_jsonObj[index.row()].toObject();
    QString key = m_columnMap[index.column()];

    rowObject.remove(key);
    rowObject.insert(key,QJsonValue::fromVariant(value));
    m_jsonObj[index.row()]=rowObject;
    //Mark given object as dirty
    if(m_dirtyItems.find(index.row())==m_dirtyItems.end())
        m_dirtyItems[index.row()]=OP_CHANGE;

    emit dataChanged(index,index);
    return true;
}

bool CJsonArrayTableModel::removeRow(int row, const QModelIndex &parent)
{
    emit layoutAboutToBeChanged();
    m_dirtyItems[row] = OP_DEL;
    emit layoutChanged();
}

Qt::ItemFlags CJsonArrayTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if(m_dirtyItems[index.row()] == OP_DEL)
        return QAbstractTableModel::flags(index) ^ Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant CJsonArrayTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            if(section<m_columnCount)
            {
                //qDebug()<<"In header "<<section;
                //if(section == COL_VALIDREPORTS)
                //    return QVariant();
                return m_columnMap[section];
            }
        }
    }

    return QVariant();
}

void CJsonArrayTableModel::TableChanged(QString& tableType,QJsonArray& json)
{
    emit layoutAboutToBeChanged();
    //Create an internal representation

    m_jsonObj = json;
    m_columnCount = m_columnMap.size();
    m_rowCount = m_jsonObj.size();
    //emit dataChanged(createIndex(0,0),createIndex(m_rowCount,m_columnCount));
    emit layoutChanged();
}

bool CJsonArrayTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent,row,row+count);
    for(int x = row;x<row+count;x++)
    {
        m_jsonObj.insert(x,QJsonValue());
        m_dirtyItems[x]=OP_ADD;
        m_rowCount++;
    }
    endInsertRows();
    return true;
}

CJsonArrayTableModel::dirtyItemsListT CJsonArrayTableModel::getDirtyItems()
{
    dirtyItemsListT retList;
    for(dirtyItemsVecT::iterator it = m_dirtyItems.begin();it!= m_dirtyItems.end() ; ++it)
    {
        retList.push_back({*it,m_jsonObj[it.key()].toObject()});
    }
    return retList;
}

void CJsonArrayTableModel::refresh()
{
    m_dirtyItems.clear();
}

void CJsonArrayTableModel::save()
{
    for(CJsonArrayTableModel::dirtyItemsVecT::iterator it = m_dirtyItems.begin();it != m_dirtyItems.end(); ++it)
    {
        if((*it == CJsonArrayTableModel::OP_CHANGE) | (*it ==  CJsonArrayTableModel::OP_DEL))
        {
           QJsonObject object;
           object["action"]=m_delCommand;
           object[m_columnMap[0]] = m_jsonObj[it.key()].toObject()[m_columnMap[0]]; //We use the index of the first item as a key
           m_httpComm->SendCommand(object);
        }
        if((*it == CJsonArrayTableModel::OP_CHANGE) | (*it == CJsonArrayTableModel::OP_ADD))
        {
            //We create a full copy of the stored JSON object
            QJsonObject object(m_jsonObj[it.key()].toObject());
            object["action"]=m_addCommand;
            m_httpComm->SendCommand(object);
        }

    }
    //Clear dirty items buffer
    m_dirtyItems.clear();
}
