#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <QSqlQueryModel>

class Employee {
private:
    int id;
    QString name;
    QString phone;
    QString email;
    double salary;
    double workhours;           // changed from QString to double
    QString assignedProject;
    double performanceRating;   // must not surpass 10

public:
    Employee();
    Employee(int id, QString name, QString phone, QString email, double salary, double workhours, QString assignedProject, double performanceRating);

    int getId();
    QString getName();
    QString getPhone();
    QString getEmail();
    double getSalary();
    double getWorkhours();
    QString getAssignedProject();
    double getPerformanceRating();

    void setId(int id);
    void setName(QString name);
    void setPhone(QString phone);
    void setEmail(QString email);
    void setSalary(double salary);
    void setWorkhours(double workhours);
    void setAssignedProject(QString assignedProject);
    void setPerformanceRating(double performanceRating);

    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool checkIfExists(int id);
    bool modifier(int id);
};

#endif // EMPLOYEE_H