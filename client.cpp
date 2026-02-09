#include "client.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include<smtpclient.h>
#include <QRandomGenerator>
#include <QDateTime>
#include <QSslSocket>

#include <QTimer>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

Client::Client() {
    id = 0;
    name = "";
    email = "";
    phone = 0;
    address = "";
    projects = 0;
}

Client::Client(int id, QString name, QString email, int phone, QString address, QByteArray contract, int projects) {
    this->id = id;
    this->name = name;
    this->email = email;
    this->phone = phone;
    this->address = address;
    this->contract = contract;
    this->projects = projects;
}

int Client::getId() const {
    return id;
}

QString Client::getName() const {
    return name;
}

QString Client::getEmail() const {
    return email;
}

int Client::getPhone() const {
    return phone;
}

QString Client::getAddress() const {
    return address;
}

QByteArray Client::getContract() const {
    return contract;
}

void Client::setId(int id) {
    this->id = id;
}
int Client::getProjects() const {
    return projects;
}

void Client::setName(const QString &name) {
    this->name = name;
}

void Client::setEmail(const QString &email) {
    this->email = email;
}

void Client::setPhone(int phone) {
    this->phone = phone;
}

void Client::setAddress(const QString &address) {
    this->address = address;
}

void Client::setContract(const QByteArray &contract) {
    this->contract = contract;
}
void Client::setProjects(int id) {
    this->id = id;
}

bool Client::validateId(int id) {
    return id > 0;  // Ensure ID is positive and numeric
}

bool Client::validatePhone(int phone) {
    if (phone < 10000000 || phone > 99999999) {
        QMessageBox::warning(nullptr, "Invalid Phone", "Phone number must be exactly 8 digits.");
        return false;
    }
    return true;
}


bool Client::add() {
    QSqlQuery query;
    query.prepare("INSERT INTO CLIENTS (ID, NAME, EMAIL, PHONE, ADDRESS, CONTRACT, PROJECTS) "
                  "VALUES (:id, :name, :email, :phone, :address, :contract, :projects)");

    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":address", address);
    query.bindValue(":contract", contract);
    query.bindValue(":projects", projects);

    if (query.exec()) {
        logClientActivity(id, "Added", "Client added with name: " + name); // 👈 log from here
        return true;
    } else {
        qDebug() << "Failed to add client:" << query.lastError().text();
        return false;
    }
}


// Viewww clients
QSqlQueryModel* Client::view(int id) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT id, name, email, phone, address, contract, projects FROM clients WHERE id = :id");
    query.bindValue(":id", id);
    query.exec();

    model->setQuery(std::move(query));

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NAME"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("EMAIL"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("PHONE"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("ADDRESS"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("NUMBER OF PROJECTS"));
    return model;
}

// Delete client by ID
bool Client::deleteClient(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM CLIENTS WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec();
}

// Update client by ID
bool Client::update(int id) {
    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT name, email, phone, address, contract, projects FROM CLIENTS WHERE id = :id");
    selectQuery.bindValue(":id", id);

    if (!selectQuery.exec() || !selectQuery.next()) {
        qDebug() << "Failed to fetch existing client data:" << selectQuery.lastError().text();
        return false;
    }

    QString currentName = selectQuery.value(0).toString();
    QString currentEmail = selectQuery.value(1).toString();
    int currentPhone = selectQuery.value(2).toInt();
    QString currentAddress = selectQuery.value(3).toString();
    QByteArray currentContract = selectQuery.value(4).toByteArray();
    int currentProjects = selectQuery.value(5).toInt();

    QString finalName = name.isEmpty() ? currentName : name;
    QString finalEmail = email.isEmpty() ? currentEmail : email;
    int finalPhone = (phone == 0) ? currentPhone : phone;
    QString finalAddress = address.isEmpty() ? currentAddress : address;
    QByteArray finalContract = contract.isEmpty() ? currentContract : contract;
    int finalProjects = (projects == 0) ? currentProjects : projects;

    // Logging changes
    if (finalName != currentName)
        logClientActivity(id, "Updated", "Name changed from '" + currentName + "' to '" + finalName + "'");
    if (finalEmail != currentEmail)
        logClientActivity(id, "Updated", "Email changed from '" + currentEmail + "' to '" + finalEmail + "'");
    if (finalPhone != currentPhone)
        logClientActivity(id, "Updated", "Phone changed from '" + QString::number(currentPhone) + "' to '" + QString::number(finalPhone) + "'");
    if (finalAddress != currentAddress)
        logClientActivity(id, "Updated", "Address changed from '" + currentAddress + "' to '" + finalAddress + "'");
    if (finalProjects != currentProjects)
        logClientActivity(id, "Updated", "Projects changed from '" + QString::number(currentProjects) + "' to '" + QString::number(finalProjects) + "'");
    if (finalContract != currentContract)
        logClientActivity(id, "Updated", "Contract file was updated.");

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE CLIENTS SET name = :name, email = :email, phone = :phone, address = :address, contract = :contract, projects = :projects "
                        "WHERE id = :id");
    updateQuery.bindValue(":name", finalName);
    updateQuery.bindValue(":email", finalEmail);
    updateQuery.bindValue(":phone", finalPhone);
    updateQuery.bindValue(":address", finalAddress);
    updateQuery.bindValue(":contract", finalContract);
    updateQuery.bindValue(":id", id);
    updateQuery.bindValue(":projects", finalProjects);

    if (!updateQuery.exec()) {
        qDebug() << "SQL Error during update:" << updateQuery.lastError().text();
        return false;
    }

    return true;
}



bool Client::checkIfExists(int id) {
    QSqlQuery query;
    query.prepare("SELECT ID FROM CLIENTS WHERE ID = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return true;  // ID exists
    }
    return false;  // ID does not exist
}



bool Client::exportAllToPdf() {
    QSqlQuery query("SELECT id, name, email, phone, address, projects FROM clients");

    QString html = "<h2>Client List</h2><table border='1' cellspacing='0' cellpadding='3'>"
                   "<tr><th>ID</th><th>Name</th><th>Email</th><th>Phone</th><th>Address</th><th>Number of projects</th></tr>";

    while (query.next()) {
        html += "<tr>";
        html += "<td>" + query.value(0).toString() + "</td>";
        html += "<td>" + query.value(1).toString() + "</td>";
        html += "<td>" + query.value(2).toString() + "</td>";
        html += "<td>" + query.value(3).toString() + "</td>";
        html += "<td>" + query.value(4).toString() + "</td>";
        html += "<td>" + query.value(5).toString() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    QString filename = QFileDialog::getSaveFileName(nullptr, "Save PDF", "", "*.pdf");
    if (filename.isEmpty())
        return false;

    if (!filename.endsWith(".pdf", Qt::CaseInsensitive))
        filename += ".pdf";

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&printer);

    return true;
}

// Search by client name

QSqlQueryModel* Client::searchByName(const QString &searchTerm) {

    QSqlQueryModel *model = new QSqlQueryModel();


    QSqlQuery query;
    query.prepare("SELECT id, name, email, phone, address, contract, projects FROM CLIENTS WHERE name LIKE :name");


    query.bindValue(":name", "%" + searchTerm + "%");


    if (query.exec()) {

        model->setQuery(query);


        model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Email"));
        model->setHeaderData(3, Qt::Horizontal, QObject::tr("Phone"));
        model->setHeaderData(4, Qt::Horizontal, QObject::tr("Address"));
        model->setHeaderData(5, Qt::Horizontal, QObject::tr("PROJECTS"));
    } else {
        qDebug() << "Error executing search query:" << query.lastError().text();
    }

    return model;
}




bool Client::sendEmail(const QString &recipient, const QByteArray &contractData,
                       const QString &subject, const QString &message)
{

    QString smtpServer = "smtp.gmail.com";
    int smtpPort = 465;  // Use 465 for SSL, 587 for TLS
    QString senderEmail = "ela.kallali@gmail.com";
    QString senderPassword = "jfyx ywwg ksco radf";

    QSslSocket socket;
    socket.connectToHostEncrypted(smtpServer, smtpPort);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "Failed to connect to SMTP server:" << socket.errorString();
        return false;
    }
    if (!socket.waitForEncrypted(3000)) {
        qDebug() << "Failed to establish secure connection:" << socket.errorString();
        return false;
    }


    if (!socket.waitForReadyRead(3000)) {
        qDebug() << "Did not receive initial 220 greeting.";
        return false;
    }
    QString banner = socket.readAll();
    qDebug() << "SMTP Banner: " << banner;

    // SMTP communication helper function
    auto sendCommand = [&](const QString &command, int expectedCode) {
        socket.write(command.toUtf8() + "\r\n");
        if (!socket.waitForBytesWritten(3000)) return false;
        if (!socket.waitForReadyRead(3000)) return false;
        QString response = socket.readAll();
        qDebug() << "Command: " << command;
        qDebug() << "Response: " << response;
        return response.startsWith(QString::number(expectedCode));
    };

    // SMTP protocol sequence
    if (!sendCommand("EHLO " + smtpServer, 250)) return false;
    if (!sendCommand("AUTH LOGIN", 334)) return false;
    if (!sendCommand(senderEmail.toUtf8().toBase64(), 334)) return false;
    if (!sendCommand(senderPassword.toUtf8().toBase64(), 235)) return false;
    if (!sendCommand("MAIL FROM: <" + senderEmail + ">", 250)) return false;
    if (!sendCommand("RCPT TO: <" + recipient + ">", 250)) return false;
    if (!sendCommand("DATA", 354)) return false;

    // Compose email
    QString emailData;
    emailData += "From: " + senderEmail + "\r\n";
    emailData += "To: " + recipient + "\r\n";
    emailData += "Subject: " + subject + "\r\n";
    emailData += "MIME-Version: 1.0\r\n";
    emailData += "Content-Type: multipart/mixed; boundary=\"boundary\"\r\n\r\n";

    // Message body
    emailData += "--boundary\r\n";
    emailData += "Content-Type: text/plain\r\n\r\n";
    emailData += message + "\r\n\r\n";

    // Attachment
    emailData += "--boundary\r\n";
    emailData += "Content-Type: application/pdf; name=\"contract.pdf\"\r\n";
    emailData += "Content-Disposition: attachment; filename=\"contract.pdf\"\r\n";
    emailData += "Content-Transfer-Encoding: base64\r\n\r\n";
    emailData += contractData.toBase64() + "\r\n";
    emailData += "--boundary--\r\n.\r\n";

    if (!sendCommand(emailData, 250)) return false;
    sendCommand("QUIT", 221);

    qDebug() << "Email sent successfully!";
    return true;
}

QString Client::getClientEmailFromDatabase(int id) {
    QSqlQuery query;
    query.prepare("SELECT EMAIL FROM clients WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return QString();
}


bool Client::logClientActivity(int clientId, const QString &action, const QString &additionalInfo)
{
    QSqlQuery query;
    query.prepare("INSERT INTO CLIENTHISTORY (client_id, actions, datetime, additional_info) "
                  "VALUES (:client_id, :actions, :datetime, :info)");
    query.bindValue(":client_id", clientId);
    query.bindValue(":actions", action);
    query.bindValue(":datetime", QDateTime::currentDateTime());
    query.bindValue(":info", additionalInfo);

    if (!query.exec()) {
        qDebug() << "Failed to insert activity log:" << query.lastError().text();
        return false;
    }
    return true;
}
