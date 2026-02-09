#include "employee.h"
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlError>

Employee::Employee() {
    id = 0;
    name = "";
    phone = "";
    email = "";
    salary = 0.0;
    workhours = 0.0;
    assignedProject = "";
    performanceRating = 0.0;
}

Employee::Employee(int id, QString name, QString phone, QString email, double salary, double workhours, QString assignedProject, double performanceRating) {
    this->id = id;
    this->name = name;
    this->phone = phone;
    this->email = email;
    this->salary = salary;
    this->workhours = workhours;
    this->assignedProject = assignedProject;
    this->performanceRating = performanceRating;
}

int Employee::getId() { return id; }
QString Employee::getName() { return name; }
QString Employee::getPhone() { return phone; }
QString Employee::getEmail() { return email; }
double Employee::getSalary() { return salary; }
double Employee::getWorkhours() { return workhours; }
QString Employee::getAssignedProject() { return assignedProject; }
double Employee::getPerformanceRating() { return performanceRating; }

void Employee::setId(int id) { this->id = id; }
void Employee::setName(QString name) { this->name = name; }
void Employee::setPhone(QString phone) { this->phone = phone; }
void Employee::setEmail(QString email) { this->email = email; }
void Employee::setSalary(double salary) { this->salary = salary; }
void Employee::setWorkhours(double workhours) { this->workhours = workhours; }
void Employee::setAssignedProject(QString assignedProject) { this->assignedProject = assignedProject; }
void Employee::setPerformanceRating(double performanceRating) { this->performanceRating = performanceRating; }

bool Employee::ajouter() {
    QSqlQuery query;
    // ID is auto-assigned by the database
    query.prepare("INSERT INTO EMPLOYEES (NAME, PHONE, EMAIL, SALARY, WORK_HOURS, ASSIGNED_PROJECTS, PERFORMANCE_RATING) "
                  "VALUES (:name, :phone, :email, :salary, :work_hours, :assigned_projects, :performance_rating)");
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.bindValue(":salary", salary);
    query.bindValue(":work_hours", workhours);
    query.bindValue(":assigned_projects", assignedProject);
    query.bindValue(":performance_rating", performanceRating);

    return query.exec();
}

QSqlQueryModel* Employee::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM EMPLOYEES");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Phone"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Salary"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Work Hours"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Assigned Projects"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Performance Rating"));
    return model;
}

bool Employee::supprimer(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYEES WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool Employee::checkIfExists(int id) {
    QSqlQuery query;
    query.prepare("SELECT ID FROM EMPLOYEES WHERE ID = :id");
    query.bindValue(":id", id);
    return query.exec() && query.next();
}

bool Employee::modifier(int id) {
    // 1) fetch existing record
    QSqlQuery select;
    select.prepare(R"(
        SELECT NAME, PHONE, EMAIL, SALARY,
               WORK_HOURS, ASSIGNED_PROJECTS, PERFORMANCE_RATING
          FROM EMPLOYEES
         WHERE ID = :id
    )");
    select.bindValue(":id", id);
    if (!select.exec() || !select.next()) {
        qWarning() << "Employee::modifier: failed to load existing data:" 
                   << select.lastError().text();
        return false;
    }

    // 2) use original values if new ones are "empty"
    QString newName        = name.isEmpty()        ? select.value(0).toString() : name;
    QString newPhone       = phone.isEmpty()       ? select.value(1).toString() : phone;
    QString newEmail       = email.isEmpty()       ? select.value(2).toString() : email;
    double  newSalary      = (salary <= 0.0)       ? select.value(3).toDouble() : salary;
    double  newWorkhours   = (workhours <= 0.0)    ? select.value(4).toDouble() : workhours;
    QString newAssigned    = assignedProject.isEmpty()
                             ? select.value(5).toString() : assignedProject;
    double  newPerformance = (performanceRating <= 0.0)
                             ? select.value(6).toDouble() : performanceRating;

    // 3) update
    QSqlQuery update;
    update.prepare(R"(
        UPDATE EMPLOYEES
           SET NAME                = :name,
               PHONE               = :phone,
               EMAIL               = :email,
               SALARY              = :salary,
               WORK_HOURS          = :work_hours,
               ASSIGNED_PROJECTS   = :assigned_projects,
               PERFORMANCE_RATING  = :performance_rating
         WHERE ID = :id
    )");
    update.bindValue(":id", id);
    update.bindValue(":name", newName);
    update.bindValue(":phone", newPhone);
    update.bindValue(":email", newEmail);
    update.bindValue(":salary", newSalary);
    update.bindValue(":work_hours", newWorkhours);
    update.bindValue(":assigned_projects", newAssigned);
    update.bindValue(":performance_rating", newPerformance);

    if (!update.exec()) {
        qWarning() << "Employee::modifier: update failed:" 
                   << update.lastError().text();
        return false;
    }
    return true;
}
