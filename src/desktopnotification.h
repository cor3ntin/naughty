#ifndef DESKTOPNOTIFICATION_H
#define DESKTOPNOTIFICATION_H

#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QVariant>

class QAction;
class DesktopNotificationPrivate;
class DesktopNotificationManager;
class DesktopNotificationManagerPrivate;

class AbstractDesktopNotificationBackend;
class DesktopNotification : public QObject
{
    Q_OBJECT

public:
    enum NotificationHint {
        NH_ActionIcon,
        NH_Category,
        NH_DesktopFile,
        NH_SoundFile,
        NH_SoundName,
        NH_NotificationName,
        NH_Timeout,
        NH_Priority,

        NH_Title, //internal
        NH_Message, //internal
        NH_Icon, //internal
        NH_IconName //internal
    };

    enum NotificationPriority {
        LowPriority = -1,
        NormalPriority = 0,
        UrgentPriority = 1
    };

    enum NotificationClosedReason {
        NotificationClicked,
        NotificationExpired,
        NotificationDissmised
    };
    typedef QMap<NotificationHint, QVariant> HintMap;

    Q_ENUMS(NotificationPriority NotificationHint NotificationClosedReason)

    Q_PROPERTY(QImage icon READ icon WRITE setIcon)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)


    virtual ~DesktopNotification(){}

    void setIcon(const QImage & icon);
    QImage icon() const;

    void setMessage(const QString & message);
    QString message() const;

    void setTitle(const QString & title);
    QString title() const;

    void addAction(QAction* action);
    void removeAction(QAction* action);
    QList<QAction*> actions() const;

    void setHint(NotificationHint hint, const QVariant & value);
    HintMap hints() const;
    QVariant hint(NotificationHint, const QVariant & defaultValue = QVariant());

    bool isActive() const;

    AbstractDesktopNotificationBackend* backend() const;


Q_SIGNALS:
    void clicked();
    void closed(NotificationClosedReason reason);

public Q_SLOTS:
    void show();
    void hide();

private:
    void update();

    friend class DesktopNotificationManager;
    friend class AbstractDesktopNotificationBackend;

    Q_DECLARE_PRIVATE(DesktopNotification)

protected:
    DesktopNotificationPrivate* const d_ptr;
    DesktopNotification(DesktopNotificationManager* manager, QObject* parent = 0);
    DesktopNotification(DesktopNotificationPrivate* d, DesktopNotificationManager* manager,QObject* parent = 0);
};


class DesktopNotificationManager : public QObject {
    Q_OBJECT
public:
    enum BackendCapability {
        ActionsNotificationCap,
        ApplicationTitleNotificationCap,
        ClickableLinkNotificationCap,
        IconNotificationCap,
        PriorizableNotificationCap,
        RichTextNotificationCap,
        TimeoutNotificationCap,
        UpdatableNotificationCap,
        SoundNotificationCap
    };
    Q_DECLARE_FLAGS(BackendCapabilities, BackendCapability)

    DesktopNotificationManager(QObject* parent = 0);
    ~DesktopNotificationManager();

    DesktopNotification* createNotification(QObject* parent = 0, const QString & title = QString(),
                                            const QString & message = QString());

    static QStringList availableBackends();
    bool setBackend(const QString & backendName);
    BackendCapabilities capabilities() const;
    QString backendName() const;

    void setDefaultIcon(const QIcon & icon);
    void setApplicationName(const QString & name);
private:
    friend class DesktopNotification;

    DesktopNotificationManagerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(DesktopNotificationManager)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DesktopNotificationManager::BackendCapabilities)

#endif // DESKTOPNOTIFICATION_H