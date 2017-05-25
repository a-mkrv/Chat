#include "authorization.h"
#include "ui_authorization.h"

Authorization::Authorization(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Authorization)
{
  ui->setupUi(this);

#ifdef __APPLE__
    ui->username_enter->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->pass_enter->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

  socket = new QTcpSocket;
  registration = new Registration(parent);
  passhash = new HashMD5;

  socket->abort();
  socket->connectToHost("127.0.0.1", 55155);

  connect(ui->log_in, SIGNAL(clicked()), this, SLOT(onButtonSendUser()));
  connect(socket, SIGNAL(readyRead()), this, SLOT(GetMessage()));
  connect(socket, SIGNAL(disconnected()), this, SLOT(close()));
  connect(registration, SIGNAL(sendData(QString)), this, SLOT(RecieveData(QString)));

  ui->error_label->hide();
  ui->errorconnect_label->hide();
  ui->pass_enter->setEchoMode(QLineEdit::Password);

  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
  this->show();
}

void Authorization::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() && Qt::LeftButton) {
      move(event->globalPos() - m_dragPosition);
      event->accept();
    }
}

void Authorization::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
      m_dragPosition = event->globalPos() - frameGeometry().topLeft();
      event->accept();
    }
}

void Authorization::onButtonSendUser()
{
  emit sendFindContact(ui->pass_enter->text().simplified());
}

void Authorization::on_log_in_clicked()
{
  QString request = "AUTH";

  if (socket->state() != QAbstractSocket::ConnectedState)
    ui->errorconnect_label->show();
  else
    ui->errorconnect_label->hide();

  QString login = ui->username_enter->text().simplified();
  QString password = passhash->HashSumPass(ui->pass_enter->text().simplified());

  if (!login.isEmpty() && !ui->pass_enter->text().simplified().isEmpty())
    {
      request.append(QString("%1 /s %2").arg(login).arg(password));
      socket->write(request.toUtf8());
    }
}

void Authorization::GetMessage()
{
  QString received_message, pub_key;

  received_message = socket->read(4);
  pub_key = socket->readAll();

  qDebug() << received_message << pub_key;

  if (received_message == "ERRA")
    ui->error_label->show();

  else if (received_message == "OKEY"
           && !ui->username_enter->text().simplified().isEmpty()
           && !ui->pass_enter->text().simplified().isEmpty()
           )
    {
      emit sendData(ui->username_enter->text().simplified(), ui->pass_enter->text().simplified(), pub_key);
      socket->close();
      socket->disconnectFromHost();
    }
}

void Authorization::keyReleaseEvent(QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Return:
      on_log_in_clicked();
      break;
    }
}

void Authorization::on_reg_button_clicked()
{
  if (socket->state() != QAbstractSocket::ConnectedState)
    ui->errorconnect_label->show();
  else
    {
      ui->errorconnect_label->hide();
      registration->show();
      this->hide();
    }
}

void Authorization::SetLanguage(QMap<QString, QString> & lan_dict)
{
  ui->username_enter->setPlaceholderText(lan_dict.value(ui->username_enter->objectName()));
  ui->pass_enter->setPlaceholderText(lan_dict.value(ui->pass_enter->objectName()));
  ui->error_label->setText(lan_dict.value(ui->error_label->objectName()));
  ui->reg_button->setText(lan_dict.value(ui->reg_button->objectName()));
  ui->log_in->setText(lan_dict.value(ui->log_in->objectName()));
  ui->errorconnect_label->setText(lan_dict.value(ui->errorconnect_label->objectName()));
  registration->SetLanguage(lan_dict);
}

void Authorization::RecieveData(QString rStr)
{
  if (rStr == "Show")
    this->show();
}

void Authorization::on_close_window_clicked()
{
  this->close();
}

void Authorization::on_turn_window_clicked()
{
  this->showMinimized();
}

Authorization::~Authorization()
{
  delete ui;
}

