#include "chttpcommunicator.h"
#include <QByteArray>
#include <QJsonObject>

CHTTPCommunicator::CHTTPCommunicator(QObject *parent) :
    QObject(parent)

{
    m_netAccessMgr = new QNetworkAccessManager;
    //Start the HTTP connection
    connect(m_netAccessMgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(GotData(QNetworkReply*)));
}

/*      Public Slots       */
void CHTTPCommunicator::SendCommand(QJsonObject& jsonObject)
{
    QJsonDocument jsonDoc;

    //Craft the HTTP request
    jsonDoc.setObject(jsonObject);
    QByteArray data = jsonDoc.toJson();
    QNetworkRequest request(m_url);
    request.setRawHeader("User-Agent","DTNOTron-Admin V0.0.1A");
    request.setRawHeader("X-Custom-User-Agent","DTNOTron-Admin V0.0.1A");
    request.setRawHeader("Content-Type","application/json");
    request.setRawHeader("Content-Length",QByteArray::number(data.size()));
    //Now, actually send the HTTP request
    m_netReply = m_netAccessMgr->post(request,data);
    connect(m_netReply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(NetworkError(QNetworkReply::NetworkError)));
}

void CHTTPCommunicator::MakeRequest(const QString& requestType,const QJsonObject& parameters)
{
    //Craft the JSON string
    QJsonObject jsonObject;
    jsonObject["action"]=requestType;
    if(parameters != QJsonObject())
    {
        for(QJsonObject::const_iterator it = parameters.begin();it!=parameters.end();++it)
        {
            jsonObject[it.key()]=it.value();
        }
    }

    SendCommand(jsonObject);
}

void CHTTPCommunicator::MakeRequest(const QString& requestType)
{
    MakeRequest(requestType,QJsonObject());
}

void CHTTPCommunicator::Connect(QUrl& url)
{
    m_url = url;
}

void CHTTPCommunicator::Disconnect()
{
}

/*      Private Slots       */
void CHTTPCommunicator::GotData(QNetworkReply* reply)
{
    //Parse the JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    //Test validity
    if (!jsonDoc.isObject())
        return;

    QJsonObject topObject = jsonDoc.object();

    //Further sanity checks
    if((topObject.size()!=1) & (!topObject.begin().value().isArray()))
        return;

    //There's probably a better way to do this
    QString type = topObject.keys()[0];
    QJsonArray array = topObject.begin().value().toArray();

    emit NewTableArrived(type,array);
}

void CHTTPCommunicator::NetworkError(QNetworkReply::NetworkError error)
{
    qDebug()<<"error! "<<error;

}
