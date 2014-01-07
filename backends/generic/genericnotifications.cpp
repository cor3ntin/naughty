#include "genericnotifications.h"
#include "genericnotificationbox.h"
#include "desktopnotification_p.h"
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>


static const int DEFAULT_TIMEOUT = 5000;
static const int DEFAULT_TIMEOUT_FADE = 500;
#ifdef Q_OS_MAC
static const Qt::Corner DEFAULT_CORNER = Qt::TopRightCorner;
#else
static const Qt::Corner DEFAULT_CORNER = Qt::BottomRightCorner;
#endif

static const DesktopNotificationManager::BackendCapabilities WIDGET_BACKEND_CAPS =
        DesktopNotificationManager::ApplicationTitleNotificationCap |
        DesktopNotificationManager::TimeoutNotificationCap |
        DesktopNotificationManager::IconNotificationCap |
        DesktopNotificationManager::PriorizableNotificationCap |
        DesktopNotificationManager::RichTextNotificationCap |
        DesktopNotificationManager::ClickableLinkNotificationCap;


QString GenericNotificationBackendFactory::name() const {
    return "generic";
}

DesktopNotificationManager::BackendCapabilities GenericNotificationBackendFactory::capabilities() const {
    return WIDGET_BACKEND_CAPS;
}


AbstractDesktopNotificationBackend* GenericNotificationBackendFactory::backend(DesktopNotificationManager* manager) const {
    return new GenericNotificationBackend(manager);
}

class GenericNotificationPrivate : public DesktopNotificationPrivate {
public:
    GenericNotificationPrivate() {}
};


GenericNotification::GenericNotification(DesktopNotificationManager* manager, QObject* parent)
    :DesktopNotification(manager, parent), box(0), timer(0){

}

GenericNotification::~GenericNotification() {
    hide();
}
void GenericNotification::doShow() {
    if(!box) {
        box = new GenericNotificationBox(0);
        connect(box, SIGNAL(clicked()), this, SLOT(was_clicked()));
        connect(box, SIGNAL(dismissed()), this, SLOT(was_dismissed()));
    }
    int timeout = hint(NH_Timeout, DEFAULT_TIMEOUT).toInt();

    if(timer)
        timer->stop();

    if(timeout > 0) {
        if(!timer) {
            timer = new QTimer(this);
        }
        disconnect(timer, 0, this, 0);
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()), Qt::UniqueConnection);
        timer->setSingleShot(true);
        timer->start(timeout);
    }

    if(!icon().isNull())
        box->setIcon(QPixmap::fromImage(icon()));
    else if(!manager()->defaultIcon().isNull())
         box->setIcon(QPixmap::fromImage(manager()->defaultIcon()));

   QString bc = hint(NH_GenericBackgroundColor).toString();
   if(!bc.isNull())
       box->setBackgroundColor(QColor(bc));

   QString style = hint(NH_GenericStyleSheet).toString();
   if(!bc.isNull())
       box->setStyleSheet(style);


    box->setApplicationName(manager()->applicationName());

    box->setTitle(title());
    box->setMessage(message());
    box->show(static_cast<Qt::Corner>(hint(NH_GenericScreenCorner, DEFAULT_CORNER).toInt()));

    Q_D(GenericNotification);
    d->visible = true;
}
void GenericNotification::onTimeout() {
    if(!box)
        return;
    QPropertyAnimation *animation = new QPropertyAnimation(box, "windowOpacity");
    animation->setDuration(DEFAULT_TIMEOUT_FADE);
    animation->setStartValue(box->windowOpacity());
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setEndValue(0);
    connect(animation, SIGNAL(finished()), this, SLOT(hide()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GenericNotification::doHide() {
    Q_D(GenericNotification);
    d->visible = false;

    if(timer)
        timer->stop();

    if(box) {
        box->deleteLater();
        box = 0;
    }
}

GenericNotificationBackend::GenericNotificationBackend(DesktopNotificationManager* manager)
    :AbstractDesktopNotificationBackend(manager) {

}

QString GenericNotificationBackend::name() const {
    return "generic";
}

DesktopNotificationManager::BackendCapabilities GenericNotificationBackend::capabilities() {
     return WIDGET_BACKEND_CAPS;
}

DesktopNotification* GenericNotificationBackend::createNewNotification(DesktopNotificationManager* manager, QObject* parent) {
    return new GenericNotification(manager, parent);
}

void GenericNotificationBackend::show(DesktopNotification* n) {
    m_notifications.append(n);

    if(!m_current.isNull()) {
        m_pending.enqueue(n->hint(DesktopNotification::NH_Priority, DesktopNotification::NormalPriority).toInt(), n);
        return;
    }
    if(!QPointer<DesktopNotification>(n))
        return;

    m_current = n;
    GenericNotification * notification = static_cast<GenericNotification*>(n);
    notification->doShow();
}

void GenericNotificationBackend::hide(DesktopNotification* n) {
    if(!QPointer<DesktopNotification>(n))
        return;
    GenericNotification* notification = static_cast<GenericNotification*>(n);
    notification->doHide();
    m_notifications.removeOne(QPointer<DesktopNotification>(notification));

    m_current = 0;

    if(m_pending.count() != 0) {
        show(m_pending.dequeue());
    }
}



Q_EXPORT_PLUGIN2(desktopnotification, GenericNotificationBackendFactory)

#include "moc_genericnotifications.cpp"
