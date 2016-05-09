#include "ceditablevariantlistdelegate.h"
#include <QTextEdit>
#include <QVariant>
#include <QDebug>
#include <QPainter>
#include <QListWidget>
#include <QStandardItem>
#include <QAbstractItemView>
#include <QListWidget>
#include <QPlainTextEdit>
CEditableVariantListDelegate::CEditableVariantListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    m_singleSelectionOutput = false;
    m_editable=true;
}

QWidget* CEditableVariantListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Validate that the data is in the correct format
    qDebug()<<index.data(Qt::EditRole);
    if (index.data(Qt::EditRole).type()==QVariant::List | QVariant::StringList)
    {
        QListWidget* listWidget = new QListWidget(parent);

        return listWidget;
    }
    else
        return QStyledItemDelegate::createEditor(parent,option,index);
}

void CEditableVariantListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    rect.setHeight(rect.height()*3);
    editor->setGeometry(rect);
}
void CEditableVariantListDelegate::PopulateListView(QListWidget* listWidget,QVariantList& data,QVariantList* checkItems,bool deleteFound,Qt::CheckState defaultCheck) const
{
    foreach(QVariant dataElement,data)
    {
        QString dataElementStr = dataElement.toString();

        QListWidgetItem* item = new QListWidgetItem(dataElementStr);
        if (!m_singleSelectionOutput)
            item->setCheckState(defaultCheck);
        //item->setData(Qt::DisplayRole,dataElement);
        if (checkItems != NULL) //We have a preset value list
        {
            int itemIdx = checkItems->indexOf(dataElement);
            if(itemIdx != -1)
            {
                    if(deleteFound)
                        checkItems->removeAt(itemIdx);
                    else if (!m_singleSelectionOutput)
                        item->setCheckState(Qt::Checked);
            }

        }
        item->setFlags(item->flags()|(Qt::ItemFlag)(m_editable && Qt::ItemIsEditable));
        listWidget->addItem(item);
    }

}

void CEditableVariantListDelegate::SetEditable(bool editable)
{
    m_editable = editable;
}

void CEditableVariantListDelegate::SetSingleSelection(bool singleSel)
{
    m_singleSelectionOutput = singleSel;
}

void CEditableVariantListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QAbstractItemModel* model = index.model();
    QVariant displayData = model->data(index,Qt::DisplayRole);
    QVariant editData = model->data(index,Qt::EditRole);
    QString curSelectedStr;
    QString dataElementStr;
    QListWidget* listWidget = static_cast<QListWidget*>(editor);

    //Populate the editor
    if(displayData.type() == QVariant::Invalid)
    {
        qDebug()<<"Creating new";
        if(m_singleSelectionOutput)
            displayData = QString();
        else
            displayData = QVariantList();
    }

    if(displayData.type() == QVariant::String)
    {
        //We have a single item selection
        curSelectedStr = displayData.toString();
        listWidget->setSelectionMode(QListWidget::SingleSelection);
        m_singleSelectionOutput = true;
    }
    else if(displayData.type() != QVariant::List)
        return QStyledItemDelegate::setEditorData(editor,index);

    QVariantList displayList = displayData.toList();
    QVariantList editList = editData.toList();
    // Prepare the items in the list
    PopulateListView(listWidget,displayList,&editList,true,Qt::Checked);
    if((editList.size()) & (!m_singleSelectionOutput))
    {
        //Add an intermediate entry
        QListWidgetItem* middleItem = new QListWidgetItem("---Predefined Values---");
        middleItem->setFlags(middleItem->flags() & !(Qt::ItemFlags)Qt::ItemIsSelectable);
        listWidget->addItem(middleItem);

    }
    PopulateListView(listWidget,editList);
    //Handle selection
    if(m_singleSelectionOutput)
    {
        QList<QListWidgetItem*> items = listWidget->findItems(curSelectedStr,Qt::MatchExactly);
        if(items.size())
            listWidget->setCurrentItem(items[0]);
    }
    //Add a blank entry at the end
    QListWidgetItem* item = new QListWidgetItem();
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    listWidget->addItem(item);
}

void CEditableVariantListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QListWidget* listWidget = static_cast<QListWidget*>(editor);
    QVariantList outList;

    if(m_singleSelectionOutput == true) //We don't return a variantlist, we return a string
    {
        QVariant curItem = listWidget->currentItem()->data(Qt::DisplayRole);
        if(curItem == QVariant())
        {
            //Empty string has been selected, bounce back to the last one.
            QModelIndex curIndex = listWidget->currentIndex();
            curItem = curIndex.sibling(curIndex.row()-1,curIndex.column()).data();
        }
        model->setData(index,curItem);
    }
    else
    {
        for(int x = 0;x<listWidget->count();x++)
        {
            QListWidgetItem* item = listWidget->item(x);
            if((item->data(Qt::DisplayRole)!=QVariant()) & (item->checkState() == Qt::Checked))
            {
                outList.append(QVariant(item->data(Qt::DisplayRole)));
                qDebug()<<"Appending..."<<QVariant(item->data(Qt::DisplayRole));
            }
        }

        model->setData(index,outList);
    }
}
void CEditableVariantListDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QVariant data = index.model()->data(index);
    painter->save();


    if(data.type()==QVariant::List)
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

        foreach(QVariant variant,data.toList())
        {
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
