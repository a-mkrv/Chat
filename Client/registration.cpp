#include "registration.h"
#include "ui_registration.h"
#include <QTime>

Registration::Registration(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Registration)
{
  ui->setupUi(this);
  this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

#ifdef __APPLE__
  ui->enter_city->setAttribute(Qt::WA_MacShowFocusRect, false);
  ui->enter_password->setAttribute(Qt::WA_MacShowFocusRect, false);
  ui->enter_user_name->setAttribute(Qt::WA_MacShowFocusRect, false);
  ui->password_confirm->setAttribute(Qt::WA_MacShowFocusRect, false);
  ui->age->setAttribute(Qt::WA_MacShowFocusRect, false);
#endif

  socket = new QTcpSocket();
  rsacrypt = new RSACrypt();

  ui->Error_label->hide();
  ui->Error_label_2->hide();

  rsacrypt->GenerationKeys();

  connect(socket, SIGNAL(readyRead()), this, SLOT(getMessagee()));
}

void Registration::getMessagee()
{
  QString  received_message;

  received_message  = socket->read(4);

  if (received_message == "PNIL")
    {
      ui->Error_label->hide();
      ui->Error_label_2->setText(errors.at(0));
      ui->Error_label_2->show();
    }
  else  if (received_message == "ALRD")
    {
      ui->Error_label_2->hide();
      ui->Error_label->setText(errors.at(1));
      ui->Error_label->show();
    }

  else if (received_message == "WELC")
    {
      QString nameKey = QDir::homePath() + "/WhisperServer/Whisper Close Key/";
      QFile *receiveFile = new QFile(nameKey + ui->enter_user_name->text()+".txt");

      QTextStream out(receiveFile);
      receiveFile->open(QIODevice::Append);
      out << QString::number(rsacrypt->GetD()) + " " + QString::number(rsacrypt->GetModule());

      receiveFile->close();
      on_come_back_clicked();
    }
}

void Registration::on_accept_button_clicked()
{
  QString request = "REGI";

  if (ui->enter_password->text().isEmpty())
  {
      ui->Error_label_2->show();
      return;
  }
  else if (ui->password_confirm->text().isEmpty())
  {
    ui->Error_label_2->show();
    ui->Error_label_2->setText(errors.at(3));
    return;     //"Confirm pass is empty"
  }
  else if (ui->password_confirm->text() != ui->enter_password->text())
  {
    ui->Error_label_2->show();
    ui->Error_label_2->setText(errors.at(2));
    return;     //"Passwords are different"
  }

  socket->abort();
  socket->connectToHost("127.0.0.1", 55155);

  // use password = md5(md5(password) + salt)

  QString passmd5 = hashmd5->HashSumPass(ui->enter_password->text());
  QString salt = hashmd5->SaltGeneration();
  passmd5 = hashmd5->HashSumPass(passmd5 + salt);

  request.append(QString("%1 /s %2 /s ").arg(ui->enter_user_name->text()).arg(ui->enter_city->text()));
  request.append(QString("%1 /s %2 /s ").arg(passmd5).arg(ui->age->text()));
  request.append(QString("%1 /s %2 /s ").arg(ui->sex_person->currentText()).arg(QString::number(rsacrypt->GetE()) + "  " + QString::number(rsacrypt->GetModule())));
  request.append(QString("%1").arg(salt));

  socket->write(request.toUtf8());
}

void Registration::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() && Qt::LeftButton) {
      move(event->globalPos() - m_dragPosition);
      event->accept();
    }
}

void Registration::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
      m_dragPosition = event->globalPos() - frameGeometry().topLeft();
      event->accept();
    }
}

void Registration::SetLanguage(QMap<QString, QString> & lan_dict)
{
  ui->register_2->setText(lan_dict.value(ui->register_2->objectName()));
  ui->username->setText(lan_dict.value(ui->username->objectName()));
  ui->sex->setText(lan_dict.value(ui->sex->objectName()));
  ui->age_2->setText(lan_dict.value(ui->age->objectName()));
  ui->city->setText(lan_dict.value(ui->city->objectName()));
  ui->password->setText(lan_dict.value(ui->password->objectName()));
  ui->confirm->setText(lan_dict.value(ui->confirm->objectName()));
  ui->accept_button->setText(lan_dict.value(ui->accept_button->objectName()));
  errors = lan_dict.value("errors1").split("//");

  if (errors.size() == 4)
    {
      ui->Error_label->setText(errors.at(1));
      ui->Error_label_2->setText(errors.at(0));
    }

  QStringList lst_setting;
  lst_setting = lan_dict.value("sex_person").split(',');

  for (int i = 0; i < lst_setting.size(); i++)
    ui->sex_person->setItemText(i, lst_setting[i]);
}

void Registration::on_come_back_clicked()
{
  ui->enter_city->setText("");
  ui->enter_password->setText("");
  ui->enter_user_name->setText("");
  ui->sex_person->setCurrentIndex(0);
  ui->age->setValue(0);

  emit sendData(QString("Show"));
  this->close();
}

void Registration::on_close_window_clicked()
{
  this->close();
}

void Registration::on_turn_window_clicked()
{
  this->showMinimized();
}

Registration::~Registration()
{
  delete ui;
}
