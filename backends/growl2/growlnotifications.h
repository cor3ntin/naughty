#ifndef GROWLNOTIFICATIONS_H
#define GROWLNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include <QtPlugin>
#include <QMap>


class GrowlNotificationsBackendFactory :  public QObject, public AbstractDesktopNotificationBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(AbstractDesktopNotificationBackendFactory)
public:
    QString name() const;
    DesktopNotificationManager::BackendCapabilities capabilities() const;
    AbstractDesktopNotificationBackend* backend() const;
};

class GrowlDesktopNotification;
class GrowlNotificationsBackend : public AbstractDesktopNotificationBackend
{
public:
    GrowlNotificationsBackend();
    ~GrowlNotificationsBackend();
    QString name() const;

    DesktopNotification* createNewNotification(DesktopNotificationManager* manager, QObject* parent = 0);

    void show(DesktopNotification*);
    void hide(DesktopNotification*);
    DesktopNotificationManager::BackendCapabilities capabilities();

public: // but, private
    void timeout(long id);
    void clicked(long id);
    DesktopNotificationManager* theManager() const;


private:
    GrowlDesktopNotification* fromId(long id);
    void* m_delegate;
    friend class GrowlDesktopNotification;
};

#endif // GROWLNOTIFICATIONS_H
