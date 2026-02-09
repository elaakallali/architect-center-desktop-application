#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "dashboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_employee_clicked();
    void on_pb_client_clicked();
    void on_pb_meetings_clicked();
    void on_pb_equipment_clicked();
    void on_pb_projects_clicked();
    void on_pb_sponsors_clicked();

private:
    Ui::MainWindow *ui;
    Dashboard *dashboard;
};

#endif // MAINWINDOW_H