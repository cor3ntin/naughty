#ifndef DESKTOPNOTIFICATIONBACKEND_H
#define DESKTOPNOTIFICATIONBACKEND_H

#include "desktopnotification.h"
#include <QPointer>

class AbstractDesktopNotificationBackend {
public:
    AbstractDesktopNotificationBackend(DesktopNotificationManager* manager);
    virtual ~AbstractDesktopNotificationBackend();

    virtual QString name() const = 0;
    virtual DesktopNotificationManager::BackendCapabilities capabilities() = 0;

    virtual DesktopNotification* createNewNotification(DesktopNotificationManager* manager, QObject* parent = 0) = 0;

    virtual void show(DesktopNotification*) = 0;
    virtual void hide(DesktopNotification*) = 0;
    void hideAll();

protected:
    DesktopNotificationManager* manager;
    QList<QPointer<DesktopNotification> > m_notifications;
};

class AbstractDesktopNotificationBackendFactory {

public:
    virtual ~AbstractDesktopNotificationBackendFactory(){}
    virtual QString name() const = 0;
    virtual DesktopNotificationManager::BackendCapabilities capabilities() const = 0;
    virtual AbstractDesktopNotificationBackend* backend(DesktopNotificationManager* manager) const = 0;
};

Q_DECLARE_INTERFACE(AbstractDesktopNotificationBackendFactory, "AbstractDesktopNotificationBackendFactory/1.0")

#endif // ABSTRACTDESKTOPNOTIFICATIONBACKEND_H
