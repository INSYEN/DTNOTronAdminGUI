#include "creportdelegate.h"
#include <QTextEdit>
#include <QVariant>
#include <QDebug>
#include <QPainter>
#include <QListWidget>
#include <QStandardItem>
#include "cnodetablemodel.h"
#include <QAbstractItemView>
#include <QListWidget>
CReportDelegate::CReportDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget* CReportDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::DisplayRole);

    // Validate that the data is in the correct format
    if (data.type()==QVariant::List) //List for reports
    {
        QListWidget* listWidget = new QListWidget(parent);
        listWidget->setStyleSheet("background-color:#E0E0E0");
        return listWidget;
    }

    else
        return QStyledItemDelegate::createEditor(parent,option,index);
}

void CReportDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    QRect rect = option.rect;
    rect.setHeight(rect.height()*3);
    editor->setGeometry(rect);
}

void CReportDelegate::SetReportNameModelAndColumn(QAbstractItemModel *model, int column)
{
    m_reportNameModel=model;
    m_reportNameColumn=column;
}

void CReportDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QAbstractItemModel* model = index.model();
    QVariant data = index.data(Qt::DisplayRole);
    //Populate the editor
    if(data.type() != QVariant::List)
        QStyledItemDelegate::setEditorData(editor,index);

    QVariantList validReports = index.data(Qt::EditRole).toList();
    QVariantList currentReportsList = data.toList();

    QListWidget* listWidget = static_cast<QListWidget*>(editor);
    // Prepare the items in the list
    foreach(QVariant dataElement,validReports)
    {
        QString outString = dataElement.toString();
        if(m_reportNameModel!=(QAbstractItemModel*)NULL)
        {
            outString+=":"+searchReportColumnForId(dataElement.toInt());
        }
        QListWidgetItem* item = new QListWidgetItem(outString);
        item->setData(Qt::UserRole,dataElement);
        //Now, determine if the ítem SHOULD be checked
        if(currentReportsList.contains(dataElement))
        {
            item->setCheckState(Qt::Checked);

        }
        else
        {
            item->setCheckState(Qt::Unchecked);
        }

        listWidget->addItem(item);
    }
}

void CReportDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QListWidget* listWidget = static_cast<QListWidget*>(editor);
    QVariantList outList;
    for(int x = 0;x<listWidget->count();x++)
    {
        QListWidgetItem* item = listWidget->item(x);

        if(item->checkState()==Qt::Checked)
        {
            outList.append(QVariant(item->data(Qt::UserRole).toInt()));
        }
    }

    model->setData(index,outList);
}

QString CReportDelegate::searchReportColumnForId(int id) const
{
    if(m_reportNameModel!=(QAbstractItemModel*)NULL)
    {
        QModelIndex searchIndex =m_reportNameModel->index(0,0);

        QModelIndexList indexList = m_reportNameModel->match(searchIndex,Qt::DisplayRole,QVariant(id));
        if(indexList.size()==1)
        {
            return m_reportNameModel->data(indexList[0].child(indexList[0].row(),m_reportNameColumn)).toString();
        }
    }
    return QString();
}

void CReportDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QVariant data = index.data();
    painter->save();
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
    if(data.type()==QVariant::List)
    {
        QString outString;

        foreach(QVariant variant,data.toList())
        {
            //Don't use the report id, but use a string
            if(m_reportNameModel!=(QAbstractItemModel*)NULL)
            {
                outString+=searchReportColumnForId(variant.toInt())+", ";
            }
            else
                outString+=variant.toString()+", ";

        }
        //Remove trailing semicolon
        outString.truncate(outString.size()-2);
        painter->drawText(option.rect,outString);
    }
    else
    {
        QStyledItemDelegate::paint(painter,option,index);
    }
    painter->restore();
}
