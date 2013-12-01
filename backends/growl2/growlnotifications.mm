#include "growlnotifications.h"
#include <Growl/Growl.h>
#include <QDebug>
#include <QBuffer>

#include "desktopnotification_p.h"

#import <Cocoa/Cocoa.h>
#import <Growl/Growl.h>

class GrowlNotificationsBackend;
class GrowlDesktopNotification : DesktopNotification {

    friend class GrowlNotificationsBackend;
    Q_DECLARE_PRIVATE(DesktopNotification)

protected:
    GrowlDesktopNotification(DesktopNotificationManager* manager, QObject* parent = 0)
        :DesktopNotification(new DesktopNotificationPrivate, manager, parent) {

        selfPtr = [NSNumber numberWithLong:(long)this];
        idStr = [selfPtr stringValue];
    }
    ~GrowlDesktopNotification() {
        Q_D(const DesktopNotification);
        GrowlNotificationsBackend* backend = (GrowlNotificationsBackend*)(d->backend);
        if(d->visible && backend)
            backend->hide(this);
        [idStr release];
        [selfPtr release];
    }

private:
    NSString* idStr;
    NSNumber* selfPtr;
};


NSData* NSDataFromQImage(const QImage & image);

static inline NSString* fromQString(const QString &string)
{
    const QByteArray utf8 = string.toUtf8();
    const char* cString = utf8.constData();
    return [[NSString alloc] initWithUTF8String:cString];
}

@interface NaughtyGrowlDelegate : NSObject <GrowlApplicationBridgeDelegate>
GrowlNotificationsBackend* parent;
@end

@implementation NaughtyGrowlDelegate

- (id) initWithParent:(GrowlNotificationsBackend*) theParent{
    if ((self = [super init])) {
        parent = theParent;
    }
    return self;
}

- (NSDictionary *) registrationDictionaryForGrowl {


    NSArray *notifications = [NSArray arrayWithObject:@"Default"];
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                            [NSNumber numberWithInt:1], GROWL_TICKET_VERSION,
                            notifications, GROWL_NOTIFICATIONS_ALL,
                            notifications, GROWL_NOTIFICATIONS_DEFAULT,
                            fromQString(parent->theManager()->applicationName()), GROWL_APP_NAME,
                            NSDataFromQImage(parent->theManager()->defaultIcon()), GROWL_APP_ICON_DATA,
                            nil];
    return dict;
}


- (void)growlNotificationWasClicked:(id)contextId {
    parent->clicked([(NSNumber *)contextId longValue]);
}

- (void)growlNotificationTimedOut:(id)contextId {
    parent->timeout([(NSNumber *)contextId longValue]);
}


@end




QString GrowlNotificationsBackendFactory::name() const {
    return "growl";
}

DesktopNotificationManager::BackendCapabilities GrowlNotificationsBackendFactory::capabilities() const {
    return DesktopNotificationManager::IconNotificationCap
           | DesktopNotificationManager::TimeoutNotificationCap
           | DesktopNotificationManager::PriorizableNotificationCap;
}

AbstractDesktopNotificationBackend* GrowlNotificationsBackendFactory::backend(DesktopNotificationManager* manager) const {
    NSString* path = @"Growl.framework";
    NSLog(@"path: %@", path);
    NSBundle *growlFramework = [NSBundle bundleWithPath:path];
    if(![growlFramework load]) {
        NSLog(@"Loading growl failed: %@", path);
        return 0;
    }
    return new GrowlNotificationsBackend(manager);
}




GrowlNotificationsBackend::GrowlNotificationsBackend(DesktopNotificationManager* manager)
    :AbstractDesktopNotificationBackend(manager),
      m_delegate(0)
{

}

GrowlNotificationsBackend::~GrowlNotificationsBackend() {
    [(NaughtyGrowlDelegate*)m_delegate release];
}

QString GrowlNotificationsBackend::name() const {
    return "growl";
}

DesktopNotification* GrowlNotificationsBackend::createNewNotification(DesktopNotificationManager* manager, QObject* parent){
    if(!m_delegate) {
        Class GAB = NSClassFromString(@"GrowlApplicationBridge");
        if(![GAB respondsToSelector:@selector(setGrowlDelegate:)])
            return 0;
         m_delegate = [[NaughtyGrowlDelegate alloc] initWithParent:this];
         [GAB performSelector:@selector(setGrowlDelegate:) withObject:(NaughtyGrowlDelegate*)m_delegate];
    }
    return new GrowlDesktopNotification(manager, parent);
}

void GrowlNotificationsBackend::show(DesktopNotification* n) {
    GrowlDesktopNotification* notification = static_cast<GrowlDesktopNotification*>(n);

    Class GAB = NSClassFromString(@"GrowlApplicationBridge");
    if([GAB respondsToSelector:@selector(notifyWithTitle:description:notificationName:iconData:priority:isSticky:clickContext:identifier:)]) {

        QPointer<DesktopNotification> ptr(notification);
        if(!m_notifications.contains(ptr))
            m_notifications.append(ptr);

        [GAB notifyWithTitle:fromQString(notification->title())
                                    description:fromQString(notification->message())
                               notificationName:@"Default"
                                       iconData:NSDataFromQImage(notification->hint(DesktopNotification::NH_Icon).value<QImage>())
                                       priority:notification->hint(DesktopNotification::NH_Priority, 0).toInt()
                                       isSticky:notification->hint(DesktopNotification::NH_Timeout, 0).toInt() == -1 ? YES : NO
                                   clickContext:notification->selfPtr
                                   identifier:notification->idStr];

        static_cast<GrowlDesktopNotification*>(notification)->d_ptr->visible = true;
    }
}

void GrowlNotificationsBackend::hide(DesktopNotification* notification) {
    m_notifications.removeOne(QPointer<DesktopNotification>(notification));
    static_cast<GrowlDesktopNotification*>(notification)->d_ptr->visible = false;
}

DesktopNotificationManager::BackendCapabilities GrowlNotificationsBackend::capabilities() {
    return DesktopNotificationManager::IconNotificationCap
           | DesktopNotificationManager::PriorizableNotificationCap;
}

void GrowlNotificationsBackend::timeout(long id) {
    GrowlDesktopNotification* n = fromId(id);
    if(n == 0)
        return;
    n->hide();
    Q_EMIT n->closed(DesktopNotification::NotificationDismissed);
}

void GrowlNotificationsBackend::clicked(long id) {
    GrowlDesktopNotification* n = fromId(id);
    if(n == 0)
        return;
    n->hide();
    Q_EMIT n->clicked();
    Q_EMIT n->closed(DesktopNotification::NotificationClicked);
}

GrowlDesktopNotification *GrowlNotificationsBackend::fromId(long id) {
    Q_FOREACH(QPointer<DesktopNotification> n, m_notifications) {
        if(n.isNull()) continue;
        if([((GrowlDesktopNotification*)n.data())->selfPtr longValue] == id) {
            return ((GrowlDesktopNotification*)n.data());
        }
    }
    return 0;
}

DesktopNotificationManager* GrowlNotificationsBackend::theManager() const {
    return this->manager;
}

NSData* NSDataFromQImage(const QImage & image) {
    NSData* image_data = nil;
    if (image.isNull())
        return nil;
    QByteArray tiff_data;
    QBuffer tiff(&tiff_data);
    image.save(&tiff, "TIFF");
    image_data = [NSData dataWithBytes:tiff_data.constData() length:tiff_data.size()];
    return image_data;
}


Q_EXPORT_PLUGIN2(desktopnotification, GrowlNotificationsBackendFactory)
