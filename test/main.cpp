#include <QCoreApplication>
#include "desktopnotification.h"
#include <QVariant>
#include <QTimer>
#include <QAction>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DesktopNotificationManager manager;
    manager.setBackend("libnotify");

    DesktopNotification* notification = manager.createNotification(&a, "Test", "this is a test.<br/><b>LOL</b> <a>http://google.com</a>");

    notification->setIcon(QImage("bantrackplateform.png"));
    notification->setHint(DesktopNotification::NH_Timeout, 20 * 1000);

    QAction act("Play", 0);
    QObject::connect(&act, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act);

    QAction act2("DoSomeStuff", 0);
    QObject::connect(&act2, SIGNAL(triggered()), qApp, SLOT(quit()));
    notification->addAction(&act2);

    QMetaObject::invokeMethod(notification, "show", Qt::QueuedConnection);
    //QTimer::singleShot(3000, qApp, SLOT(quit()));
    return a.exec();
}
