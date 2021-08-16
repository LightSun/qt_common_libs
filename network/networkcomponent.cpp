

#include <QJsonDocument>
#include <QCoreApplication>
#include "networkcomponent.h"

using namespace h7;

#define NET_COMPONENT_POST(reqType) \
m_netBuffer.clear();\
m_netReqType = reqType;\
connect(m_netReply, &QIODevice::readyRead, this, &NetworkComponent::onNetReadToRead);\
connect(m_netReply, &QNetworkReply::finished, this, &NetworkComponent::onNetReadFinished);\
connect(m_netReply, &QNetworkReply::sslErrors, this, &NetworkComponent::onSslError);\
connect(m_netReply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),\
        this, &NetworkComponent::onNetError);
//\
//connect(m_net, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), \
//        this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));

  //void sslErrors(const QList<QSslError> &errors);

static QNetworkAccessManager s_nam;

NetworkComponent::NetworkComponent(const QNetworkAccessManager* net):
    m_parts(NULL), m_need_release_netM(false), m_netReply(NULL), m_releaseAfterCb(true), m_ctx(NULL),
        m_func_normal(NULL), m_func_error(NULL), m_eventLoop(NULL)
{
    if(net == NULL){
        this->m_net = &s_nam;
    }else{
        this->m_net = const_cast<QNetworkAccessManager*>(net);
    }
}

NetworkComponent::~NetworkComponent(){
    if(m_parts != NULL){
        delete m_parts;
        m_parts = NULL;
    }
    if(m_need_release_netM){
        m_net->deleteLater();
    }
}

NetworkComponent& NetworkComponent::url(const QString &url){
    if(url.startsWith("https:")){
        QSslConfiguration conf = m_request.sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        conf.setProtocol(QSsl::TlsV1SslV3);
        m_request.setSslConfiguration(conf);
    }
    m_url0 = url;
    return *this;
}
NetworkComponent& NetworkComponent::releaseAfterCallback(bool release){
    m_releaseAfterCb = release;
    return *this;
}
NetworkComponent& NetworkComponent::header(QNetworkRequest::KnownHeaders key,const QString &value){
    m_header0.insert(key, value);
    return *this;
}

NetworkComponent& NetworkComponent::header(const QString& key,const QString &value){
    m_rawHeader.insert(key, value);
    return *this;
}

NetworkComponent& NetworkComponent::body(const QByteArray &body){
    m_bodyBytes.append(body);
    m_bodyAsBytes = true;
    return *this;
}
NetworkComponent& NetworkComponent::header(const QMap<QString, QString> &_header){
    auto end = _header.cend();
    for (auto it = _header.cbegin(); it != end; ++it)
    {
        m_rawHeader.insert(it.key(), it.value());
    }
    return *this;
}

NetworkComponent& NetworkComponent::body(const QMap<QString, QString> &body){
    auto end = body.cend();
    for (auto it = body.cbegin(); it != end; ++it)
    {
        m_bodys.insert(it.key(), it.value());
    }
    m_bodyAsBytes = false;
    return *this;
}
NetworkComponent& NetworkComponent::body(const QString &key,const  QString &value){
    m_bodys.insert(key, value);
    m_bodyAsBytes = false;
    return *this;
}
NetworkComponent& NetworkComponent::keyValue(const QString &key,const  QString &value){
    m_bodys.insert(key, value);
    m_bodyAsBytes = false;
    return *this;
}

void NetworkComponent::buildData(QByteArray &out){
    if(!m_bodyAsBytes){
        QString data = "";
        bool first = true;
        auto end = m_bodys.cend();
        for (auto it = m_bodys.cbegin(); it != end; ++it)
        {
            if(!first){
                data.append("&");
            }else {
                first = false;
            }
            data.append(it.key()).append("=").append(it.value());
            if(m_debug0){
                qDebug() << "body: key,value = " << it.key() << ", " << it.value();
            }
        }
        out.append(data.toUtf8());
    }else{
        out.append(m_bodyBytes);
    }
}
NetworkComponent& NetworkComponent::debug(bool debug){
    m_debug0 = debug;
    return *this;
}
NetworkComponent& NetworkComponent::part(const QHttpPart& part){
    if(m_parts == NULL){
        m_parts = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    }
    m_parts->append(part);
    return *this;
}

QNetworkReply* NetworkComponent::getReply(){
    setHeaders();
    //param
    {
        auto end = m_bodys.cend();
        bool first = true;
        QString key, value;
        for (auto it = m_bodys.cbegin(); it != end; ++it)
        {
            key = it.key().toUtf8();
            value = it.value().toUtf8();
            if(!first){
                m_url0.append("&").append(QString("%1=%2").arg(key).arg(value));
            }else{
                first = false;
                m_url0.append("?")
                    .append(QString("%1=%2").arg(key).arg(value));
            }
        }
        if(m_debug0){
            qDebug() << "request url = " << m_url0;
        }
    }
    m_request.setUrl(QUrl(m_url0));
    return m_net->get(m_request);
}
QNetworkReply* NetworkComponent::postReply(){
    if(m_debug0){
        qDebug() << "request url = " << m_url0;
    }
    m_request.setUrl(QUrl(m_url0));

    setHeaders();
    QByteArray bodyData;
    buildData(bodyData);
    if(m_parts != NULL){
        QNetworkReply* reply = m_net->post(m_request, m_parts);
        m_parts->setParent(reply);
        return reply;
    }
    return m_net->post(m_request, bodyData);
}
QNetworkReply* NetworkComponent::putReply(){
    if(m_debug0){
        qDebug() << "request url = " << m_url0;
    }
    m_request.setUrl(QUrl(m_url0));
    setHeaders();
    QByteArray bodyData;
    buildData(bodyData);
    if(m_parts != NULL){
        QNetworkReply* reply = m_net->put(m_request, m_parts);
        m_parts->setParent(reply);
        return reply;
    }
    return m_net->put(m_request, bodyData);
}

NetworkComponent& NetworkComponent::new1(const QNetworkAccessManager* net){
    return *(new NetworkComponent(net));
}

NetworkComponent& NetworkComponent::new1(const QNetworkAccessManager* net, bool autoRelease){
    NetworkComponent* nc = new NetworkComponent(net);
    nc->m_need_release_netM = autoRelease;
    return *nc;
}

void *NetworkComponent::getContext() const
{
    return m_ctx;
}

NetworkComponent& NetworkComponent::context(void *ctx)
{
    m_ctx = ctx;
    return *this;
}

void NetworkComponent::setHeaders(){
    {
        auto end = m_header0.cend();
        for (auto it = m_header0.cbegin(); it != end; ++it)
        {
            if(m_debug0){
                qDebug() << "header: " << it.key() << " = " << it.value().toUtf8();
            }
            m_request.setHeader(it.key(), it.value());
        }
    }
    {
        auto end = m_rawHeader.cend();
        for (auto it = m_rawHeader.cbegin(); it != end; ++it)
        {
            if(m_debug0){
                qDebug() << "raw header: " << it.key() << " = " << it.value().toUtf8();
            }
            m_request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
        }
    }
}

void NetworkComponent::get(){
    Q_ASSERT(m_eventLoop == NULL);
    m_eventLoop = new QEventLoop();
    if(m_net->networkAccessible() == QNetworkAccessManager::NotAccessible){
        m_net->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    m_netReply = getReply();
    NET_COMPONENT_POST(0);
    //if(m_debug0) qDebug() << "start http/https get()";

    m_loopCode = m_eventLoop->exec();
}

void NetworkComponent::post(){
    Q_ASSERT(m_eventLoop == NULL);
    m_eventLoop = new QEventLoop();
    if(m_net->networkAccessible() == QNetworkAccessManager::NotAccessible){
        m_net->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    m_netReply = postReply();
    NET_COMPONENT_POST(0);
    m_loopCode = m_eventLoop->exec();
}

void NetworkComponent::put(){
    Q_ASSERT(m_eventLoop == NULL);
    m_eventLoop = new QEventLoop();
    if(m_net->networkAccessible() == QNetworkAccessManager::NotAccessible){
        m_net->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(120));
    m_netReply = putReply();
    NET_COMPONENT_POST(0);

    m_loopCode = m_eventLoop->exec();
}
NetworkComponent& NetworkComponent::header(std::function<void(QString&,NetKnownHeaderMap&, NetMap&)> func){
    func(m_url0, m_header0, m_rawHeader);
    return *this;
}
NetworkComponent& NetworkComponent::body(std::function<QHttpMultiPart*(QString&, NetMap&, QByteArray&)> func){
    NetMap _body;
    QByteArray _bodyBytes;
    m_parts = (QHttpMultiPart*)func(m_url0, _body, _bodyBytes);
    if(m_parts == NULL){
        if(_bodyBytes.size() > 0){
            return body(_bodyBytes);
        }else{
            return body(_body);
        }
    }else{
        return *this;
    }
}
NetworkComponent& NetworkComponent::callback(std::function<void(NetworkComponent&,QString&,QByteArray&)> func){
    m_func_normal = FUNC_MAKE_SHARED_PTR_3(void(NetworkComponent&,QString&,QByteArray&), func);
    return *this;
}
NetworkComponent& NetworkComponent::callback_error(std::function<void(NetworkComponent&,QString&,int, const QString&)> func){
    m_func_error = FUNC_MAKE_SHARED_PTR_4(void(NetworkComponent&,QString&,int, const QString&), func);
    return *this;
}

void NetworkComponent::onNetReadToRead(){
    if(m_debug0) qDebug() << "OnDataReadyToRead()";
    QVariant statusCode = m_netReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        if(m_debug0) qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = m_netReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        if(m_debug0)  qDebug() << "reason=" << reason.toString();
    QNetworkReply::NetworkError err = m_netReply->error();
    if(err != QNetworkReply::NoError) {
        if(m_debug0) qDebug() << "Failed: " << m_netReply->errorString();
    }else{
        m_netBuffer.append(m_netReply->readAll());
    }
}

#define EVEVT_LOOP_EXIT()\
if(m_eventLoop != NULL){\
    m_eventLoop->exit(m_loopCode);\
    m_eventLoop->deleteLater();\
    m_eventLoop = NULL;\
}

void NetworkComponent::onNetReadFinished(){

    m_netReply->deleteLater();
    if(m_debug0) qDebug() << m_netBuffer;

    if(m_func_normal){
        (*m_func_normal)(*this, m_url0, m_netBuffer);
    }
    if(m_releaseAfterCb){
        deleteLater();
    }
    EVEVT_LOOP_EXIT();
}
void NetworkComponent::onNetError(QNetworkReply::NetworkError e){
    /*if(m_netReply->isRunning()){
        m_netReply->close();
    }*/
    m_netReply->deleteLater();
    if(m_debug0) qDebug() << "onNetError: " << e;
    if(m_func_error){
        (*m_func_error)(*this, m_url0, e, QString());
    }
    if(m_releaseAfterCb){
        deleteLater();
    }
    EVEVT_LOOP_EXIT();
}
void NetworkComponent::onSslError(const QList<QSslError> &errors){

    m_netReply->deleteLater();
    QString str;
    foreach(QSslError se, errors){
        str.append(se.errorString()).append("\r\n");
       if(m_debug0) qDebug() << "onSslError: " << se.errorString();
    }
    if(m_func_error){
       (*m_func_error)(*this, m_url0, 0, str);
    }
    if(m_releaseAfterCb){
        deleteLater();
    }
    EVEVT_LOOP_EXIT();
}
void NetworkComponent::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible){
    if(accessible != QNetworkAccessManager::Accessible)
     {
         // case where the network is not available
        qDebug() << "networkAccessibleChanged::::::::::::::: not available";
     }else{
        qDebug() << "networkAccessibleChanged::::::::::::::: available";
     }
}

h7::NetworkComponent& newNetworkCom(const QNetworkAccessManager* net){
    return h7::NetworkComponent::new1(net);
}
h7::NetworkComponent& newNetworkComAbsolute(){
    return h7::NetworkComponent::new1(new QNetworkAccessManager(), true);
}
//---------------------------------------------------
typedef struct _Node {
     bool needReleaseObj;
     QString name; //name for end object/array.
     QJsonObject* obj;
     QJsonArray* array;
     QList<struct _Node*> children;
     struct _Node* parent;
}Node;

Node* newNode(Node* parent, QJsonObject* obj, bool needReleaseObj = true);
Node* newArrayNode(Node* parent);
void deleteNode(Node* node);
//---------------------------------------------
h7::JsonBuilder newJsonBuilder(){
    return h7::JsonBuilder();
}

JsonBuilder::JsonBuilder(){
    m_current = newNode(NULL, &m_root, false);
}
JsonBuilder::~JsonBuilder(){
    Node* last;
    Node* rootNode = (Node*)m_current;
    do{
        last = rootNode;
        rootNode = rootNode->parent;
    }while(rootNode != NULL);
    deleteNode(last);
}
JsonBuilder& JsonBuilder::pair(const QString& key, const QJsonValue& value){
    return append(key, value);
}
JsonBuilder& JsonBuilder::append(const QString& key, const QJsonValue& value){
    if(!((Node*)m_current)->obj){
        qErrnoWarning("error: pair only used for json object.");
        return *this;
    }
    ((Node*)m_current)->obj->insert(key, value);
    //qDebug() << "json size = " << ((Node*)m_current)->obj->size();
    return *this;
}
JsonBuilder& JsonBuilder::append(const QJsonValue& value){
    if(((Node*)m_current)->obj){
        qErrnoWarning("error: append(QJsonValue) only used for json-array.");
        return *this;
    }
    ((Node*)m_current)->array->append(value);
    return *this;
}

JsonBuilder& JsonBuilder::beginObject(const QString& key){
    ((Node*)m_current)->name = key;
    m_current = newNode((Node*)m_current, NULL, true);
    return *this;
}
JsonBuilder& JsonBuilder::beginArray(const QString& key){
    ((Node*)m_current)->name = key;
    m_current = newArrayNode((Node*)m_current);
    return *this;
}
JsonBuilder& JsonBuilder::endObject(){
    if(((Node*)m_current)->array){
        qErrnoWarning("error: endArray only used for json-array.");
        return *this;
    }
    if(((Node*)m_current)->parent == NULL){
        qErrnoWarning("error: endObject already the root.");
        return *this;
    }
    doEnd();
    m_current = ((Node*)m_current)->parent;
    return *this;
}
JsonBuilder& JsonBuilder::endArray(){
    if(((Node*)m_current)->obj){
        qErrnoWarning("error: endArray only used for json-array.");
        return *this;
    }
    if(((Node*)m_current)->parent == NULL){
        qErrnoWarning("error: endArray already the root.");
        return *this;
    }
    doEnd();
    m_current = ((Node*)m_current)->parent;
    return *this;
}
QJsonObject& JsonBuilder::toObject(){
    return m_root;
}
QByteArray JsonBuilder::toBytes(){
    QJsonDocument doc;
    doc.setObject(m_root);
    //qDebug() << "doc.isEmpty(): "<< doc.isEmpty();
    return doc.toJson(QJsonDocument::Compact);
}

Node* newNode(Node* parent, QJsonObject* obj, bool needReleaseObj){
    Node* node = new Node();
    if(parent != NULL){
        parent->children.append(node);
    }
    node->name = "";
    node->parent = parent;

    node->obj = obj != NULL ? obj : new QJsonObject();
    node->array = NULL;
    node->needReleaseObj = obj == NULL ? true : needReleaseObj;
    return node;
}
Node* newArrayNode(Node* parent){
    Node* node = new Node();
    node->name = "";
    if(parent != NULL){
        parent->children.append(node);
    }
    node->parent = parent;
    node->obj = NULL;
    node->array = new QJsonArray();
    node->needReleaseObj = false;
    return node;
}
void deleteNode(Node* node){
    if(!node){
        qDebug() << "deleteNode: NULL";
        return;
    }
    if(node->needReleaseObj && node->obj){
        delete node->obj;
        node->obj = NULL;
    }
    if(node->array){
        delete node->array;
        node->array = NULL;
    }
    if(node->children.size() > 0 ){
        for(int i = 0 ; i < node->children.size() ; i ++){
            deleteNode(node->children.value(i));
        }
        node->children.clear();
    }
    delete node;
}
void JsonBuilder::doEnd(){
    Node* parent = ((Node*)m_current)->parent;
    if(parent->obj){
        if(((Node*)m_current)->obj){
            parent->obj->insert(parent->name, *((Node*)m_current)->obj);
        }else{
            parent->obj->insert(parent->name, *((Node*)m_current)->array);
        }
    }else if(parent->array){
        if(((Node*)m_current)->obj){
            parent->array->append(*((Node*)m_current)->obj);
        }else{
            parent->array->append(*((Node*)m_current)->array);
        }
    }
}

