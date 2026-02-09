#include "equipment.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QObject>
#include <QIntValidator>
#include <QSqlError>
#include <QString>
#include <QComboBox>

Equipment::Equipment() {
    id = 0;
    name = "";
    type = "";
    status = "";
}

Equipment::Equipment(int id, QString name, QString type, QString status) {
    this->id = id;
    this->name = name;
    this->type = type;
    this->status = status;
}

int Equipment::getId() { return id; }
QString Equipment::getName() { return name; }
QString Equipment::getType() { return type; }
QString Equipment::getStatus() { return status; }

void Equipment::setId(int id) { this->id = id; }
void Equipment::setName(QString name) { this->name = name; }
void Equipment::setType(QString type) { this->type = type; }
void Equipment::setStatus(QString status) { this->status = status; }

bool Equipment::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO EQUIPMENTS (ID, NAME, TYPE, STATUS) "
                  "VALUES (:id, :name, :type, :status)");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":status", status);

    return query.exec();
}

QSqlQueryModel* Equipment::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EQUIPMENTS");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("NAME"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("TYPE"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("STATUS"));

    return model;
}

bool Equipment::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM EQUIPMENTS WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Equipment::checkIfExists(int id) {
    QSqlQuery query;
    query.prepare("SELECT ID FROM EQUIPMENTS WHERE ID = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        return true;
    } else {
        return false;
    }
}

bool Equipment::modifier(int id) {
    QSqlQuery query;
    query.prepare("UPDATE EQUIPMENTS SET NAME = :name, TYPE = :type, STATUS = :status "
                  "WHERE ID = :id");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":status", status);

    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text(); // Print SQL error
        return false;
    }
    return true;
}
