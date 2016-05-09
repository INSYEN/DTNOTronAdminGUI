#include "cvariantmapdelegate.h"
#include <QTextEdit>
#include <QVariant>
#include <QDebug>
#include <QPainter>
#include <QStandardItem>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <cmath>

CVariantMapDelegate::CVariantMapDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget* CVariantMapDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.model()->data(index);
    qDebug()<<data;
    if ((data.type()==QVariant::Map) | (data.type()==QVariant::Invalid)) //Core JSON object is a map
    {
        QTreeWidget* treeWidget = new QTreeWidget(parent);
        treeWidget->setColumnCount(2); //Key, value
        QStringList headerStrList;
        headerStrList<<"Key"<<"Value";
        treeWidget->setHeaderLabels(headerStrList);
        treeWidget->setStyleSheet("background-color:#E0E0E0");
        return treeWidget;
    }
    else
        return QStyledItemDelegate::createEditor(parent,option,index);
}

void CVariantMapDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    rect.setHeight(rect.height()*8);
    rect.setWidth(rect.width()*3);
    editor->setGeometry(rect);
}

void CVariantMapDelegate::traverseVariantMaps(QTreeWidget* treeWidget,QVariantMap& map, QTreeWidgetItem* parent, const QVariantMap& defaultMap) const
{
    QTreeWidgetItem* lastInsertedItem = NULL;
    QVariantMap parseMap;
    qDebug()<<"Traversing";
    //Special case: the main map is empty
    if(map.isEmpty())
    {
        qDebug()<<"Using default map";
        //In that case, we'll transform the default map to the main
        parseMap = defaultMap;
    }
    else
    {
        parseMap = map;
    }
    for(QVariantMap::iterator dataIt=parseMap.begin();dataIt!=parseMap.end();++dataIt)
    {
        QStringList tempStrings;
        tempStrings<<dataIt.key();
        QTreeWidgetItem* treeItem = new QTreeWidgetItem(tempStrings);
        treeItem->setFlags(treeItem->flags()|Qt::ItemIsEditable);

        if(parent==NULL) //Top-level element
        {
            if(map.find(dataIt.key()) != map.end())
                treeItem->setData(0,Qt::CheckStateRole,Qt::Checked);
            else
                treeItem->setData(0,Qt::CheckStateRole,Qt::Unchecked);
            //treeItem->setFlags(treeItem->flags() | Qt::ItemIsUserCheckable | (Qt::ItemFlags)~Qt::ItemIsTristate);

            treeWidget->insertTopLevelItem(0,treeItem);
            lastInsertedItem=treeItem;
        }

        if(map.find(dataIt.key()) == map.end())
        {
            treeItem->setTextColor(0,QColor(Qt::lightGray));
        }

        if(dataIt.value().type()==QVariant::Map)
        {
            QVariantMap tempMap = dataIt.value().toMap();
            traverseVariantMaps(treeWidget,tempMap,lastInsertedItem);
        }
        else
        {
            qDebug()<<"Data iterator:"<<dataIt.value();
            if(dataIt.value().type()==QVariant::Double)
            {
                //typecast to int if theres no decimal, in order to bypass wacky QJsonValue behavior where the decimal will always be displayed
                double tempVal = dataIt.value().toDouble();

                if(modf(tempVal,&tempVal) == 0)
                {
                    dataIt.value()=QVariant(dataIt.value().toInt());
                }
            }
            treeItem->setData(1,Qt::DisplayRole,dataIt.value());

            //treeItem->setText(1,dataIt.value().toString());
            if(parent != NULL)
            {
                qDebug()<<"Adding child: "<<treeItem->data(0,Qt::DisplayRole).toString()<<" for "<<parent->data(0,Qt::DisplayRole).toString();
                parent->addChild(treeItem);
            }
            else if(lastInsertedItem != NULL)
            {
                lastInsertedItem->addChild(treeItem);
            }

        }
        lastInsertedItem=treeItem;
    }
}

void CVariantMapDelegate::createVariantMap(QTreeWidgetItem* treeItem,QVariantMap& outMap) const
{
    qDebug()<<"CreateVariantMap";
    for(int x = 0;x<treeItem->childCount();x++)
    {
        QTreeWidgetItem* curItem = treeItem->child(x);
        QString curItemStr = curItem->data(0,Qt::DisplayRole).toString();

        if(!(treeItem->flags() & Qt::ItemIsUserCheckable) & (curItem->checkState(0) == Qt::Unchecked))
        {
            qDebug()<<"Unchecked or disabled: "<<curItemStr<<" "<<(treeItem->flags() && Qt::ItemIsUserCheckable);
            continue;
        }
        //QVariant itemData = curItem->data(1,Qt::DisplayRole);
        if(curItem->childCount() > 0)
        {
            qDebug()<<"Child has children, recursing: "<<curItemStr;
            QVariantMap tempMap;

            createVariantMap(curItem,tempMap);
            qDebug()<<tempMap;
            outMap[curItemStr]=tempMap;

        }
        else
        {
            qDebug()<<"Child has no children "<<curItemStr;
           outMap[curItemStr]=curItem->data(1,Qt::DisplayRole);
        }
    }
}

void CVariantMapDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QAbstractItemModel* model = index.model();
    QVariant data = index.data();

    if (data.type() == QVariant::Invalid) //There's nothing here, so we can create it
    {
        data=QVariantMap();
    }
    else if(data.type()!=QVariant::Map)
        return QStyledItemDelegate::setEditorData(editor,index);

    QVariantMap baseData = index.data(Qt::EditRole).toMap();
    //Populate the editor
    QTreeWidget* treeWidget = static_cast<QTreeWidget*>(editor);
    //Populate the tree
    QVariantMap treeMap = data.toMap();
    traverseVariantMaps(treeWidget,treeMap,(QTreeWidgetItem*)NULL,baseData);

}

void CVariantMapDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariantMap outMap;
    QTreeWidget* treeWidget = static_cast<QTreeWidget*>(editor);
    QTreeWidgetItem* treeItem = treeWidget->invisibleRootItem();
    createVariantMap(treeItem,outMap);
    model->setData(index,outMap);
}


void CVariantMapDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QVariant data = index.model()->data(index);
    painter->save();

    if(data.type()==QVariant::Map)
    {
        QBrush rectBrush=option.palette.base();
        QPen textPen = option.palette.text().color();

        if (option.state & QStyle::State_Selected)
        {
            rectBrush = option.palette.highlight();
            textPen = option.palette.highlightedText().color();
            if (!(option.state & QStyle::State_Enabled))
            {
                rectBrush = option.palette.brush(QPalette::Inactive,QPalette::Highlight);
            }
        }
        painter->fillRect(option.rect, rectBrush);
        painter->setPen(textPen);

        QString outString;

        foreach(QString variant,data.toMap().keys())
        {

            outString+=variant+", ";
        }
        outString.truncate(outString.size()-2);
        painter->drawText(option.rect,outString);
    }
    else
    {
        QStyledItemDelegate::paint(painter,option,index);
    }
    painter->restore();
}
