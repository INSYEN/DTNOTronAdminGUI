#ifndef CEDITABLEVARIANTLISTDELEGATE_H
#define CEDITABLEVARIANTLISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QListWidget>

class CEditableVariantListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CEditableVariantListDelegate(QObject *parent = 0);
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void SetEditable(bool editable);
    void SetSingleSelection(bool singleSel);
protected:
    mutable bool m_singleSelectionOutput;
    bool m_editable;

    void PopulateListView(QListWidget* listWidget,QVariantList& data,QVariantList* checkItems = NULL,bool deleteFound = false, Qt::CheckState defaultCheck = Qt::Unchecked) const;
signals:

public slots:

};

#endif // CEDITABLEVARIANTLISTDELEGATE_H
