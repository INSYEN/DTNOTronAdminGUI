#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chttpcommunicator.h"
#include "cnodetablemodel.h"
#include "creporttablemodel.h"
#include "creportdelegate.h"
#include "cvariantmapdelegate.h"
#include "ceditablevariantlistdelegate.h"
#include <QSortFilterProxyModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CHTTPCommunicator* m_httpComm;
    CNodeTableModel *m_nodeModel;
    CReportTableModel *m_reportModel;

    CReportDelegate* m_reportDelegate;
    CVariantMapDelegate* m_protocolDelegate;
    CEditableVariantListDelegate* m_reportMidDelegate;
    CVariantMapDelegate* m_reportParametersDelegate;
    CEditableVariantListDelegate* m_commandGeneratorDelegate;
    CEditableVariantListDelegate* m_reportTypeDelegate;


    enum tableTypeT {TABLE_NODES,TABLE_REPORTS};
    CJsonArrayTableModel* getCurrentModel();

public slots:
    void DoRefresh();
    void DoSave();
    void DoAdd();
    void DoRemove();
};

#endif // MAINWINDOW_H
