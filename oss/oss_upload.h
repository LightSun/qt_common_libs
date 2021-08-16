#ifndef OSS_UPLOAD_H
#define OSS_UPLOAD_H

#include <functional>
#include <QString>
#include <QObject>
#include <QStringList>
#include <alibabacloud/oss/OssClient.h>
using namespace AlibabaCloud::OSS;

namespace h7 {
    typedef struct {
        QString AccessKeyId = "yourAccessKeyId";
        QString AccessKeySecret = "yourAccessKeySecret";
        QString Endpoint = "yourEndpoint";
        QString BucketName = "yourBucketName";
        QString SecurityToken = "";
        uint32_t Expiration = 0;
        /* eg: abc/efg/123.jpg */
        //QString ObjectName = "yourObjectName";
    } OssInfo;

    class Oss_Uploader: public QObject
    {
        Q_OBJECT
    public:
        ~Oss_Uploader();
        void resetOssClient();
        bool upload(const QString& file, QString& outObjectName);

        bool uploadAll(QStringList files, QStringList& outObjNames);

        void uploadAllAsync(QStringList files, std::function<void(bool,QStringList&, QStringList&)> func);

        Oss_Uploader& asTest();
        static Oss_Uploader* get();

        static void init();
        static void destroy();

        OssInfo& getOssInfo();

    signals:
        void _log(int level, const QString& msg);
    private:
        Oss_Uploader();
        OssClient* m_client;
        OssInfo m_ossInfo;
        std::mutex m_mutex;
        bool m_test;
    };

}

#endif // OSS_UPLOAD_H
