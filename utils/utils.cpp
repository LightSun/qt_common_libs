
#include <QDebug>
#include <QTextCodec>
#include <QDir>
#include <QDebug>
#include <QtMath>
#include <QDateTime>
#include <QMap>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QTextStream>
#include "utils.h"
#include "qtutils.h"
#include "../pure_cpp/sole.hpp" // for guid

static void appendBuf(std::string &main, const char* format, QString& arg){
    std::string str = arg.toStdString();
    char buf[1024];
    snprintf(buf, 1024, format, str.c_str());
    main.append(buf);
}
/*
QString h7::htr(const char* str){
   return QString::fromLocal8Bit(str);
}*/

namespace h7 {

    static QString readElementText0(QXmlStreamReader& reader, const char* ele){
        if(reader.isStartElement()){
            if(reader.name() == ele){
                QString text = reader.readElementText();
                if(reader.isEndElement()){
                    reader.readNext();
                }
                return text;
            }
        }
        return QString("");
    }

    void yv12toYUV(char *outYuv, char *inYv12, int width, int height,int widthStep)
    {
        int col,row;
        unsigned int Y,U,V;
        int tmp;
        int idx;
        for (row=0; row<height; row++)
        {
            idx=row * widthStep;
            int rowptr=row*width;
            for (col=0; col<width; col++)
            {
                tmp = (row/2)*(width/2)+(col/2);
                Y=(unsigned int) inYv12[row*width+col];
                U=(unsigned int) inYv12[width*height + width*height/4+tmp];
                V=(unsigned int) inYv12[width*height+tmp];
                outYuv[idx+col*3]   = Y;
                outYuv[idx+col*3+1] = U;
                outYuv[idx+col*3+2] = V;
            }
        }
    }
    void toInts(const QString& str, const QString& sep, QList<int>* out){
        QStringList list = str.split(sep);
        auto travel = [](void* ud, QString& a){
            QList<int>* ints = static_cast<QList<int>*>(ud);
            ints->append(a.toInt());
        };
        travelList<QString>(out, list, travel);
    }
     void toRect(const QString& str, const QString& sep, QRect& out){
        QList<int> rects;
        toInts(str, sep, &rects);
        out.setLeft(rects.value(0));
        out.setTop(rects.value(1));
        out.setWidth(rects.value(2));
        out.setHeight(rects.value(3));
     }
     void toRectLTRB(const QString& str, const QString& sep, QRect& out){
        QList<int> rects;
        toInts(str, sep, &rects);
        out.setLeft(rects.value(0));
        out.setTop(rects.value(1));
        out.setRight(rects.value(2));
        out.setBottom(rects.value(3));
     }
      void copyRect(QRect& in, QRect& out){
         int x1,y1,x2,y2;
         in.getCoords(&x1, &y1, &x2, &y2);
         out.setCoords(x1, y1, x2, y2);
      }

      void scaleRect(QRect& in, float factor){
          int x1,y1,x2,y2;
          in.getCoords(&x1, &y1, &x2, &y2);
          in.setCoords((int)(x1 * factor),
                       (int)(y1 * factor),
                       (int)(x2 * factor),
                       (int)(y2 * factor)
                       );
      }

     void mkdirs(QString& file){
        if(!QFile::exists(file)){
            QString dirPath = file.mid(0, file.lastIndexOf("/"));
            QDir dir;
            if(!dir.exists(dirPath)){
                dir.mkpath(dirPath);
            }
        }
     }
     bool getFilename(QString& file, QString& out){
         int index = file.lastIndexOf("/");
         if(index < 0 ){
             return false;
         }
         int n = file.length() - file.mid(0, index + 1).length();
         out.append(file.mid(index + 1, n));
         return true;
     }
     bool getRelativeFilePath(QString& file, QString prefix,QString& out){
         int index = file.indexOf(prefix);
         if(index < 0 ){
             return false;
         }
         out.append(file.mid(index + prefix.length(), file.length() - prefix.length()));
         return true;
     }

     void getMaxSize(int srcW,int srcH,int maxW, int maxH, int& outW, int& outH){
         float sy = maxH * 1.0f/ srcH;
         float sx = maxW * 1.0f/ srcW;
         float s = qMax(sx, sy);
         outW = srcW * s;
         outH = srcH * s;
     }

     void getMinSize(int srcW,int srcH,int maxW, int maxH, int& outW, int& outH){
         float sy = maxH * 1.0f/ srcH;
         float sx = maxW * 1.0f/ srcW;
         float s = qMin(sx, sy);
         outW = srcW * s;
         outH = srcH * s;
     }
    void logRect(QRect rect, QString prefix){
        qDebug() << QString("%1 >> rect: x,y, w,h =(%2,%3, %4,%5)").arg(prefix)
                    .arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    }
    void logRectF(QRectF rect, QString prefix){
        qDebug() << QString("%1 >> rect: x,y, w,h =(%2,%3, %4,%5)").arg(prefix)
                    .arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    }
    void logPoint(QPoint rect, QString prefix){
        qDebug() << QString("%1 >> rect: x,y=(%2,%3)").arg(prefix)
                    .arg(rect.x()).arg(rect.y());
    }
    void logPointF(QPointF rect, QString prefix){
        qDebug() << QString("%1 >> rect: x,y=(%2,%3)").arg(prefix)
                    .arg(rect.x()).arg(rect.y());
    }
    void getRGBAColor(unsigned int value, QColor& c){
        int r = value >> 24 & 0xff;
        int g = value >> 16 & 0xff;
        int b = value >> 8 & 0xff;
        int a = value & 0xff;
       // QString val16 = QString("%1").arg(value, 8,16, QLatin1Char('0'));
       // qDebug() << QString("raw color is : 0x%1. cast. r,g,b,a = %2, %3, %4,%5")
        //            .arg(val16).arg(r).arg(g).arg(b).arg(a);
        c.setRgb(r, g, b, a);
    }
     unsigned int toRGBAColor(QColor& c){
         int r, g, b,a;
         c.getRgb(&r, &g, &b, &a);
         return r << 24 | g << 16 | b << 8 | a;
     }

     void drawRectF(QPainter& painter, QPointF& p1, QPointF& p2){
         static QPointF tmp1, tmp2;
         tmp1.setX(p1.x());
         tmp1.setY(p2.y());
         tmp2.setX(p2.x());
         tmp2.setY(p1.y());
         painter.drawLine(p1, tmp2);
         painter.drawLine(tmp2, p2);
         painter.drawLine(p2, tmp1);
         painter.drawLine(tmp1, p1);
     }
     void drawRectF2(QPainter& painter, QRectF& rect){
          QPointF p1 = rect.topLeft();
          QPointF p2 = rect.bottomRight();
          drawRectF(painter, p1, p2);
     }
     QString readStrings(const QString& filePath, bool* ok){
         QFile file(filePath);
         if(!file.open(QFile::OpenModeFlag::ReadOnly|QFile::OpenModeFlag::Text)){
             qDebug() << "mark_loadInfo >> open file failed. " << filePath;
             if(ok) *ok = false;
             return "";
         }
         QByteArray data = file.readAll();
         file.close();
         if(ok) *ok = true;
         return data;
     }
      bool readFileLines(QString& path, QList<QString>& list){
         if(QFile::exists(path)){
            QFile file(path);
            if(file.open(QIODevice::ReadOnly | QIODevice::Text)){

                while (!file.atEnd())
                {
                    list.append(QString(file.readLine()));
                }
                file.close();
                return true;
            }
         }
         return false;
      }

      bool saveJsonToFile(QJsonObject& json,QString& filePath){
          //save
          QJsonDocument document;
          document.setObject(json);
          QByteArray byteArray = document.toJson(QJsonDocument::Compact);
          QString strJson(byteArray);

          mkdirs(filePath);
          if(QFile::exists(filePath)){
              QFile::remove(filePath);
          }
          QFile file(filePath);
          if(!file.open(QFile::OpenModeFlag::WriteOnly|QFile::OpenModeFlag::Append|QFile::OpenModeFlag::Text)){
              qDebug() << "mark_saveInfo >> open file failed. " << filePath;
              return false;
          }
          QTextStream writer(&file);
          writer << strJson;
          writer.flush();
          file.close();
          return true;
      }
       QJsonObject loadJsonFromFile(const QString& filePath, bool* ok){
           QFile file(filePath);
           if(!file.open(QFile::OpenModeFlag::ReadOnly|QFile::OpenModeFlag::Text)){
               qDebug() << "mark_loadInfo >> open file failed. " << filePath;
               if(ok)
                   *ok = false;
               return QJsonObject();
           }
           QByteArray data = file.readAll();
           file.close();
           QJsonDocument document = QJsonDocument::fromJson(data);
           if(document.isNull()){
               if(ok)
                   *ok = false;
               return QJsonObject();
           }
           if(ok)
               *ok = true;
           return document.object();
       }

        QStringList getImageFiles(QString dir){
            QStringList filters;
            filters << QString("*.jpg") << QString("*.png");
            return findFiles(dir, filters);
        }
        QStringList getJsonFiles(QString dir){
            QStringList filters;
            filters << QString("*.json");
            return findFiles(dir, filters);
        }
        //QStringList files = findFiles("./", QStringList() << "*.cpp" << "*.h");
        QStringList findFiles(const QString &startDir, QStringList filters)
        {
            QStringList names;
            QDir dir(startDir);

             //files
            foreach (QString file, dir.entryList(filters, QDir::Files))
                names += startDir + '/' + file;

            //dirs
            foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
                names += findFiles(startDir + '/' + subdir, filters);

            return names;
        }
        void findFiles(const QString &startDir, QStringList& filters,
                       QMap<QString, QStringList>& filesMap){
            qDebug() << "dir: " << startDir;
            QDir dir(startDir);
            if(!dir.exists()){
                return;
            }

            //files
            QStringList list;
            foreach (QString file, dir.entryList(filters, QDir::Files)) {
              list.append(startDir + "/" + file);
            }
            filesMap.insert(startDir, list);
            //dirs
            foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
                findFiles(startDir + '/' + subdir, filters, filesMap);
        }
        QString gen_uuid(const QString& sed){
            sole::uuid uid4;
            if(sed.isEmpty()){
                uid4 = sole::uuid4();
            }else{
                uid4 = sole::rebuild(sed.toStdString());
            }
            return QString::fromStdString(uid4.base62()).replace("-", "_");
        }
}
