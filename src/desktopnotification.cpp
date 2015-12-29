#include "desktopnotification.h"
#include "desktopnotification_p.h"
#include "desktopnotificationbackend.h"
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

AbstractDesktopNotificationBackend::AbstractDesktopNotificationBackend(DesktopNotificationManager* manager)
    :manager(manager){

}

AbstractDesktopNotificationBackend::~AbstractDesktopNotificationBackend() {
    Q_FOREACH(QPointer<DesktopNotification> notif, m_notifications){
        if(notif)
            notif->d_ptr->backend = 0;
    }
}

void AbstractDesktopNotificationBackend::hideAll() {
    Q_FOREACH(QPointer<DesktopNotification> notif, m_notifications){
        if(notif)
            notif->hide();
    }
}


class DesktopNotificationBackendFactoryLoader {
public:
    DesktopNotificationBackendFactoryLoader();
    QList<AbstractDesktopNotificationBackendFactory*> availableBackendFactories();
private:
    bool init;
    QList<AbstractDesktopNotificationBackendFactory*> backendFactories;
    void load();
};

DesktopNotificationBackendFactoryLoader::DesktopNotificationBackendFactoryLoader()
    :init(false)
{
}

QList<AbstractDesktopNotificationBackendFactory*>
DesktopNotificationBackendFactoryLoader::availableBackendFactories()
{
    if(!init)
        load();
    init = true;
    return backendFactories;
}

void DesktopNotificationBackendFactoryLoader::load()
{
    Q_FOREACH(const QString & path, qApp->libraryPaths()) {
        QDir pluginsDir(path+"/desktopnotifications");
        Q_FOREACH(const QString & fileName, pluginsDir.entryList(QDir::Files)) {
           qWarning() << fileName;
           QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
           if(pluginLoader.load()){
               AbstractDesktopNotificationBackendFactory* instance = qobject_cast<AbstractDesktopNotificationBackendFactory*>(pluginLoader.instance());
               if(!instance){
                   qWarning() << "Not a valid desktop notification plugin" << fileName ;
                   pluginLoader.unload();
                   continue;
               }
               backendFactories.append(instance);
           }
           else {
                qWarning() << "Not a valid plugin" << fileName << pluginLoader.errorString();
           }
        }
    }
}

Q_GLOBAL_STATIC(DesktopNotificationBackendFactoryLoader, desktopNotificationBackendLoader)

DesktopNotificationManager::DesktopNotificationManager(QObject* parent)
    :QObject(parent),d_ptr(new DesktopNotificationManagerPrivate)
{
    d_ptr->backend = 0;
}

DesktopNotificationManager::~DesktopNotificationManager() {
    Q_D(DesktopNotificationManager);
    if(d->backend) {
        d->backend->hideAll();
        delete d->backend;
    }
    delete d_ptr;
}


DesktopNotification* DesktopNotificationManager::createNotification(QObject *parent, const QString & title,
                                        const QString & message){
    Q_D(DesktopNotificationManager);
    if(!d->backend && !createDefaultBackend())
        return 0;

    DesktopNotification* notification = d->backend->createNewNotification(this, parent);
    if(!notification)
        return 0;
    notification->d_ptr->backend = d->backend;
    notification->d_ptr->visible = false;

    notification->setTitle(title);
    notification->setMessage(message);
    return notification;
}

QStringList DesktopNotificationManager::availableBackends()
{
    QStringList names;
    Q_FOREACH(const AbstractDesktopNotificationBackendFactory* f,
              desktopNotificationBackendLoader()->availableBackendFactories()) {
        names.append(f->name());
    }
    return names;
}

bool  DesktopNotificationManager::createDefaultBackend() {
    QStringList names = availableBackends();

#ifdef Q_OS_LINUX
    if(names.contains("libnotify") && setBackend("libnotify"))
        return true;
#endif

#ifdef Q_OS_MAC
    if(QSysInfo::MacintoshVersion >= QSysInfo::MV_10_8 && names.contains("osx")
           && setBackend("osx"))
        return true;
    // growl only works for 10.6+
    if(QSysInfo::MacintoshVersion >= QSysInfo::MV_10_6 && names.contains("growl")
       && setBackend("growl"))
        return true;
#endif
    if(names.contains("generic") && setBackend("generic"))
        return true;

    return false;
}

bool DesktopNotificationManager::setBackend(const QString & backendName)
{
    Q_D(DesktopNotificationManager);
    if(d->backend && d->backend->name() == backendName)
            return true;
    const AbstractDesktopNotificationBackendFactory* factory = 0;
    Q_FOREACH(const AbstractDesktopNotificationBackendFactory* f,
              desktopNotificationBackendLoader()->availableBackendFactories()) {
        if(f->name() == backendName) {
            factory = f;
            break;
        }
    }

    if(!factory)
        return false;
    qDebug() << "Creating backend with factory:" << factory->name();
    AbstractDesktopNotificationBackend* backend = factory->backend(this);
    if(!backend)
        return false;
    delete d->backend;
    d->backend = backend;
    return true;
}

DesktopNotificationManager::BackendCapabilities  DesktopNotificationManager::capabilities() const
{
    Q_D(const DesktopNotificationManager);
    if(!d->backend)
        return (BackendCapabilities)0;
    return d->backend->capabilities();
}

QString DesktopNotificationManager::backendName() const
{
    Q_D(const DesktopNotificationManager);
    if(!d->backend)
        return 0;
    return d->backend->name();
}


QImage DesktopNotificationManager::defaultIcon() const {
    Q_D(const DesktopNotificationManager);
    return d->hints.value(DesktopNotification::NH_ApplicationIcon).value<QImage>();
}

void DesktopNotificationManager::setDefaultIcon(const QImage & icon) {
    setDefaultHint(DesktopNotification::NH_ApplicationIcon, icon);
}

QString DesktopNotificationManager::applicationName() const {
    Q_D(const DesktopNotificationManager);
    QString name = d->hints.value(DesktopNotification::NH_ApplicationName).toString();
    if(!name.isEmpty())
        return name;
    return QCoreApplication::applicationName();
}

void DesktopNotificationManager::setApplicationName(const QString & name) {
    setDefaultHint(DesktopNotification::NH_ApplicationName, name);
}


void DesktopNotificationManager::setDefaultHint(DesktopNotification::NotificationHint hint, const QVariant & value) {
    Q_D(DesktopNotificationManager);
    d->hints.insert(hint, value);
}

QVariant DesktopNotificationManager::defaultHint(DesktopNotification::NotificationHint hint) const {
    Q_D(const DesktopNotificationManager);
    return d->hints.value(hint);
}


DesktopNotification::DesktopNotification(DesktopNotificationManager* manager, QObject* parent)
    :QObject(parent), d_ptr(new DesktopNotificationPrivate){

    d_ptr->manager = manager;
}

DesktopNotification::DesktopNotification(DesktopNotificationPrivate* d, DesktopNotificationManager* manager, QObject* parent)
    :QObject(parent), d_ptr(d){

    d_ptr->manager = manager;
}

void DesktopNotification::setIcon(const QImage & icon)
{
    Q_D(DesktopNotification);
    d->hints.insert(NH_Icon, icon);
    update();
}

QImage DesktopNotification::icon() const
{
    Q_D(const DesktopNotification);
    return d->hints.value(NH_Icon).value<QImage>();
}

void DesktopNotification::setMessage(const QString & message)
{
    Q_D(DesktopNotification);
    d->hints.insert(NH_Message, message);
    update();
}

QString DesktopNotification::message() const
{
    Q_D(const DesktopNotification);
    return d->hints.value(NH_Message).toString();
}

void DesktopNotification::setTitle(const QString & title)
{
    Q_D(DesktopNotification);
    d->hints.insert(NH_Title, title);
}

QString DesktopNotification::title() const
{
    Q_D(const DesktopNotification);
    return d->hints.value(NH_Title).toString();
}

void DesktopNotification::addAction(QAction* action)
{
    Q_D(DesktopNotification);
    if(!d->actions.contains(action))
        d->actions.append(action);
    update();
}

void DesktopNotification::removeAction(QAction* action)
{
    Q_D(DesktopNotification);
    d->actions.removeOne(action);
    update();
}

QList<QAction*> DesktopNotification::actions() const
{
    Q_D(const DesktopNotification);
    return d->actions;
}

void DesktopNotification::setHint(NotificationHint hint, const QVariant & value)
{
    Q_D(DesktopNotification);
    d->hints.insert(hint, value);
    update();
}

DesktopNotification::HintMap DesktopNotification::hints() const
{
    Q_D(const DesktopNotification);
    return d->hints;
}

QVariant DesktopNotification::hint(NotificationHint hint, const QVariant &defaultValue)
{
    Q_D(DesktopNotification);
    QVariant h = d->hints.value(hint);
    if(h.isNull())
        h = manager()->defaultHint(hint);
    if(h.isNull())
        h = defaultValue;
    return h;
}

bool DesktopNotification::isActive() const
{
    Q_D(const DesktopNotification);
    return d->visible;
}


AbstractDesktopNotificationBackend* DesktopNotification::backend() const {
    Q_D(const DesktopNotification);
    return d->backend;
}

DesktopNotificationManager* DesktopNotification::manager() const {
    Q_D(const DesktopNotification);
    return d->manager;
}

void DesktopNotification::update(){
    if(isActive())
        show();
}

void DesktopNotification::show() {
    Q_D(DesktopNotification);
    if(d->backend)
        d->backend->show(this);
}

void DesktopNotification::hide() {
    Q_D(DesktopNotification);
    if(d->backend)
        d->backend->hide(this);
    Q_EMIT closed(DesktopNotification::NotificationExpired);
}

void DesktopNotification::was_dismissed() {
    Q_EMIT closed(DesktopNotification::NotificationDismissed);
}

void DesktopNotification::was_clicked() {
    Q_EMIT closed(DesktopNotification::NotificationClicked);
    Q_EMIT clicked();
}
