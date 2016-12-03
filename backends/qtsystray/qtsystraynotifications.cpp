#include "qtsystraynotifications.h"
#include <QtPlugin>
#include <QtDebug>
#include <QCoreApplication>
#include <QAction>
#include <QSystemTrayIcon>
#include <QTimer>

#include "desktopnotification_p.h"


class QtSystrayNotificationPrivate : public DesktopNotificationPrivate {

};

QtSystrayNotification::QtSystrayNotification(DesktopNotificationManager* manager, QObject* parent)
    :DesktopNotification(new QtSystrayNotificationPrivate, manager, parent) {

}
QtSystrayNotification::~QtSystrayNotification() {
    Q_D(const QtSystrayNotification);
    if(d->visible)
        d->backend->hide(this);
}

void QtSystrayNotification::onClicked() {
    was_clicked();

    Q_D(QtSystrayNotification);

    d->visible = false;
    d->backend->hide(this);
}

void QtSystrayNotification::onTimeout() {

    Q_D(QtSystrayNotification);

    if(!d->visible)
        return;

    d->backend->hide(this);

    Q_EMIT closed(DesktopNotification::NotificationExpired);
}


QString QtSystrayNotificationBackendFactory::name() const
{
    return QLatin1String("qtsystray");
}

DesktopNotificationManager::BackendCapabilities QtSystrayNotificationBackendFactory::capabilities() const
{
    return DesktopNotificationManager::IconNotificationCap |
            DesktopNotificationManager::TimeoutNotificationCap;
}

AbstractDesktopNotificationBackend* QtSystrayNotificationBackendFactory::backend(DesktopNotificationManager* manager) const
{
    if(!QSystemTrayIcon::supportsMessages())
        return 0;
    QSystemTrayIcon* icon = qobject_cast<QSystemTrayIcon*>(manager->defaultHint(DesktopNotification::NH_QtSystemTrayInstance).value<QObject*>());
    if(!icon)
        return 0;

    return new QtSystrayNotificationBackend(manager, icon);
}

QtSystrayNotificationBackend::QtSystrayNotificationBackend(DesktopNotificationManager* manager, QSystemTrayIcon* icon)
    :AbstractDesktopNotificationBackend(manager), m_icon(QPointer<QSystemTrayIcon>(icon)) {

}

QString QtSystrayNotificationBackend::name() const
{
    return QLatin1String("qtsystray");
}

DesktopNotificationManager::BackendCapabilities QtSystrayNotificationBackend::capabilities()
{
    return DesktopNotificationManager::IconNotificationCap |
            DesktopNotificationManager::TimeoutNotificationCap;
}

DesktopNotification* QtSystrayNotificationBackend::createNewNotification(DesktopNotificationManager* manager, QObject *parent) {
    return new QtSystrayNotification(manager, parent);
}

void QtSystrayNotificationBackend::show(DesktopNotification* notification)
{
    if(m_icon.isNull())
        return;

    Q_FOREACH(QPointer<DesktopNotification> n, m_notifications) {
        if(!n.isNull())
            n->hide();
    }

    int timeout = notification->hint(DesktopNotification::NH_Timeout, 12000).toInt();

    QObject::connect(m_icon.data(), SIGNAL(messageClicked()), notification, SLOT(onClicked()));

    if(timeout > 0)
        QTimer::singleShot(timeout, notification, SLOT(onTimeout()));



    m_icon->showMessage(notification->title(),
                        notification->message(),
                        QSystemTrayIcon::Information,
                        timeout);

    static_cast<QtSystrayNotification*>(notification)->d_ptr->visible = true;
    QPointer<DesktopNotification> ptr(notification);
    if(!m_notifications.contains(ptr))
        m_notifications.append(ptr);

}

void QtSystrayNotificationBackend::hide(DesktopNotification* notification)
{
    static_cast<QtSystrayNotification*>(notification)->d_ptr->visible = false;
    m_notifications.removeOne(QPointer<DesktopNotification>(notification));
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(desktopnotification, QtSystrayNotificationBackendFactory)
#endif
