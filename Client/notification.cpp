#include "notification.h"
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

Notification::Notification(QWidget *parent) : QWidget(parent)
{

  setWindowFlags(Qt::FramelessWindowHint |        // Disable window decoration
                 Qt::Tool |                       // Cancel the show in a separate window
                 Qt::WindowStaysOnTopHint);       // Set on top of all windows
  setAttribute(Qt::WA_TranslucentBackground);     // Indicates that it will be transparent
  setAttribute(Qt::WA_ShowWithoutActivating);     // At the show, the widget does not get the focus automatically

  animation.setTargetObject(this);                // Set the target animation
  animation.setPropertyName("popupOpacity");      // Install animated property
  connect(&animation, &QAbstractAnimation::finished, this, &Notification::hide); /*Connect the signal to the end
                                                                                  *animation to hide slots  * */


  label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  label.setStyleSheet("QLabel { color : white; "
                      "margin-top: 6px;"
                      "margin-bottom: 6px;"
                      "margin-left: 10px;"
                      "margin-right: 10px; }");


  layout.addWidget(&label, 0, 0);
  setLayout(&layout);
  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &Notification::HideAnimation);
}

void Notification::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QRect roundedRect;
  roundedRect.setX(rect().x() + 5);
  roundedRect.setY(rect().y() + 5);
  roundedRect.setWidth(rect().width() - 10);
  roundedRect.setHeight(rect().height() - 10);

  painter.setBrush(QBrush(QColor(0,0,0,180)));
  painter.setPen(Qt::NoPen);

  painter.drawRoundedRect(roundedRect, 10, 10);
}

void Notification::SetPopupText(const QString &text)
{
  label.setText(text);
  adjustSize();
}

void Notification::Show()
{
  setWindowOpacity(0.0);
  animation.setDuration(150);
  animation.setStartValue(0.0);
  animation.setEndValue(1.0);

  setGeometry(QApplication::desktop()->availableGeometry().width() - 36 - width() + QApplication::desktop() -> availableGeometry().x(),
              QApplication::desktop()->availableGeometry().height() - 36 - height() + QApplication::desktop() -> availableGeometry().y(),
              width(),
              height());
  QWidget::show();
  animation.start();
  timer->start(3000);
}

void Notification::HideAnimation()
{
  timer->stop();
  animation.setDuration(1000);
  animation.setStartValue(1.0);
  animation.setEndValue(0.0);
  animation.start();
}

void Notification::Hide()
{
  if(GetPopupOpacity() == 0.0){
      QWidget::hide();
    }
}

void Notification::SetPopupOpacity(float opacity)
{
  popupOpacity = opacity;
  setWindowOpacity(opacity);
}

float Notification::GetPopupOpacity() const
{
  return popupOpacity;
}

