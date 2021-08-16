#include <QTextStream>
#include <QApplication>
#include <QDebug>
#include "qt_h7_log.h"
//#include "localconfig.h"


namespace h7 {
    LocalLogWriter::~LocalLogWriter(){
       destroy();
    }
    LocalLogWriter::LocalLogWriter(): file(NULL){

    }
    LocalLogWriter::LocalLogWriter(const QString &filePath){
        //QString filePath = LocalConfig::getFilePath(path);
        if(QFile::exists(filePath)){
             QFile::remove(filePath);
        }
        file = new QFile(filePath);
        if(!file->open(QFile::OpenModeFlag::WriteOnly|QFile::OpenModeFlag::Append|QFile::OpenModeFlag::Text)){
            qDebug() << "open file failed. " << filePath;
        }
    }
    void LocalLogWriter::destroy(){
        fileLock.lock();
        if(file != NULL){
            file->close();
            delete file;
            file = NULL;
        }
        fileLock.unlock();
    }
    void LocalLogWriter::write(int level, const QString &msg){
        QString str;
        switch (level) {
        case H7_LOG_LEVEL_INFO:
            str.append("((Info))>>> ");
            break;

        case H7_LOG_LEVEL_WARN:
            str.append("((Warn))>>> ");
            break;

        case H7_LOG_LEVEL_ERROR:
            str.append("((Error))>>> ");
            break;

        case H7_LOG_LEVEL_DEBUG:
        default:
            str.append("((Debug))>>> ");
            break;
        }
        str.append(msg).append("\r\n");
        qDebug() << str;
        if(file != NULL){
            fileLock.lock();
            QTextStream writer(file);
            writer << str;
            writer.flush();
            fileLock.unlock();
        }
    }

    static LocalLogWriter* sIns = NULL;
    void initGlobalLogger(const QString& path){
        if(sIns == NULL){
            sIns = new LocalLogWriter(path);
        }
    }
    LocalLogWriter* getGlobalLogger(){
        return sIns;
    }
}
