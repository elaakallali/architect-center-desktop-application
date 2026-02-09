#ifndef AI_DIALOG_H
#define AI_DIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class AIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIDialog(QWidget *parent = nullptr);
    ~AIDialog();

private slots:
    void onSendClicked();
    void handleReply();

private:
    void sendToAI(const QString &text);
    void addMessage(const QString &sender, const QString &message);

    QListWidget *chatList;
    QTextEdit *inputBox;
    QPushButton *sendButton;

    QNetworkAccessManager *networkManager;
    QNetworkReply *currentReply;
};

#endif // AI_DIALOG_H