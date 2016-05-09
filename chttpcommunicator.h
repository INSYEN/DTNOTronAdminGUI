
#ifndef CHTTPCOMMUNICATOR_H
#define CHTTPCOMMUNICATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
class CHTTPCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit CHTTPCommunicator(QObject *parent = 0);
private:
    QNetworkAccessManager *m_netAccessMgr;
    QNetworkReply *m_netReply;
    QUrl m_url;
signals:
    void NewTableArrived(QString& tableType,QJsonArray& json);
    void NetworkError(int error);
public slots:
    void MakeRequest(const QString& requestType,const QJsonObject& parameters);
    void MakeRequest(const QString& requestType);
    void SendCommand(QJsonObject& jsonObject);
    void Connect(QUrl& url);
    void Disconnect();
private slots:
    void GotData(QNetworkReply* reply);
    void NetworkError(QNetworkReply::NetworkError error);
};

#endif // CHTTPCOMMUNICATOR_H
