#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QPainter>
#include <QRectF>
#include <QPointF>
#include <QJsonObject>


namespace h7 {
     void yv12toYUV(char *outYuv, char *inYv12, int width, int height,int widthStep);

     void toInts(const QString& str,const QString& sep, QList<int>* out);
     void toRect(const QString& str, const QString& sep, QRect& out);
     void toRectLTRB(const QString& str, const QString& sep, QRect& out);
     void copyRect(QRect& in, QRect& out);
     void scaleRect(QRect& in, float factor);

     void mkdirs(QString& file);
     bool getFilename(QString& file, QString& out); //xxx/xxx/aa.jpg -> aa.jpg
     bool getRelativeFilePath(QString& file, QString prefix,QString& out);

     void getMaxSize(int srcW,int srcH,int maxW, int maxH, int& outW, int& outH);
     void getMinSize(int srcW,int srcH,int maxW, int maxH, int& outW, int& outH);

     void logRect(QRect rect, QString prefix = QString(""));
     void logRectF(QRectF rect, QString prefix = QString(""));
     void logPoint(QPoint rect, QString prefix = QString(""));
     void logPointF(QPointF rect, QString prefix = QString(""));

     void getRGBAColor(unsigned int value, QColor& c);
     unsigned int toRGBAColor(QColor& c);

     void drawRectF(QPainter& painter, QPointF& p1, QPointF& p2);
     void drawRectF2(QPainter& painter, QRectF& p1);
     bool readFileLines(QString& path, QList<QString>& list);

     bool saveJsonToFile(QJsonObject& json, QString& filePath);
     QJsonObject loadJsonFromFile(const QString& filePath, bool* ok = NULL);
     QString readStrings(const QString& filePath, bool* ok = NULL);

     QStringList getImageFiles(QString dir);
     QStringList getJsonFiles(QString dir);
     QStringList findFiles(const QString &startDir, QStringList filters);
     void findFiles(const QString &startDir, QStringList& filters, QMap<QString, QStringList>& filesMap);

     QString gen_uuid(const QString& sed);
}

#endif // UTILS_H
