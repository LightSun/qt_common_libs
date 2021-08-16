#ifndef NETWORKCOMPONENT_H
#define NETWORKCOMPONENT_H

//#include <QObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonArray>
#include <QJsonObject>
#include <QEventLoop>
#include <functional>

#include "../pure_cpp/commons.h"

namespace h7 {
    typedef QMap<QNetworkRequest::KnownHeaders, QString> NetKnownHeaderMap;
    typedef QMap<QString, QString> NetMap;
#define NETWORK_FUNC_CALLBACK(code) (h7::NetworkComponent& com,QString& url, QByteArray& res){code;}
#define NETWORK_FUNC_HEADER(code) (QString& url,h7::NetKnownHeaderMap& knownHeaderMap, h7::NetMap& headerMap){code;}
#define NETWORK_FUNC_BODY(code) (QString& url, NetMap& form, QByteArray& bytes){code;return (QHttpMultiPart*)NULL;}
#define NETWORK_FUNC_CALLBACK_ERROR(c) (NetworkComponent& com,QString& url,int code,const QString& msg){c;}

    class NetworkComponent: public QObject{
        Q_OBJECT
    private:
        NetKnownHeaderMap m_header0;
        NetMap m_rawHeader;
        NetMap m_bodys;
        QByteArray m_bodyBytes;
        bool m_bodyAsBytes;
        bool m_debug0 = true;
        QHttpMultiPart* m_parts;

        QNetworkAccessManager* m_net;
        bool m_need_release_netM;
        QNetworkRequest m_request;
        QString m_url0;

        QByteArray m_netBuffer;
        QNetworkReply* m_netReply;
        int m_netReqType;
        bool m_releaseAfterCb;
        void* m_ctx;

        int m_loopCode;
        QEventLoop* m_eventLoop;

        FUNC_SHARED_PTR(void(NetworkComponent&,QString&,QByteArray&)) m_func_normal;
        FUNC_SHARED_PTR(void(NetworkComponent&,QString&,int, const QString&)) m_func_error;

        void setHeaders();
        void buildData(QByteArray& out);

    private:
        NetworkComponent(const QNetworkAccessManager* m_net = NULL);

    public:
        ~NetworkComponent();
        static NetworkComponent& new1(const QNetworkAccessManager* m_net = NULL);
        static NetworkComponent& new1(const QNetworkAccessManager* m_net, bool autoRelease);

        NetworkComponent& releaseAfterCallback(bool release);
        NetworkComponent& header(QNetworkRequest::KnownHeaders key,const QString& value);
        NetworkComponent& header(const QString& key,const QString& value);
        NetworkComponent& header(const QMap<QString, QString>& _header);
        NetworkComponent& body(const QByteArray& body);
        NetworkComponent& body(const QMap<QString, QString>& body);
        NetworkComponent& body(const QString& key, const QString& value);
        NetworkComponent& keyValue(const QString& key,const QString& value);
        NetworkComponent& url(const QString& _url);
        NetworkComponent& debug(bool debug = true);
        //multi parts: https://www.freesion.com/article/2300250704/
        NetworkComponent& part(const QHttpPart& part);

        QNetworkReply* getReply();
        QNetworkReply* postReply();
        QNetworkReply* putReply();

        NetworkComponent& header(std::function<void(QString&,NetKnownHeaderMap&, NetMap&)> func);
        /**
         * @brief body set body attrs. support: string-key-value, byte-array, multiparts.
         * @param func the func to set body
         * @return
         */
        NetworkComponent& body(std::function<QHttpMultiPart*(QString&, NetMap&, QByteArray&)> func);
        NetworkComponent& callback(std::function<void(NetworkComponent&,QString&, QByteArray&)> func);
        NetworkComponent& callback_error(std::function<void(NetworkComponent&,QString&,int,const QString&)> func);
        //template<class F, class... Args>
        //void FgsbServerApi::doOnOssValid(F&& f, Args&&... args){
//        NetworkComponent& callback(std::function<void(NetworkComponent&,QString&, QByteArray&)> func);

        void get();
        void post();
        void put();

        void *getContext() const;
        NetworkComponent& context(void *ctx);

    public slots:
        void onNetReadToRead();
        void onNetReadFinished();
        void onNetError(QNetworkReply::NetworkError e);
        void onSslError(const QList<QSslError> &errors);
        void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);
    };

    class JsonBuilder{
    public:
        JsonBuilder();
        ~JsonBuilder();
        JsonBuilder& pair(const QString& key, const QJsonValue& value);
        JsonBuilder& append(const QString& key, const QJsonValue& value);
        JsonBuilder& append(const QJsonValue& value); //used for array
        JsonBuilder& beginObject(const QString& name="");
        JsonBuilder& beginArray(const QString& name="");
        JsonBuilder& endObject();
        JsonBuilder& endArray();
        QByteArray toBytes();
        QJsonObject& toObject();

    private:
        QJsonObject m_root;
        void* m_current;
        void doEnd();
    };
}

h7::NetworkComponent& newNetworkCom(const QNetworkAccessManager* net = NULL);
h7::NetworkComponent& newNetworkComAbsolute();
h7::JsonBuilder newJsonBuilder();

#endif // NETWORKCOMPONENT_H
