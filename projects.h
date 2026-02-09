#ifndef PROJECTS_H
#define PROJECTS_H

#include <QString>
#include <QSqlQueryModel>

class Projects {
private:
    int id;
    QString name;
    QString description;
    QString startDate;
    QString endDate;
    double budget;

public:
    Projects();
    Projects(int id, QString name, QString description, QString startDate, QString endDate, double budget);

    int getId();
    QString getName();
    QString getDescription();
    QString getStartDate();
    QString getEndDate();
    double getBudget();

    void setId(int id);
    void setName(QString name);
    void setDescription(QString description);
    void setStartDate(QString startDate);
    void setEndDate(QString endDate);
    void setBudget(double budget);

    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool checkIfExists(int id);
    bool modifier(int id);
};

#endif // PROJECTS_H