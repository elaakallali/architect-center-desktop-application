#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QString>
#include <QByteArray>

class SmtpClient : public QObject
{
    Q_OBJECT

public:
    explicit SmtpClient(const QString &host, int port,
                        const QString &username, const QString &password,
                        QObject *parent = nullptr);

    void sendEmail(const QString &to, const QString &subject,
                   const QString &body, const QString &attachmentPath = "");

signals:
    void emailSent(bool success);
    void errorOccurred(const QString &error);

private slots:
    void onSocketReadyRead();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    enum State {
        Disconnected,
        Connected,
        Authenticating,
        Sending
    };

    QSslSocket *socket;
    QString host;
    int port;
    QString username;
    QString password;
    State state;

    QString currentTo;
    QString currentSubject;
    QString currentBody;
    QString currentAttachmentPath;

    void sendCommand(const QString &command);
    QString base64Encode(const QString &text);
    void processResponse(const QString &response);
    void sendEmailData();
};

#endif // SMTPCLIENT_H
