#ifndef CNODEDELEGATE_H
#define CNODEDELEGATE_H

#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

class CReportDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CReportDelegate(QObject *parent = 0);
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void SetReportNameModelAndColumn(QAbstractItemModel* model,int column);
private:
    QSortFilterProxyModel* m_comboProxy;
    //QStandardItemModel* m_comboModel;
    QAbstractItemModel* m_reportNameModel;
    int m_reportNameColumn;
    QString searchReportColumnForId(int id) const;
signals:

public slots:
//    void refresh();

};

#endif // CNODEDELEGATE_H
