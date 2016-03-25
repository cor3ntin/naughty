#include "libnotifynotifications.h"
#include <QtPlugin>
#include <QtDebug>
#include <QCoreApplication>
#include <QAction>

#include "libnotify/notify.h"
#include "desktopnotification_p.h"



class LibnotifyDesktopNotificationPrivate : public DesktopNotificationPrivate {
public:
    LibnotifyDesktopNotificationPrivate() : handle(0) {}

    NotifyNotification* handle;
};
class LibnotifyDesktopNotification : DesktopNotification {

    friend class LibnotifyNotificationBackend;
    Q_DECLARE_PRIVATE(LibnotifyDesktopNotification)

protected:
    LibnotifyDesktopNotification(DesktopNotificationManager* manager, QObject* parent = 0)
        :DesktopNotification(new LibnotifyDesktopNotificationPrivate, manager, parent) {

    }
    NotifyNotification* handle() const {
        Q_D(const LibnotifyDesktopNotification);
        return d->handle;
    }

    void setClosed(DesktopNotification::NotificationClosedReason reason) {
        if(reason == NotificationClicked)
            Q_EMIT clicked();
        Q_EMIT closed(reason);
    }

    void setHandle(NotifyNotification* handle) {
        Q_D(LibnotifyDesktopNotification);
        d->handle = handle;
    }
};


QString LibnotifyNotificationBackendFactory::name() const
{
    return QLatin1String("libnotify");
}

DesktopNotificationManager::BackendCapabilities LibnotifyNotificationBackendFactory::capabilities() const
{
    return DesktopNotificationManager::IconNotificationCap |
            DesktopNotificationManager::PriorizableNotificationCap |
            DesktopNotificationManager::TimeoutNotificationCap |
            DesktopNotificationManager::UpdatableNotificationCap;
}

AbstractDesktopNotificationBackend* LibnotifyNotificationBackendFactory::backend(DesktopNotificationManager* manager) const
{
    return new LibnotifyNotificationBackend(manager);
}

LibnotifyNotificationBackend::LibnotifyNotificationBackend(DesktopNotificationManager* manager)
    :AbstractDesktopNotificationBackend(manager),
     m_capabilities((DesktopNotificationManager::BackendCapabilities)0)
{
    notify_init(manager->applicationName().toLocal8Bit().constData());
}

LibnotifyNotificationBackend::~LibnotifyNotificationBackend()
{
    notify_uninit();
}


QString LibnotifyNotificationBackend::name() const
{
    return QLatin1String("libnotify");
}


DesktopNotification* LibnotifyNotificationBackend::createNewNotification(DesktopNotificationManager* manager, QObject *parent) {
    return new LibnotifyDesktopNotification(manager, parent);
}

static void onNotificationClosed(NotifyNotification*, DesktopNotification* notification){
    static_cast<LibnotifyNotificationBackend*>(notification->backend())->onNotificationClosed(notification);
}

static void onNotificationActionClicked(NotifyNotification*, char* action, void* data){
    DesktopNotification* notification = static_cast<DesktopNotification*>(data);
    reinterpret_cast<LibnotifyNotificationBackend*>(notification->backend())->onNotificationActionClicked(notification, action);
}

static GdkPixbuf* pixbufFromQImage(QImage & image);
void LibnotifyNotificationBackend::show(DesktopNotification* notification)
{
    NotifyNotification* handle = static_cast<LibnotifyDesktopNotification*>(notification)->handle();
    if(!handle) {
        handle = notify_notification_new (
                qPrintable(notification->title()),
                qPrintable(notification->message()),
                0);
        static_cast<LibnotifyDesktopNotification*>(notification)->setHandle(handle);

        //Connect the close signal
        g_signal_connect (handle, "closed", G_CALLBACK (::onNotificationClosed), notification);
    }

    QImage icon = notification->hint(DesktopNotification::NH_Icon).value<QImage>();
    if(icon.isNull())
        icon = notification->manager()->defaultIcon();

    if(!icon.isNull()) {

        GdkPixbuf* pixbuf = pixbufFromQImage(icon);
        notify_notification_set_image_from_pixbuf(handle, pixbuf);
        g_object_unref(pixbuf);
    }
    else {
        QString iconName = notification->hint(DesktopNotification::NH_IconName).value<QString>();
        if(!iconName.isEmpty()) {
            notify_notification_set_hint(handle, "image-path",
                                         g_variant_new_string(iconName.toLocal8Bit().constData()));
        }
    }


    int timeout = notification->hint(DesktopNotification::NH_Timeout).toInt();
    if(timeout > 0)
        notify_notification_set_timeout(handle, timeout);

    notify_notification_clear_actions(handle);

    Q_FOREACH(QAction* action, notification->actions()) {
        int id = action->property("_libnotifyId").toInt();
        if(id == 0) {
            id = qrand();
            action->setProperty("_libnotifyId", id);
        }
        notify_notification_add_action (handle,
                                        QByteArray::number(id).constData(),
                                        qPrintable(action->text()),
                                        ::onNotificationActionClicked, notification, 0);
    }
    if(notification->actions().isEmpty()) {
        QString actName = notification->hint(DesktopNotification::NH_FallbackActionName).toString();
        if(!actName.isEmpty()) {
            notify_notification_add_action (handle,
                                            "_clicked",
                                            qPrintable(actName),
                                            ::onNotificationActionClicked, notification, 0);
        }
    }

    notify_notification_show(handle,0);

    static_cast<LibnotifyDesktopNotification*>(notification)->d_ptr->visible = true;
    QPointer<DesktopNotification> ptr(notification);
    if(!m_notifications.contains(ptr))
        m_notifications.append(ptr);

}

void LibnotifyNotificationBackend::hide(DesktopNotification* notification)
{
    NotifyNotification* handle = static_cast<LibnotifyDesktopNotification*>(notification)->handle();
    if(handle) {
        notify_notification_close(handle, 0);
    }
    static_cast<LibnotifyDesktopNotification*>(notification)->d_ptr->visible = false;
    m_notifications.removeOne(QPointer<DesktopNotification>(notification));
}

void LibnotifyNotificationBackend::onNotificationClosed(DesktopNotification* notification) {

    gint reason_int = notify_notification_get_closed_reason(static_cast<LibnotifyDesktopNotification*>(notification)->handle());

    DesktopNotification::NotificationClosedReason reason = DesktopNotification::NotificationExpired;

    switch(reason_int) { // https://developer.gnome.org/notification-spec/
        case 1: reason = DesktopNotification::NotificationExpired; break;
        case 2: reason = DesktopNotification::NotificationDismissed; break;
        case 3: reason = DesktopNotification::NotificationClicked; break;
    }
    static_cast<LibnotifyDesktopNotification*>(notification)->setClosed(reason);
}

void LibnotifyNotificationBackend::onNotificationActionClicked(DesktopNotification* notification, char* action)
{
    if(strcmp(action, "_clicked") == 0) {
        static_cast<LibnotifyDesktopNotification*>(notification)->setClosed(DesktopNotification::NotificationClicked);
        return;
    }
    int actionId = QByteArray(action).toInt();
    Q_FOREACH(QAction* action, notification->actions()) {
        int id = action->property("_libnotifyId").toInt();
        if(id == actionId) {
            action->trigger();
            static_cast<LibnotifyDesktopNotification*>(notification)->setClosed(DesktopNotification::NotificationClicked);
            break;
        }
    }
}

DesktopNotificationManager::BackendCapabilities LibnotifyNotificationBackend::capabilities()
{
    if(m_capabilities != 0)
        return m_capabilities;

    m_capabilities =
            DesktopNotificationManager::IconNotificationCap |
            DesktopNotificationManager::PriorizableNotificationCap |
            DesktopNotificationManager::TimeoutNotificationCap |
            DesktopNotificationManager::UpdatableNotificationCap;

    GList* lst = notify_get_server_caps();
    for(GList* c = lst; c != NULL; c = c->next) {
        qDebug() << (char*)c->data;
        if(strcmp((char*)c->data, "actions") == 0 ) {
            m_capabilities |= DesktopNotificationManager::ActionsNotificationCap;
        }
        else if(strcmp((char*)c->data, "sound") == 0 ) {
            m_capabilities |= DesktopNotificationManager::SoundNotificationCap;
        }
        else if(strcmp((char*)c->data, "body-hyperlinks") == 0 ) {
            m_capabilities |= DesktopNotificationManager::ClickableLinkNotificationCap;
        }
        else if(strcmp((char*)c->data, "body-markup") == 0 ) {
            m_capabilities |= DesktopNotificationManager::RichTextNotificationCap;
        }
    }

    g_list_foreach(lst, (GFunc)g_free, NULL);
    g_list_free(lst);

    return m_capabilities;
}

GdkPixbuf* pixbufFromQImage(QImage & image) {

    if (image.isNull())
        return 0;
    image = image.convertToFormat(QImage::Format_ARGB32).rgbSwapped();


    GdkPixbuf *pixbuf2 = gdk_pixbuf_new_from_data(image.constBits(), GDK_COLORSPACE_RGB,
                                                 image.hasAlphaChannel(), 8,
                                                 image.width(), image.height(),
                                                 image.bytesPerLine(), 0, 0);

    return pixbuf2;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(desktopnotification, LibnotifyNotificationBackendFactory)
#endif
