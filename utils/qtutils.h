#ifndef QTUTILS_H
#define QTUTILS_H

#include <QMap>
#include <QList>
#include <QString>
#include <QtMath>

template <typename K, typename V>
void travelMap(void* ud, const QMap<K, V>& map, bool (*Traveller)(void* ud, K&, V&)){
    auto end = map.cend();
    for (auto it = map.cbegin(); it != end; ++it)
    {
        //qDebug() << "header: " << it.key() << " = " << it.value();
        if(Traveller(ud, (K&)it.key(), (V&)it.value())){
            break;
        }
    }
}
template <typename T>
void travelList(void* ud, const QList<T>& set, void (*Traveller)(void* ud, T&)){
    auto end = set.cend();
    for (auto it = set.cbegin(); it != end; ++it)
    {
        //qDebug() << "header: " << it.key() << " = " << it.value();
        Traveller(ud, (T&)(*it));
    }
}

template <typename T>
void listToString(void* ud, const QList<T>& set,
                  QString (*ToString)(void* ud, T&, int index),
                  QString& out){
    auto end = set.cend();
    int index = 0;
    for (auto it = set.cbegin(); it != end; ++it)
    {
        //qDebug() << "header: " << it.key() << " = " << it.value();
        out.append(ToString(ud, (T&)*it, index++));
    }
}

template <typename T>
void freeList(QList<T*>& set){
    auto end = set.cend();
    for (auto it = set.cbegin(); it != end; ++it)
    {
        delete (T*)(*it);
    }
    set.clear();
}

template <typename T>
float square(const T& t){
    return (float)(t * t);
}

template <typename T>
float pointsDistance(const T& pt1, const T& pt2)
{
    float dx = pt2.x() - pt1.x();
    float dy = pt2.y() - pt1.y();
    return sqrt(double(square<float>(dx) + square<float>(dy)));
}

#endif // QTUTILS_H
