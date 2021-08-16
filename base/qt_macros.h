#ifndef QT_MACROS_H
#define QT_MACROS_H

#include <QObject>

#define QT_CONNECT_LOG(sender, method1, receiver, method2) \
QObject::connect(sender, SIGNAL(method1(int,const QString&)),receiver, SLOT(method2(int,const QString&)));

#define QT_CONNECT_METHOD(sender, method1, receiver, method2, paramDesc) \
QObject::connect(sender, SIGNAL(method1(paramDesc)),receiver, SLOT(method2(paramDesc)));

#endif // QT_MACROS_H
