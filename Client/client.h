#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>
#include <QStackedLayout>
#include <QListWidgetItem>
#include <QMenu>
#include <QCloseEvent>
#include <QtMultimedia/QSound>
#include "emojiframe.h"
#include "findcontacts.h"
#include "trayicon.h"
#include "registration.h"
#include <QtConcurrent/QtConcurrent>
#include <QStackedWidget>
#include "listwidgetemoji.h"
#include <QDir>
#include <QCoreApplication>
#include <QColorDialog>
#include <QKeyEvent>
#include "listdelegate.h"
#include "chatlistdelegate.h"
#include "aboutdialog.h"
#include "notification.h"
#include <QMouseEvent>
#include <QPoint>
#include <QTime>

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    virtual void keyPressEvent(QKeyEvent* event) { key = event->key(); }
    virtual void keyReleaseEvent(QKeyEvent* event);

    ~Client();


protected:
    void closeEvent(QCloseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void getMessage();                  //Получение сообщений
    void on_sendMessage_clicked();      //Отправка сообщения
    void show_Error(QAbstractSocket::SocketError errorSocket);  //Их большая часть в коде -_-
    void send_personal_data();          //Отправка сведений о клиенте
    void onDisconnect();                //Отключение
    void sendUserCommand(QString command);
    void on_userSetting_clicked();      //Настройки (-)
    void on_close_setting_button_clicked();
    void whisperOnClick(QListWidgetItem* user);
    void showEmoji();
    void showFindCont();
    void on_pushButton_clicked();
    void on_newContact_Button_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showHideWindow();
    void on_actionShowHideWindow_triggered();
    void on_actionExit_triggered();
    void insertEmoticon(QString symbol);
    void on_PB_SelColor_clicked();
    void on_PB_LoadFileBackground_clicked();
    void findtoserv(QString str);
    void whisperOnClickUsers(QListWidgetItem* user);
    void whisperOnClickSelectUsers(QListWidgetItem* user);
    void on_radioButton_2_clicked();
    void on_radioButton_clicked();
    void on_Download_path_PB_clicked();
    void on_pushButton_2_clicked();

    void on_userList_clicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_comboBox_currentIndexChanged(int index);

public slots:
    void recieveData(QString str, QString pas);

signals:
    void find_user_Serv(QString str);
    void setProcentage(int);
private:
    Ui::Client *ui;

    int key;
    TrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QStackedLayout *layout;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QString getIP();
    EmojiManager *emojiMan;
    EmojiFrame *frameEmoji;
    findcontacts *findcont;
    AboutDialog *aboutdialog;
    registration *reg_window;
    Notification *notice;

    QPoint m_dragPosition;
    QVector<QListWidgetItem*> vec;
    QVector<QListWidget*> chatvec;

    QStackedWidget *stackchat;
    QFile *sendFile;
    QString name;
    QString download_path;
    bool personDates;
    int t;
};

#endif // CLIENT_H
