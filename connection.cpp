#include "connection.h"

Connection::Connection()
{

}

#include <QDebug>
#include <QSqlError>

bool Connection::createconnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("source_project2a"); // Assure-toi que ce DSN existe bien
    db.setUserName("ela");
    db.setPassword("esprit2025");

    if (db.open()) {
        test = true;
        qDebug() << "Connexion réussie !";
    } else {
        qDebug() << "Échec de connexion :" << db.lastError().text();
    }

    return test;
}
