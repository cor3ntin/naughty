#ifndef LIBNOTIFYNOTIFICATIONS_H
#define LIBNOTIFYNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include <QtPlugin>

class LibnotifyNotificationBackendFactory : public QObject, public AbstractDesktopNotificationBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(AbstractDesktopNotificationBackendFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "AbstractDesktopNotificationBackendFactory/1.0")
#endif
public:
    QString name() const;
    DesktopNotificationManager::BackendCapabilities capabilities() const;
    AbstractDesktopNotificationBackend* backend(DesktopNotificationManager* manager) const;
};

class LibnotifyNotificationBackend : public AbstractDesktopNotificationBackend
{
public:
    LibnotifyNotificationBackend(DesktopNotificationManager* manager);
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
