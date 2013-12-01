#include <QApplication>
#include "desktopnotification.h"
#include <QVariant>
#include <QTimer>
#include <QAction>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("MyApplication");

    QApplication a(argc, argv);
    DesktopNotificationManager manager;

    /*if(!manager.setBackend("generic")) {
        return -1;
    }*/

    DesktopNotification* notification = manager.createNotification(&a,
                                                                   "Awesome notification",
                                                                   "This is an <i>awesome</i> notification.<br/>And there is <a href=\"http://google.com\">a link</a>.");
    if(notification == 0)
        return 2;

    manager.setDefaultIcon(QImage(":/qt.png"));
    manager.setDefaultHint(DesktopNotification::NH_GenericScreenCorner, Qt::TopLeftCorner);

    QAction act("Play", 0);
    QObject::connect(&act, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act);

    QAction act2("DoSomeStuff", 0);
    QObject::connect(&act2, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act2);

    QObject::connect(notification, SIGNAL(closed(NotificationClosedReason)), qApp, SLOT(quit()));

    QMetaObject::invokeMethod(notification, "show", Qt::QueuedConnection);
    QObject::connect(notification, SIGNAL(closed(NotificationClosedReason)), qApp, SLOT(quit()));

    //QTimer::singleShot(3000, notification, SLOT(show()));
    return a.exec();
}
