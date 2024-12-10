#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonValue>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    m_chatClient = new ChatClient(this);

    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connecedToServer);
    // connect(m_chatClient, &ChatClient::messageReceived, this, &MainWindow::messageReceived);
    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginBtn_clicked()
{
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()), 1967);
}


void MainWindow::on_sayBtn_clicked()
{
    if (!ui->sayLineEdit->text().isEmpty()) {
        m_chatClient->sendMessage(ui->sayLineEdit->text());
    }
}


void MainWindow::on_logoutBtn_clicked()
{
    m_chatClient->disconnectFromHost();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    for (auto aItem : ui->userListWidget->findItems(ui->usernameEdit->text(), Qt::MatchExactly)) {
        ui->userListWidget->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::connecedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    m_chatClient->sendMessage(ui->usernameEdit->text(), "login");

}

void MainWindow::messageReceived(const QString &sender, const QString &msg)
{
    ui->roomTextEdit->append(QString("%1 : %2").arg(sender).arg(msg));
}

void MainWindow::jsonReceived(const QJsonObject &docobj)
{
    const QJsonValue typeVal = docobj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docobj.value("text");
        const QJsonValue senderVal = docobj.value("sender");

        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;

        messageReceived(senderVal.toString(), textVal.toString());

    } else if (typeVal.toString().compare("newuser", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docobj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userJoined(usernameVal.toString());

    } else if (typeVal.toString().compare("userdisconnected", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docobj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userLeft(usernameVal.toString());

    } else if (typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0) {
        const QJsonValue userlistVal = docobj.value("userlist");
        if (userlistVal.isNull() || !userlistVal.isArray())
            return;

        userListReceived(userlistVal.toVariant().toStringList());
    }
}

void MainWindow::userJoined(const QString &user)
{
    ui->userListWidget->addItem(user);
}

void MainWindow::userLeft(const QString &user)
{
    for (auto aItem : ui->userListWidget->findItems(user, Qt::MatchExactly)) {
        ui->userListWidget->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::userListReceived(const QStringList &list)
{
    ui->userListWidget->clear();
    ui->userListWidget->addItems(list);
}

