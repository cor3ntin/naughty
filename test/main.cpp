#include <QApplication>
#include "desktopnotification.h"
#include <QVariant>
#include <QTimer>
#include <QAction>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesktopNotificationManager manager;
    if(!manager.setBackend("growl")) {
        return -1;
    }

    DesktopNotification* notification = manager.createNotification(&a, "Test", "this is a <a href=\"http://google.com\">test.</a>");
    if(notification == 0)
        return 2;

    notification->setIcon(QImage("bantrackplateform.png"));
    notification->setHint(DesktopNotification::NH_Timeout, -1);

    QAction act("Play", 0);
    QObject::connect(&act, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act);

    QAction act2("DoSomeStuff", 0);
    QObject::connect(&act2, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act2);

    QMetaObject::invokeMethod(notification, "show", Qt::QueuedConnection);
    QObject::connect(notification, SIGNAL(closed(NotificationClosedReason)), qApp, SLOT(quit()));

    //QTimer::singleShot(3000, notification, SLOT(show()));
    return a.exec();
}
