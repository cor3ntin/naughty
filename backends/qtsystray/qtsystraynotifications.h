#ifndef LIBNOTIFYNOTIFICATIONS_H
#define LIBNOTIFYNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include <QtPlugin>

class QSystemTrayIcon;

class QtSystrayNotificationBackendFactory : public QObject, public AbstractDesktopNotificationBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(AbstractDesktopNotificationBackendFactory)
public:
    QString name() const;
    DesktopNotificationManager::BackendCapabilities capabilities() const;
    AbstractDesktopNotificationBackend* backend(DesktopNotificationManager* manager) const;
};

class QtSystrayNotificationBackend : public AbstractDesktopNotificationBackend
{
public:
    QtSystrayNotificationBackend(DesktopNotificationManager* manager, QSystemTrayIcon *icon);
    QString name() const;

    DesktopNotification* createNewNotification(DesktopNotificationManager* manager, QObject* parent = 0);

    void show(DesktopNotification*);
    void hide(DesktopNotification*notification);
    DesktopNotificationManager::BackendCapabilities capabilities();

private:
    QPointer<QSystemTrayIcon> m_icon;
};

class QtSystrayNotificationPrivate;
class QtSystrayNotification : DesktopNotification {

    friend class QtSystrayNotificationBackend;
    Q_DECLARE_PRIVATE(QtSystrayNotification)
    Q_OBJECT

protected:
    QtSystrayNotification(DesktopNotificationManager* manager, QObject* parent = 0);
    ~QtSystrayNotification();

private Q_SLOTS:
    void onClicked();
    void onTimeout();
};


#endif // LIBNOTIFYNOTIFICATIONS_H
