#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loginBtn_clicked();

    void on_sayBtn_clicked();

    void on_logoutBtn_clicked();

    void connecedToServer();

    void messageReceived(const QString &sender, const QString &msg);

    void jsonReceived(const QJsonObject &docobj);

    void userJoined(const QString &user);
private:
    Ui::MainWindow *ui;

    ChatClient *m_chatClient;
};
#endif // MAINWINDOW_H
