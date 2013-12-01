#include "genericnotificationbox.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QStyle>
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

static const int NOTIFICATION_WIDTH  = 300;
static const int NOTIFICATION_HEIGHT = 90;
static const int NOTIFICATION_IMAGE_SIZE = 48;
static const float NOTIFICATION_OPACITY = 0.95;
static const float NOTIFICATION_TITLE_FONT_SIZE = 0.9;

static const QString NOTIFICATION_STYLE=
"#windowbar {color:#696969;font-size:8em }"
"#title {color:white; font-size:10em; font-weight:bold;}"
"#message {color:white}";


static const char* NOTIFICATION_WINDOWBAR_COLOR  = "#f5f5f5";
static const char* NOTIFICATION_DEFAULT_BG_COLOR = "#000000";


static const char * close_xpm[] = {
"8 8 2 1",
" 	c None",
".	c #696969",
" .    . ",
"...  ...",
" ...... ",
"  ....  ",
"  ....  ",
" ...... ",
"...  ...",
" .    . "};



//make sure events sended to the label are not forwarded
//to the box : we don't want to close the notification when a link is opened
class Label : public QLabel {

protected:
    void mousePressEvent(QMouseEvent *ev) {
        QLabel::mousePressEvent(ev);
        ev->accept();
    }

    void mouseReleaseEvent(QMouseEvent *ev) {
        QLabel::mouseReleaseEvent(ev);
        ev->accept();
    }
};


GenericNotificationButton::GenericNotificationButton(const QPixmap & pixmap, QWidget *parent)
    :QLabel(parent)
{
    this->setPixmap(pixmap);
    this->installEventFilter(this);
}

bool GenericNotificationButton::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::MouseButtonPress) {
        Q_EMIT clicked();
        event->accept();
    }
    return QLabel::eventFilter(obj, event);
}

GenericNotificationBox::GenericNotificationBox(QWidget *parent)
    : QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMaximumSize(QSize(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));
    setMinimumSize(QSize(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));

    QVBoxLayout* topLayout = new QVBoxLayout;
    topLayout->setContentsMargins(5, 0, 2, 5);
    //Top layout
    QHBoxLayout* titleBox = new QHBoxLayout;
    m_applicationName = new QLabel;
    m_applicationName->setObjectName("titlebar");

    titleBox->setContentsMargins(2, 3, 5, 2);
    titleBox->addWidget(m_applicationName,0);

    QLabel* closeButton = new GenericNotificationButton(QPixmap(close_xpm));
    closeButton->setMaximumSize(10, 10);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(dismissed()));

    titleBox->addWidget(closeButton, 0, Qt::AlignRight);

    topLayout->addLayout(titleBox);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setMargin(0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    m_icon = new QLabel;
    m_icon->setMaximumSize(NOTIFICATION_IMAGE_SIZE,NOTIFICATION_IMAGE_SIZE);
    layout->addWidget(m_icon);

    QVBoxLayout* txtLayout = new QVBoxLayout;
    m_title = new QLabel;
    m_title->setObjectName("title");
    m_title->setWordWrap(true);
    txtLayout->setMargin(0);
    txtLayout->setContentsMargins(0, 0, 0, 0);
    txtLayout->addWidget(m_title);

    m_message = new Label;
    m_message->setObjectName("message");
    m_message->setOpenExternalLinks(true);
    m_message->setWordWrap(true);
    txtLayout->addWidget(m_message, 1, Qt::AlignTop);
    layout->addLayout(txtLayout);

    topLayout->addLayout(layout);

    //topLayout->addLayout(buttonLayout);
    topLayout->setSizeConstraint(QLayout::SetMaximumSize);

    setLayout(topLayout);
    setStyleSheet(NOTIFICATION_STYLE);

    setAttribute(Qt::WA_X11NetWmWindowTypeNotification);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(NOTIFICATION_OPACITY);

    QPalette p = palette();
    p.setColor(QPalette::Window, NOTIFICATION_WINDOWBAR_COLOR);
    setPalette(p);

    setBackgroundColor(NOTIFICATION_DEFAULT_BG_COLOR);
}

GenericNotificationBox::~GenericNotificationBox()
{

}

QSize GenericNotificationBox::sizeHint () const {
    return QSize(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT);
}

void GenericNotificationBox::show(Qt::Corner corner) {

    QDesktopWidget w;
    int primary_screen = w.primaryScreen();
    QRect screen_size = w.availableGeometry(primary_screen);

    switch(corner) {
        case Qt::TopLeftCorner    : move(10, 10); break;
        case Qt::TopRightCorner   : move((screen_size.width() - width()) - 10, 10); break;
        case Qt::BottomLeftCorner : move(10, (screen_size.height() - height()) - 15); break;
        case Qt::BottomRightCorner: move((screen_size.width() - width()) - 10, (screen_size.height() - height()) - 15); break;
    }



    QWidget::show();
}


void GenericNotificationBox::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);

    QPainterPath rounded_rect;
    rounded_rect.addRoundRect(0, 0, width(),height(), 20);
    painter.setClipPath(rounded_rect);

    painter.fillPath(rounded_rect, QBrush(m_backgroundColor));
    painter.fillRect(0, 0, width(), 18, palette().color(QPalette::Window));
    painter.setPen(Qt::lightGray);
    painter.drawLine(0,18, width(), 18);

    QPen pen(Qt::gray);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPath(rounded_rect);

}


void GenericNotificationBox::mousePressEvent(QMouseEvent *){
    close();
    Q_EMIT clicked();
}

void GenericNotificationBox::setIcon(const QPixmap & icon) {
    m_icon->setPixmap(icon.scaled(NOTIFICATION_IMAGE_SIZE,
                                  NOTIFICATION_IMAGE_SIZE,
                                  Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void GenericNotificationBox::setApplicationName(const QString & appName) {
    m_applicationName->setText(appName);
}

void GenericNotificationBox::setTitle(const QString & title) {
    m_title->setText(title);
}

void GenericNotificationBox::setMessage(const QString & message) {
    m_message->setText(message);
}

void GenericNotificationBox::setBackgroundColor(const QColor & color) {
    m_backgroundColor = color;
}
