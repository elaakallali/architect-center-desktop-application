#include "dashboard.h"
#include "ui_dashboard.h"
#include "mainwindow.h"
#include "employee.h"
#include "client.h"
#include "meetings.h"
#include "equipment.h"
#include "projects.h"
#include "sponsors.h"
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);

    // Create a stacked widget to hold different module interfaces
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Create buttons for each module
    QPushButton *employeeButton = new QPushButton("Employee Module", this);
    QPushButton *clientButton = new QPushButton("Client Module", this);
    QPushButton *meetingsButton = new QPushButton("Meetings", this);
    QPushButton *equipmentButton = new QPushButton("Equipment", this);
    QPushButton *projectsButton = new QPushButton("Projects", this);
    QPushButton *sponsorsButton = new QPushButton("Sponsors", this);

    // Create layouts for the sidebar
    QVBoxLayout *sidebarLayout = new QVBoxLayout();
    sidebarLayout->addWidget(employeeButton);
    sidebarLayout->addWidget(clientButton);
    sidebarLayout->addWidget(meetingsButton);
    sidebarLayout->addWidget(equipmentButton);
    sidebarLayout->addWidget(projectsButton);
    sidebarLayout->addWidget(sponsorsButton);

    // Create a widget for the sidebar
    QWidget *sidebarWidget = new QWidget(this);
    sidebarWidget->setLayout(sidebarLayout);
    sidebarWidget->setFixedWidth(200); // Set a fixed width for the sidebar

    // Add the sidebar to the main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);

    // Create instances of each module
    Employee *employeeModule = new Employee();
    Client *clientModule = new Client();
    Meetings *meetingsModule = new Meetings();
    Equipment *equipmentModule = new Equipment();
    Projects *projectsModule = new Projects();
    Sponsors *sponsorsModule = new Sponsors();

    // Add modules to the stacked widget
    stackedWidget->addWidget(employeeModule);
    stackedWidget->addWidget(clientModule);
    stackedWidget->addWidget(meetingsModule);
    stackedWidget->addWidget(equipmentModule);
    stackedWidget->addWidget(projectsModule);
    stackedWidget->addWidget(sponsorsModule);

    // Connect buttons to switch between modules
    connect(employeeButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(employeeModule);
    });
    connect(clientButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(clientModule);
    });
    connect(meetingsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(meetingsModule);
    });
    connect(equipmentButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(equipmentModule);
    });
    connect(projectsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(projectsModule);
    });
    connect(sponsorsButton, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(sponsorsModule);
    });
}

Dashboard::~Dashboard()
{
    delete ui;
}