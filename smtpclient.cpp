#include "smtpclient.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

SmtpClient::SmtpClient(const QString &host, int port,
                       const QString &username, const QString &password,
                       QObject *parent)
    : QObject(parent),
    socket(new QSslSocket(this)),
    host(host),
    port(port),
    username(username),
    password(password),
    state(Disconnected)
{
    // Corrected signal connections
    connect(socket, &QSslSocket::readyRead, this, &SmtpClient::onSocketReadyRead);
    connect(socket, &QSslSocket::connected, this, &SmtpClient::onSocketConnected);
    connect(socket, &QSslSocket::disconnected, this, &SmtpClient::onSocketDisconnected);

// Fixed error signal connection
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
            this, &SmtpClient::onSocketError);
#else
    connect(socket, &QSslSocket::errorOccurred,
            this, &SmtpClient::onSocketError);
#endif
}

void SmtpClient::sendEmail(const QString &to, const QString &subject,
                           const QString &body, const QString &attachmentPath)
{
    if (state != Disconnected) {
        emit errorOccurred("Already in process of sending an email");
        return;
    }

    currentTo = to;
    currentSubject = subject;
    currentBody = body;
    currentAttachmentPath = attachmentPath;

    qDebug() << "Connecting to SMTP server...";
    socket->connectToHostEncrypted(host, port);
}

void SmtpClient::onSocketReadyRead()
{
    QString response;
    while (socket->canReadLine()) {
        response = QString::fromUtf8(socket->readLine());
        qDebug() << "SMTP:" << response.trimmed();
        processResponse(response);
    }
}

void SmtpClient::onSocketConnected()
{
    qDebug() << "Connected to SMTP server";
    state = Connected;
}

void SmtpClient::onSocketDisconnected()
{
    qDebug() << "Disconnected from SMTP server";
    state = Disconnected;
}

void SmtpClient::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorMsg = QString("SMTP Error: %1").arg(socket->errorString());
    qDebug() << errorMsg;
    emit errorOccurred(errorMsg);
    state = Disconnected;
}

void SmtpClient::processResponse(const QString &response)
{
    if (state == Connected && response.startsWith("220")) {
        sendCommand("EHLO " + host);
    }
    else if (state == Connected && response.startsWith("250")) {
        sendCommand("AUTH LOGIN");
        state = Authenticating;
    }
    else if (state == Authenticating && response.startsWith("334 VXNlcm5hbWU6")) { // Username
        sendCommand(base64Encode(username));
    }
    else if (state == Authenticating && response.startsWith("334 UGFzc3dvcmQ6")) { // Password
        sendCommand(base64Encode(password));
    }
    else if (state == Authenticating && response.startsWith("235")) {
        state = Sending;
        sendCommand("MAIL FROM: <" + username + ">");
    }
    else if (state == Sending && response.startsWith("250") && response.contains("MAIL FROM")) {
        sendCommand("RCPT TO: <" + currentTo + ">");
    }
    else if (state == Sending && response.startsWith("250") && response.contains("RCPT TO")) {
        sendCommand("DATA");
    }
    else if (state == Sending && response.startsWith("354")) {
        sendEmailData();
    }
    else if (state == Sending && response.startsWith("250") && response.contains("DATA")) {
        sendCommand("QUIT");
    }
    else if (response.startsWith("221")) {
        socket->disconnectFromHost();
        emit emailSent(true);
    }
    else if (response.startsWith("5")) {
        emit errorOccurred("SMTP Error: " + response);
        socket->disconnectFromHost();
    }
}

void SmtpClient::sendEmailData()
{
    QString emailData;
    emailData += "From: " + username + "\r\n";
    emailData += "To: " + currentTo + "\r\n";
    emailData += "Subject: " + currentSubject + "\r\n";
    emailData += "MIME-Version: 1.0\r\n";
    emailData += "Content-Type: multipart/mixed; boundary=\"boundary\"\r\n\r\n";

    // Message body
    emailData += "--boundary\r\n";
    emailData += "Content-Type: text/plain; charset=utf-8\r\n\r\n";
    emailData += currentBody + "\r\n\r\n";

    // Attachment
    if (!currentAttachmentPath.isEmpty()) {
        QFile file(currentAttachmentPath);
        if (file.open(QIODevice::ReadOnly)) {
            emailData += "--boundary\r\n";
            emailData += "Content-Type: application/octet-stream; name=\"" + QFileInfo(file).fileName() + "\"\r\n";
            emailData += "Content-Disposition: attachment; filename=\"" + QFileInfo(file).fileName() + "\"\r\n";
            emailData += "Content-Transfer-Encoding: base64\r\n\r\n";
            emailData += file.readAll().toBase64() + "\r\n";
            file.close();
        }
    }

    emailData += "--boundary--\r\n.\r\n";
    sendCommand(emailData);
}

void SmtpClient::sendCommand(const QString &command)
{
    qDebug() << "CMD:" << command.left(command.indexOf('\r')); // Don't log full email data
    socket->write(command.toUtf8());
}

QString SmtpClient::base64Encode(const QString &text)
{
    return QString::fromLatin1(text.toUtf8().toBase64());
}
