#include "projects.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Projects::Projects() {
    id = 0;
    name = "";
    description = "";
    startDate = QDate();
    endDate = QDate();
    budget = 0.0;
}

Projects::Projects(int id, QString name, QString description, QDate startDate, QDate endDate, double budget) {
    this->id = id;
    this->name = name;
    this->description = description;
    this->startDate = startDate;
    this->endDate = endDate;
    this->budget = budget;
}

int Projects::getId() { return id; }
QString Projects::getName() { return name; }
QString Projects::getDescription() { return description; }
QDate Projects::getStartDate() { return startDate; }
QDate Projects::getEndDate() { return endDate; }
double Projects::getBudget() { return budget; }

void Projects::setId(int id) { this->id = id; }
void Projects::setName(QString name) { this->name = name; }
void Projects::setDescription(QString description) { this->description = description; }
void Projects::setStartDate(QDate startDate) { this->startDate = startDate; }
void Projects::setEndDate(QDate endDate) { this->endDate = endDate; }
void Projects::setBudget(double budget) { this->budget = budget; }

bool Projects::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO PROJECTS (NAME, DESCRIPTION, START_DATE, END_DATE, BUDGET) "
                  "VALUES (:name, :description, :start_date, :end_date, :budget)");
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);
    query.bindValue(":budget", budget);

    return query.exec();
}

QSqlQueryModel* Projects::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM PROJECTS");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Start Date"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("End Date"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Budget"));
    return model;
}

bool Projects::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM PROJECTS WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Projects::modifier(int id) {
    QSqlQuery query;
    query.prepare("UPDATE PROJECTS SET NAME = :name, DESCRIPTION = :description, START_DATE = :start_date, "
                  "END_DATE = :end_date, BUDGET = :budget WHERE ID = :id");
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":start_date", startDate);
    query.bindValue(":end_date", endDate);
    query.bindValue(":budget", budget);
    query.bindValue(":id", id);

    return query.exec();
}