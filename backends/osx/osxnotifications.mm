#include "osxnotifications.h"
#include <QDebug>
#include <QBuffer>
#include <QCoreApplication>
#include <QAction>

#include "desktopnotification_p.h"
#include <Availability.h>

#import <Cocoa/Cocoa.h>


#if __MAC_OS_X_VERSION_MAX_ALLOWED < 1080
@protocol NSUserNotificationCenterDelegate
@end
#endif


static inline NSString* fromQString(const QString &string)
{
    const QByteArray utf8 = string.toUtf8();
    const char* cString = utf8.constData();
    return [[NSString alloc] initWithUTF8String:cString];
}

@interface UserNotificationDelegate : NSObject <NSUserNotificationCenterDelegate>

@end

@implementation UserNotificationDelegate

OsXNotificationsBackend* parent;

- (id) initWithParent:(OsXNotificationsBackend*) p {
  if ((self = [super init])) {
      parent = p;
  }
  return self;
}

- (BOOL) userNotificationCenter:(id)center shouldPresentNotification:(id) notification {
    Q_UNUSED(center)
    Q_UNUSED(notification)

    return YES;
}

- (void)userNotificationCenter:(id)center didActivateNotification:(id)notification {
    Q_UNUSED(center)

    parent->clicked((DesktopNotification*)([[[notification userInfo] objectForKey:@"__ptr"] longValue]),
        (long)([notification activationType]) == 2); // Button clicked ?
}

@end

class OsXDesktopNotification: public DesktopNotification {

    friend class OsXNotificationsBackend;
    Q_DECLARE_PRIVATE(DesktopNotification)

protected:
    OsXDesktopNotification(DesktopNotificationManager* manager, id h, QObject* parent = 0)
        :DesktopNotification(new DesktopNotificationPrivate, manager, parent),
         handle(h){
    }
    ~OsXDesktopNotification() {
        hide();
    }
private:
    id handle;
};

QString OsXNotificationsBackendFactory::name() const {
    return "osx";
}

DesktopNotificationManager::BackendCapabilities OsXNotificationsBackendFactory::capabilities() const {
    return DesktopNotificationManager::UpdatableNotificationCap
            | DesktopNotificationManager::SoundNotificationCap
            | DesktopNotificationManager::ActionsNotificationCap;
}

AbstractDesktopNotificationBackend* OsXNotificationsBackendFactory::backend(DesktopNotificationManager* manager) const {
    return new OsXNotificationsBackend(manager);
}




OsXNotificationsBackend::OsXNotificationsBackend(DesktopNotificationManager* manager)
    :AbstractDesktopNotificationBackend(manager), m_delegate(0)
{

}

OsXNotificationsBackend::~OsXNotificationsBackend() {
    [(id)m_delegate release];
}

QString OsXNotificationsBackend::name() const {
    return "osx";
}

DesktopNotification* OsXNotificationsBackend::createNewNotification(DesktopNotificationManager* manager, QObject* parent){
    Class UserNotificationClass = NSClassFromString(@"NSUserNotification");
    if(!UserNotificationClass)
        return 0;
    id handle = [[UserNotificationClass alloc] init];
    return new OsXDesktopNotification(manager, handle, parent);
}

void OsXNotificationsBackend::show(DesktopNotification* n) {
    OsXDesktopNotification* notification = static_cast<OsXDesktopNotification*>(n);

    Class UserNotificationCenterClass = NSClassFromString(@"NSUserNotificationCenter");
    if(!UserNotificationCenterClass)
        return;
    id center = [UserNotificationCenterClass defaultUserNotificationCenter];
    if(center == nil) {
        qDebug() << "Can not get a pointer to defaultUserNotificationCenter";
        return;
    }
    if(!m_delegate) {
         m_delegate = [[UserNotificationDelegate alloc] initWithParent:this];
         [center setDelegate:(UserNotificationDelegate*)m_delegate];
    }

    [notification->handle setTitle:fromQString(notification->title())];
    [notification->handle setInformativeText:fromQString(notification->message())];

    if(!notification->actions().isEmpty()) {
        QAction* act = notification->actions().first();
        [notification->handle setActionButtonTitle:fromQString(act->text())];
        [notification->handle setHasActionButton:YES];
    }

    QString sound = notification->hint(DesktopNotification::NH_SoundName).toString();
    if (sound.isEmpty() || sound != QLatin1String("DefaultSoundName")) {
        sound = notification->hint(DesktopNotification::NH_SoundFile).toString();
    }
    if (!sound.isEmpty()) {
        [notification->handle setSoundName:fromQString(sound)];
    }

    // put the pointer to the actual notification in the 'userInfo' dict, as a NSumber
    NSDictionary *dict = [[NSDictionary alloc] initWithObjectsAndKeys:[NSNumber numberWithLong:(long)notification], @"__ptr", nil];
    [notification->handle setUserInfo:dict];
    [center deliverNotification:notification->handle];

    QPointer<DesktopNotification> ptr(notification);
    if(!m_notifications.contains(ptr))
        m_notifications.append(ptr);
    notification->d_ptr->visible = true;
}

void OsXNotificationsBackend::hide(DesktopNotification* notification) {
    m_notifications.removeOne(QPointer<DesktopNotification>(notification));
    static_cast<OsXDesktopNotification*>(notification)->d_ptr->visible = false;
}

DesktopNotificationManager::BackendCapabilities OsXNotificationsBackend::capabilities() {
    return DesktopNotificationManager::UpdatableNotificationCap
            | DesktopNotificationManager::SoundNotificationCap
            | DesktopNotificationManager::ActionsNotificationCap;
}

void OsXNotificationsBackend::clicked(DesktopNotification* n, bool actionButtonClicked) {
    if(QPointer<DesktopNotification>(n).isNull()) {
        return;
    }
    OsXDesktopNotification* notification(static_cast<OsXDesktopNotification*>(n));
    hide(notification);

    if(actionButtonClicked && !notification->actions().isEmpty()) {
            QAction* act = notification->actions().first();
            act->trigger();
    }
    else {
        notification->was_clicked();
    }
}

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2(desktopnotification, OsXNotificationsBackendFactory)
#endif
