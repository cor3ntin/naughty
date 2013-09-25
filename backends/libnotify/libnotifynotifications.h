#ifndef LIBNOTIFYNOTIFICATIONS_H
#define LIBNOTIFYNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include <QtPlugin>

class LibnotifyNotificationBackendFactory : public QObject, public AbstractDesktopNotificationBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(AbstractDesktopNotificationBackendFactory)
public:
    QString name() const;
    DesktopNotificationManager::BackendCapabilities capabilities() const;
    AbstractDesktopNotificationBackend* backend() const;
};

class LibnotifyNotificationBackend : public AbstractDesktopNotificationBackend
{
public:
    LibnotifyNotificationBackend();
    ~LibnotifyNotificationBackend();
    QString name() const;

    DesktopNotification* createNewNotification(DesktopNotificationManager* manager, QObject* parent = 0);

    void show(DesktopNotification*);
    void hide(DesktopNotification*notification);
    DesktopNotificationManager::BackendCapabilities capabilities();

    void onNotificationClosed(DesktopNotification* notification);
    void onNotificationActionClicked(DesktopNotification* notification, char* action);

private:    
    DesktopNotificationManager::BackendCapabilities m_capabilities;
};

#endif // LIBNOTIFYNOTIFICATIONS_H
