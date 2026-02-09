#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQueryModel>
#include <QByteArray>
#include <QFileDialog>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QDateTime>

class Client  {

public:

    // Constructors
    Client();
    Client(int id, QString name, QString email, int phone, QString address, QByteArray contract, int projects);

    // Getters
    int getId() const;
    QString getName() const;
    QString getEmail() const;
    int getPhone() const;
    QString getAddress() const;
    QByteArray getContract() const;
    int getProjects() const;
    // Setters
    void setId(int id);
    void setName(const QString &name);
    void setEmail(const QString &email);
    void setPhone(int phone);
    void setAddress(const QString &address);
    void setContract(const QByteArray &contract);
    void setProjects(int projects);

    // CRUD Operations
    bool validateId(int id);
    bool validatePhone(int phone);
    bool add();
    QSqlQueryModel* view(int id);
    bool deleteClient(int id);
    bool update(int id);
    bool checkIfExists(int id);
    bool exportAllToPdf();
    QSqlQueryModel* searchByName(const QString &name);
    bool sendEmail(const QString &recipient, const QByteArray &contractData, const QString &subject, const QString &message);
    bool getClientDetailsForEmail(int id, QString &clientEmail, QByteArray &clientContract) const;
    QString getClientEmailFromDatabase(int id);
    bool logClientActivity(int id, const QString &action, const QString &additionalInfo);

private:
    int id;
    QString name;
    QString email;
    int phone;
    QString address;
    QByteArray contract;
    int projects;
};

#endif // CLIENT_H
