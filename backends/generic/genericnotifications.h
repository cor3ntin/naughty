#ifndef WIDGETNOTIFICATIONS_H
#define WIDGETNOTIFICATIONS_H

#include "desktopnotificationbackend.h"
#include "desktopnotification_p.h"
#include <QtPlugin>
#include "priorityqueue.h"

class GenericNotificationBackendFactory : public QObject, public AbstractDesktopNotificationBackendFactory
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

class GenericNotificationBackend : public AbstractDesktopNotificationBackend
{
public:
    GenericNotificationBackend(DesktopNotificationManager* manager);
    QString name() const;

    DesktopNotification* createNewNotification(DesktopNotificationManager* manager, QObject* parent = 0);

    void show(DesktopNotification*);
    void hide(DesktopNotification*notification);
    DesktopNotificationManager::BackendCapabilities capabilities();

private:
    QPointer<DesktopNotification> m_current;
    PriorityQueue<int, QPointer<DesktopNotification> > m_pending;
};

class GenericNotificationBox;
class GenericNotificationPrivate;
class GenericNotification : public DesktopNotification {

    Q_OBJECT
public:
    friend class GenericNotificationBackend;
    Q_DECLARE_PRIVATE(GenericNotification)

    GenericNotification(DesktopNotificationManager* manager, QObject* parent);
    ~GenericNotification();

    void doShow();
    void doHide();

public Q_SLOTS:
    void onTimeout();

public:
    GenericNotificationBox* box;
private:
    QTimer* timer;
};

#endif // WIDGETNOTIFICATIONS_H
