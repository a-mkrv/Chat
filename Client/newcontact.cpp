#include "newcontact.h"
#include "ui_newcontact.h"
#include <QTime>

NewContact::NewContact(QWidget *parent, QTcpSocket *client) :
    QWidget(parent),
    ui(new Ui::NewContact)
{

    ui->setupUi(this);
    ui->Error_label->hide();
    socket = client;

    connect(socket, SIGNAL(readyRead()), this, SLOT(getMessagee()));
}

NewContact::~NewContact()
{
    delete ui;
}

void NewContact::getMessagee()
{
        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_5_4);
        QString mes;
        in >> mes;
        qDebug() << "GetNew" << mes;
        if(mes == "Already")
            ui->Error_label->show();
        if(mes == "Welcome!" && !ui->enter_city->text().isEmpty() \
                && !ui->enter_password->text().isEmpty() \
                && !ui->enter_user_name->text().isEmpty() \
                && !ui->age->text().isEmpty() \
                && !ui->sex_person->currentText().isEmpty())
        {
            emit sendData(QString("Show"));
            this->~NewContact();
        }
}

void NewContact::on_accept_button_clicked()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);

    out << quint16(0) << QTime::currentTime() << QString("_REG_") << ui->enter_user_name->text() << ui->enter_city->text() << ui->enter_password->text() << ui->age->text() << ui->sex_person->currentText();

    socket->write(block);
}
