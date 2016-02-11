#include "newcontact.h"
#include "ui_newcontact.h"
#include <QTime>

NewContact::NewContact(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewContact)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    socket = new QTcpSocket();

    ui->setupUi(this);
    ui->Error_label->hide();
    ui->Error_label_2->hide();

    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessagee()));
}

void NewContact::getMessagee()
{
    QString  received_message;

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_4);
    in >> received_message;

    if(received_message == "PassEmpty")
    {
        ui->Error_label->hide();
        ui->Error_label_2->show();
    }
    else if(received_message == "Already!")
    {
        ui->Error_label_2->hide();
        ui->Error_label->show();
    }

    else if(received_message == "Welcome!")
        on_pushButton_clicked();
}


void NewContact::on_accept_button_clicked()
{
    socket->abort();
    socket->connectToHost("127.0.0.1", 55155);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << quint32(0) << QTime::currentTime() << QString("_REG_") << ui->enter_user_name->text()
        << ui->enter_city->text() << ui->enter_password->text()
        << ui->age->text() << ui->sex_person->currentText();

    socket->write(block);
}

void NewContact::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() && Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void NewContact::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void NewContact::on_pushButton_clicked()
{
    ui->enter_city->setText("");
    ui->enter_password->setText("");
    ui->enter_user_name->setText("");
    ui->sex_person->setCurrentIndex(0);
    ui->age->setValue(0);

    emit sendData(QString("Show"));
    this->close();
}

NewContact::~NewContact()
{
    delete ui;
}

void NewContact::on_closeregBut_clicked()
{
    this->close();
}

void NewContact::on_minimazregBut_clicked()
{
    this->showMinimized();
}
