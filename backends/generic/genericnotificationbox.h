#ifndef GENERICNOTIFICATIONBOX_H
#define GENERICNOTIFICATIONBOX_H

#include <QWidget>
#include <QLabel>

class GenericNotificationButton : public QLabel {
    Q_OBJECT
public:
    GenericNotificationButton(const QPixmap & pixmap, QWidget* parent = 0);
Q_SIGNALS:
    void clicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class GenericNotificationBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor)

public:
    GenericNotificationBox(QWidget *parent = 0);
    ~GenericNotificationBox();

    void setIcon(const QPixmap & icon);
    void setApplicationName(const QString & appName);
    void setTitle(const QString & title);
    void setMessage(const QString & message);

    void setBackgroundColor(const QColor & color);


    //QWidget
    QSize sizeHint () const;

protected:
    void paintEvent(QPaintEvent *);
    //bool eventFilter(QObject *, QEvent *);
    void mousePressEvent(QMouseEvent *);

public Q_SLOTS:
    void show(Qt::Corner corner);

Q_SIGNALS:
    void clicked();
    void dismissed();

private:
    QLabel* m_applicationName;
    QLabel* m_title;
    QLabel* m_message;
    QColor m_backgroundColor;
    QLabel* m_icon;

};

#endif // GENERICNOTIFICATIONBOX_H
