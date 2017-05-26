#ifndef CLIENT_H
#define CLIENT_H

#include <QtConcurrent/QtConcurrent>
#include <QGraphicsOpacityEffect>
#include <QtMultimedia/QSound>
#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>
#include <QStackedLayout>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QFileDialog>
#include <QStackedWidget>
#include <QCoreApplication>
#include <QPoint>
#include <QMenu>
#include <QTime>
#include <QPair>
#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QString>
#include <QPainter>
#include <QDir>
#include <QColorDialog>
#include <QKeyEvent>

#include "emojiframe.h"
#include "findcontacts.h"
#include "trayicon.h"
#include "authorization.h"
#include "listwidgetemoji.h"
#include "userlistdelegate.h"
#include "chatlistdelegate.h"
#include "aboutdialog.h"
#include "notification.h"
#include "rsacrypt.h"
#include "confirmwindow.h"
#include "creategroup.h"
#include "choicecreate.h"
#include "selectcontacts.h"
#include "usersgroupinfo.h"
#include "xml_language.h"


typedef QList <QPair <QString, QList<QPair <QString, QString> > > > ChatListVector;
typedef QList <QPair <QString, QString>> PairStringList;

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
  void on_sendMessage_clicked();
  void on_userSetting_clicked();
  void on_close_setting_button_clicked();
  void on_pushButton_clicked();
  void on_newContact_Button_clicked();
  void on_actionShowHideWindow_triggered();
  void on_actionExit_triggered();
  void on_PB_SelColor_clicked();
  void on_PB_LoadFileBackground_clicked();
  void on_radioButton_2_clicked();
  void on_radioButton_clicked();
  void on_Download_path_PB_clicked();
  void on_pushButton_2_clicked();
  void on_userList_clicked(const QModelIndex &index);
  void on_pushButton_3_clicked();
  void on_pushButton_5_clicked();
  void on_search_line_edit_textChanged(const QString &arg1);
  void on_search_list_clicked(const QModelIndex &index);
  void on_glass_button_clicked();
  void on_info_user_button_clicked();
  void on_select_language_box_currentIndexChanged(int index);

  void ChoiceWindow(QString);
  void AddGroupToList(QStringList, QString);
  void ShowContextMenuForWidget(const QPoint &pos);
  void ShowContextMenuForChat(const QPoint &pos);
  void ClearHistory();
  void ClearCurHistory(QString);
  void ClearCurUser(QString);
  void ClearSelect();
  void DeleteUser();
  void SetGlass();
  void ParseResponseUserList(QString, ChatListVector &);
  void ParseResponseChatList(QString );
  void AddUserToList(QString, QString, PairStringList, int);
  void AddChatToList(QString );
  void InsertEmoticon(QString symbol);
  void FindUserInServer(QString str);
  void WhisperOnClickUsers(QListWidgetItem* user);
  void IconActivated(QSystemTrayIcon::ActivationReason reason);
  void ShowHideWindow();
  void GetMessage();
  void ShowError(QAbstractSocket::SocketError errorSocket);
  void SendPersonalData();
  void OnDisconnect();
  void WhisperOnClick(QListWidgetItem* user);
  void ShowEmoji();
  void ShowUserFound();
  void ShowCreateGroup();
  void GetCreateGroupSig(QString, QString, QString, QString);
  void SetDefaultLanguage();
  void SetLanguage();
  void RemoveSelections();
  QString SuffixDay(int day);
  QString Pronoun(QString username);

public slots:

  // Sending personal data at authorization and receiving keys.
  void RecieveData(QString str, QString pas, QString);

signals:
  void find_user_Serv(QString str);
  void setProcentage(int);

private:
  Ui::Client *ui;

  int FriendCount;
  int key;
  quint32 blockSize;
  quint32 nextBlockSize;
  bool personDates;

  QMenu *trayIconMenu;
  QStackedLayout *layout;
  QTcpSocket *tcpSocket;

  QMap<QString, QString> pubFriendKey;
  QVector<QListWidgetItem*> vec;
  QVector<QListWidget*> chatvec;
  QMap<QString, QString> lan_dict;
  QHash<QString, QString> FriendOnlineStatus;
  QHash<QString, QString> SexOfUsers;

  QStringList groupData;
  QString name;
  QString download_path;
  QString colorchat;
  QString myPrivateKey;
  QString myPublicKey;
  QPoint m_dragPosition;

  QStackedWidget *stackchat;
  QFile *sendFile;

  TrayIcon *trayIcon;
  EmojiManager *emojiMan;
  EmojiFrame *frameEmoji;
  findcontacts *findcont;
  AboutDialog *aboutdialog;
  Authorization *authorization;
  ConfirmWindow *conf_message;
  CreateGroup *create_group;
  ChoiceCreate *choice_window;
  SelectContacts *selectContacts;
  UsersGroupInfo *users_info;
  Notification *notice;
  RSACrypt *rsaCrypt;
  XML_Language *set_language;
};

#endif // CLIENT_H
