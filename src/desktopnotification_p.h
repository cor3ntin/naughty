#ifndef DESKTOPNOTIFICATION_P_H
#define DESKTOPNOTIFICATION_P_H

#include "desktopnotification.h"
#include <QVariant>
#include <QMap>
#include <QPointer>

class AbstractDesktopNotificationBackend;
class DesktopNotificationManagerPrivate {
public:
    AbstractDesktopNotificationBackend* backend;
};

class DesktopNotification;
class DesktopNotificationPrivate {
public:
    AbstractDesktopNotificationBackend* backend;
    DesktopNotificationManager* manager;
    DesktopNotification::HintMap hints;
    QList<QAction*> actions;
    bool visible;


    //Q_DECLARE_PUBLIC(DesktopNotification)

};

#endif // DESKTOPNOTIFICATION_P_H