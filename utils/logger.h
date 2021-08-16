#ifndef QT_H7_LOG_H
#define QT_H7_LOG_H

#include <QFile>
#include <QMutex>
#include <QObject>

#define H7_LOG_LEVEL_DEBUG 1
#define H7_LOG_LEVEL_INFO 2
#define H7_LOG_LEVEL_WARN 3
#define H7_LOG_LEVEL_ERROR 4


namespace h7 {
   /* class LogSender{

    public:
        signals:
        void logMessage(int level, const QString& msg);
    };
    */

    class LocalLogWriter: public QObject{
        Q_OBJECT
    public:
        ~LocalLogWriter();
        LocalLogWriter();
        LocalLogWriter(const QString& path);
        void destroy();
public Q_SLOTS:
        void write(int level, const QString& msg);
    private:
        QFile* file;
        QMutex fileLock;
    };

    void initGlobalLogger(const QString& path);
    LocalLogWriter* getGlobalLogger();
}

#endif // QT_H7_LOG_H
