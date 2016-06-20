#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class Notification : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(float popupOpacity READ GetPopupOpacity WRITE SetPopupOpacity)

public:
  explicit Notification(QWidget *parent = 0);

protected:
  void paintEvent(QPaintEvent *event);

public slots:
  void SetPopupText(const QString& text);
  void Show();

private slots:
  void HideAnimation();
  void Hide();

private:
  QLabel label;
  QGridLayout layout;
  QPropertyAnimation animation;
  float popupOpacity;     // Properties translucent widget
  QTimer *timer;          // The timer on which the widget is hidden

  void SetPopupOpacity(float opacity);
  float GetPopupOpacity() const;
};

#endif // NOTIFICATION_H
