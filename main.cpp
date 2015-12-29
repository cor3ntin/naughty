#include "genericnotificationbox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GenericNotificationBox w;
    QMetaObject::invokeMethod(&w, "start", Qt::QueuedConnection);
    return a.exec();
}
