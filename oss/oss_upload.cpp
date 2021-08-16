#include <QDebug>
#include <QDateTime>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <QThread>

#include "oss_upload.h"
#include "../utils/utils.h"
#include "../utils/logger.h"

namespace h7 {

Oss_Uploader::Oss_Uploader(): m_client(NULL), m_test(false)
{
    init();
}

OssInfo& Oss_Uploader::getOssInfo()
{
    return m_ossInfo;
}

Oss_Uploader::~Oss_Uploader()
{
    if(m_client != NULL){
        delete m_client;
        m_client = NULL;
    }
    destroy();
}
Oss_Uploader& Oss_Uploader::asTest(){
    m_test = true;
    return *this;
}

void Oss_Uploader::init(){
    InitializeSdk();
}
void Oss_Uploader::destroy(){
    ShutdownSdk();
}
Oss_Uploader* Oss_Uploader::get(){
    static Oss_Uploader loader;
    return &loader;
}
void Oss_Uploader::resetOssClient(){
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_client != NULL){
        delete m_client;
        m_client = NULL;
    }
    ClientConfiguration conf;
    m_client = new OssClient(m_ossInfo.Endpoint.toStdString(),
                             m_ossInfo.AccessKeyId.toStdString(),
                             m_ossInfo.AccessKeySecret.toStdString(),
                             m_ossInfo.SecurityToken.toStdString(),
                             conf);
}

bool Oss_Uploader::upload(const QString& file, QString& outObjectName){
    QDateTime time = QDateTime::currentDateTime();
   // uint ts = time.toTime_t();
    QString day = time.toString("yyyy-MM-dd");

    QString simpleName;
    if(h7::getFilename((QString&)file, simpleName)){
        QString prefix = m_test ? "fgsb_test/" : "fgsb/";
        //retain file name
        outObjectName = prefix + day + "/" + simpleName;
    }else{
        qDebug() << "wrong file: " << file;
        emit _log(H7_LOG_LEVEL_WARN, QString("Oss_Uploader --- wrong file: %1").arg(file));
        return false;
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_client == NULL){
            ClientConfiguration conf;
            m_client = new OssClient(m_ossInfo.Endpoint.toStdString(),
                                     m_ossInfo.AccessKeyId.toStdString(),
                                     m_ossInfo.AccessKeySecret.toStdString(),
                                     m_ossInfo.SecurityToken.toStdString(),
                                     conf);
        }
    }

    //local file
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(file.toStdString(),
                                                                            std::ios::in | std::ios::binary);
    //memory file
    //std::shared_ptr<std::iostream> content = std::make_shared<std::stringstream>();
    //*content << "Thank you for using Alibaba Cloud Object Storage Service!";
    PutObjectRequest request(m_ossInfo.BucketName.toStdString(), outObjectName.toStdString(), content);
    //request.setVersionId("2017-05-25");

    /*（可选）请参见如下示例设置存储类型及访问权限ACL*/
    request.MetaData().addHeader("x-oss-object-acl", "public-read");//private,public-read,public-read-write
    request.MetaData().addHeader("x-oss-storage-class", "Standard");

    /* 上传文件 */
    auto outcome = m_client->PutObject(request);

    if (!outcome.isSuccess()) {
        /* 异常处理 */
        emit _log(H7_LOG_LEVEL_WARN, QString("(BucketName,endpoint,AccessKeyId,AccessKeySecret,SecurityToken)"
                                             "=\r\n(%1, %2, %3, %4, %5, %6)")
                  .arg(m_ossInfo.BucketName)
                  .arg(m_ossInfo.Endpoint)
                  .arg(m_ossInfo.AccessKeyId)
                  .arg(m_ossInfo.AccessKeySecret)
                  .arg(m_ossInfo.SecurityToken)
                  .arg(outObjectName)
                  );
        QString msg = QString("oss Uoload fail >>> code = %1, message = %2, reqId = %3")
                .arg(QString::fromStdString(outcome.error().Code()))
                .arg(QString::fromStdString(outcome.error().Message()))
                .arg(QString::fromStdString(outcome.error().RequestId()));
        qDebug() << msg;
        emit _log(H7_LOG_LEVEL_WARN, msg);
        return false;
    }
    return true;
}

bool Oss_Uploader::uploadAll(QStringList files, QStringList& outObjNames){
    QString objName;
    for(int i = 0 ; i < files.size(); i ++){
        if(!upload(files[i], objName)){
            return false;
        }
        outObjNames.append(objName);
    }
    return true;
}
//(state, all_files, success_files)
void Oss_Uploader::uploadAllAsync(QStringList files, std::function<void(bool, QStringList&, QStringList&)> func){
    auto task = std::make_shared<std::packaged_task<void(bool,QStringList&, QStringList&)>>(
        std::bind(func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
    auto run = [=](QStringList& files){
        QStringList objNames;
        if(!uploadAll(files, objNames)){
            (*task)(false, files, objNames);
        }else{
            (*task)(true, files, objNames);
        }
    };
    std::thread thd(run, std::ref(files));
    thd.join();
}

}

