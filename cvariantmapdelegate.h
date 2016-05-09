#ifndef CPROTOCOLDELEGATE_H
#define CPROTOCOLDELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
class CVariantMapDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CVariantMapDelegate(QObject *parent = 0);
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    void traverseVariantMaps(QTreeWidget* treeWidget,QVariantMap& map, QTreeWidgetItem* parent, const QVariantMap& defaultMap = QVariantMap()) const;
    void createVariantMap(QTreeWidgetItem* treeItem,QVariantMap& outMap) const;
signals:


public slots:

};

#endif // CPROTOCOLDELEGATE_H
