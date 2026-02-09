#include "meetings.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Meetings::Meetings() {
    // Initialize default values if necessary
}

bool Meetings::scheduleMeeting(const QString &title, const QDateTime &dateTime, const QString &location) {
    QSqlQuery query;
    query.prepare("INSERT INTO MEETINGS (TITLE, DATE_TIME, LOCATION) VALUES (:title, :date_time, :location)");
    query.bindValue(":title", title);
    query.bindValue(":date_time", dateTime);
    query.bindValue(":location", location);

    if (!query.exec()) {
        qDebug() << "Error scheduling meeting:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQueryModel* Meetings::displayMeetings() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM MEETINGS");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Title"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date & Time"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Location"));
    return model;
}

bool Meetings::deleteMeeting(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM MEETINGS WHERE ID = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error deleting meeting:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Meetings::updateMeeting(int id, const QString &title, const QDateTime &dateTime, const QString &location) {
    QSqlQuery query;
    query.prepare("UPDATE MEETINGS SET TITLE = :title, DATE_TIME = :date_time, LOCATION = :location WHERE ID = :id");
    query.bindValue(":title", title);
    query.bindValue(":date_time", dateTime);
    query.bindValue(":location", location);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error updating meeting:" << query.lastError().text();
        return false;
    }
    return true;
}