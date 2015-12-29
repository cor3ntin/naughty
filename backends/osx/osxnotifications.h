#ifndef OSXNOTIFICATIONS_H
#define OSXNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include <QtPlugin>
#include <QMap>

class OsXNotificationsBackendFactory :  public QObject, public AbstractDesktopNotificationBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(AbstractDesktopNotificationBackendFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "AbstractDesktopNotificationBackendFactory/1.0")
#endif
public:
    QString name() const;
    DesktopNotificationManager::BackendCapabilities capabilities() const;
    AbstractDesktopNotificationBackend* backend(DesktopNotificationManager *manager) const;
};

class OsXDesktopNotification;
class OsXNotificationsBackend : public AbstractDesktopNotificationBackend
{
public:
    OsXNotificationsBackend(DesktopNotificationManager* manager);
    ~OsXNotificationsBackend();
    QString name() const;

    DesktopNotification* createNewNotification(DesktopNotificationManager* thManager, QObject* parent = 0);

    void show(DesktopNotification*);
    void hide(DesktopNotification*);
    DesktopNotificationManager::BackendCapabilities capabilities();

public: //actually private
    void clicked(DesktopNotification* notification, bool actionButtonClicked);

private:
    void* m_delegate;
    friend class OsXDesktopNotification;
};

#endif // OSXNOTIFICATIONS_H
