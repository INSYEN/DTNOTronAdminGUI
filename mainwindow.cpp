#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Configure the HTTP stuff
    m_httpComm = new CHTTPCommunicator;


    //Configure UI connections
    connect(ui->actionRefresh,SIGNAL(triggered()),this,SLOT(DoRefresh()));
    connect(ui->refreshButton,SIGNAL(clicked()),this,SLOT(DoRefresh()));
    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(DoAdd()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(DoSave()));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(DoRemove()));


    // Configure the nodes table
    m_nodeModel = new CNodeTableModel(m_httpComm,parent);
    m_reportModel = new CReportTableModel(m_httpComm,parent);
    m_reportDelegate = new CReportDelegate;
    m_protocolDelegate = new CVariantMapDelegate;
    m_commandGeneratorDelegate = new CEditableVariantListDelegate;
    m_reportMidDelegate = new CEditableVariantListDelegate;
    m_reportParametersDelegate = new CVariantMapDelegate;
    m_reportTypeDelegate = new CEditableVariantListDelegate;

    //This function is not how model/view-delegate should work.
    m_reportDelegate->SetReportNameModelAndColumn(m_reportModel,CReportTableModel::COL_NAME);

    ui->nodeTreeView->setModel(m_nodeModel);
    //m_nodeModel->SetTableType("getNodes");
    ui->nodeTreeView->setItemDelegateForColumn(CNodeTableModel::COL_REPORTSTORUN,m_reportDelegate);
    ui->nodeTreeView->setItemDelegateForColumn(CNodeTableModel::COL_PROTOCOLS,m_protocolDelegate);
    ui->nodeTreeView->setItemDelegateForColumn(CNodeTableModel::COL_COMMANDGENERATOR,m_commandGeneratorDelegate);

    // Configure the reports table
    ui->reportTreeView->setModel(m_reportModel);
    //m_reportModel->SetTableType("getReports");
    m_reportTypeDelegate->SetEditable(false);
    m_reportTypeDelegate->SetSingleSelection(true);
    ui->reportTreeView->setItemDelegateForColumn(CReportTableModel::COL_TYPE,m_reportTypeDelegate);
    ui->reportTreeView->setItemDelegateForColumn(CReportTableModel::COL_MIDS,m_reportMidDelegate);
    ui->reportTreeView->setItemDelegateForColumn(CReportTableModel::COL_PARAMETERS,m_reportParametersDelegate);

    DoRefresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DoRefresh()
{
    QUrl url = QUrl(ui->urlEdit->text());
    m_httpComm->Connect(url);

    m_nodeModel->refresh();
    m_reportModel->refresh();
}

void MainWindow::DoSave()
{
    //Call the models save functions
    m_nodeModel->save();
    m_reportModel->save();
    //and refresh
    DoRefresh();


}
CJsonArrayTableModel* MainWindow::getCurrentModel()
{
    CJsonArrayTableModel* model;
    switch(ui->tabWidget->currentIndex())
    {
        case 0: //Nodes
            model = m_nodeModel;
        break;
        case 1: //Reports
            model = m_reportModel;
        break;
        default:
            qDebug()<<"Index: "<<ui->tabWidget->currentIndex();
            return NULL;
    }
    return model;
}

void MainWindow::DoAdd()
{
    //Insert a new row...
    qDebug()<<"Add has been clicked"<<ui->tabWidget->currentIndex();
    CJsonArrayTableModel* model = getCurrentModel();
    model->insertRow(model->rowCount());

}

void MainWindow::DoRemove()
{
    CJsonArrayTableModel* model = getCurrentModel();
    QTreeView* view;
    switch(ui->tabWidget->currentIndex())
    {
        case 0: //Nodes
            view = ui->nodeTreeView;
        break;
        case 1: //Reports
            view = ui->reportTreeView;
        break;
        default:
            qDebug()<<"Index: "<<ui->tabWidget->currentIndex();
            return;
    }
    model->removeRow(view->selectionModel()->currentIndex().row());
}
