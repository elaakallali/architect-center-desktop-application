#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "employee.h"
#include "client.h"
#include "meetings.h"
#include "equipment.h"
#include "projects.h"
#include "sponsors.h"

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard();

private slots:
    void showEmployeeModule();
    void showClientModule();
    void showMeetingsModule();
    void showEquipmentModule();
    void showProjectsModule();
    void showSponsorsModule();

private:
    QStackedWidget *stackedWidget;
    Employee *employeeModule;
    Client *clientModule;
    Meetings *meetingsModule;
    Equipment *equipmentModule;
    Projects *projectsModule;
    Sponsors *sponsorsModule;

    void setupUI();
};

#endif // DASHBOARD_H