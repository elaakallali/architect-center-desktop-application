

#include "employee.h"
#include "connection.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QKeyEvent>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QVariant>
#include <QSqlRecord>
#include <QPdfWriter>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>

#include <QVBoxLayout>
#include <QtCharts/QPieSeries>   // Add this include at the top along with your other QtCharts includes
#include <QApplication>
// sponsor
#include "mainwindow.h"
#include "qrcode.h"
#include "ui_mainwindow.h"
#include "sponsor.h"



#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


#include <QString>


#include <QSqlError>

#include <QVBoxLayout>
#include <QFileDialog>
#include <QPdfWriter>

#include <QPageSize>
//using namespace QtCharts;

#include <QUrl>
#include <QDesktopServices>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "geminiai.h"

#include <qtranslator.h>
#include <QApplication>
#include <QFile>
#include <QTextStream>

// client

#include "mainwindow.h"
#include "keycardmanager.h"
#include "stats.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>

#include <QByteArray>


#include "qrcode.h"
#include "equipment.h"
#include "equipment_bluetooth.h"
#include <fstream>


KeyCardManager  *keyCardManager;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    equipment(new Equipment),
    proxy(new QSortFilterProxyModel(this)),
    bt(new EquipmentBluetooth(this)),
    employee(new Employee()),
    darkTheme(true),
    client(new Client())
{
    ui->setupUi(this);

    geminiHandler = new GeminiHandler(this);
    connect(geminiHandler, &GeminiHandler::responseReceived, this, [=](const QString &reply) {
        ui->chatResponseTextEdit->append(reply);
    });
    // capture whatever style you defined in mainwindow.ui
    baseStyleSheet = ui->centralwidget->styleSheet();

    // Database connection, event filters etc.
    Connection conn;
    if (!conn.createconnect()) {
        QMessageBox::critical(this, tr("Database Error"), tr("Failed to connect to the database."));
    } else {
        qDebug() << "Connection established!";
    }

    ui->name->installEventFilter(this);
    ui->email->installEventFilter(this);
    ui->phone->installEventFilter(this);
    ui->performance_rating->installEventFilter(this);




    connect(ui->pb_afficher, &QPushButton::clicked, this, &MainWindow::displayTable);

    // New connections for search, export, stats and clear
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::on_lineEditSearch_textChanged);
    connect(ui->pb_exportPdf, &QPushButton::clicked, this, &MainWindow::on_pb_exportPdf_clicked);
    connect(ui->pb_showStats, &QPushButton::clicked, this, &MainWindow::on_pb_showStats_clicked);
    connect(ui->pb_clearForm, &QPushButton::clicked, this, &MainWindow::on_pb_clearForm_clicked);
    connect(ui->toggletheme, &QPushButton::clicked, this, &MainWindow::on_actionToggleTheme_triggered);

    // Connect UI elements to slots
    connect(ui->pb_afficher_s, &QPushButton::clicked, this, &::MainWindow::displayTable_S);
    connect(ui->trier_id, &QPushButton::clicked, this, &MainWindow::on_pushButton_trier_id_clicked);
    connect(ui->trier_mount, &QPushButton::clicked, this, &MainWindow::on_pushButton_trier_mount_clicked);
    connect(ui->trier_name, &QPushButton::clicked, this, &MainWindow::on_pushButton_trier_name_clicked);
    connect(ui->recherche, &QPushButton::clicked, this, &MainWindow::on_pushButton_recherche_clicked);
    connect(ui->statistique_organisation, &QPushButton::clicked, this, &MainWindow::on_pushButton_statistique_organisation_clicked);
    connect(ui->exporter_p, &QPushButton::clicked, this, &MainWindow::on_pushButton_exporter_p_clicked);
    connect(ui->btnDarkMode, &QPushButton::clicked, this, &MainWindow::on_btnDarkMode_clicked);

    // client connection

    smtp = new SmtpClient("smtp.gmail.com", 465, "ela.kallali@gmail.com", "jfyx ywwg ksco radf", this);


    connect(ui->add, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(ui->view, &QPushButton::clicked, this, &MainWindow::onViewClicked);
    connect(ui->update, &QPushButton::clicked, this, &MainWindow::onUpdateClicked);
    connect(ui->delete_client, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->selectContractButton, &QPushButton::clicked, this, &MainWindow::onSelectContractClicked);
    connect(ui->exportList, &QPushButton::clicked, this, &MainWindow::onExportButtonClicked);
    connect(ui->search, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(ui->sortButton, &QPushButton::clicked, this, &MainWindow::onSortClicked);
    connect(ui->statsButton, &QPushButton::clicked, this, &MainWindow::onStatsClicked);
    connect(ui->emailButton, &QPushButton::clicked, this, &MainWindow::onEmailClicked);
    connect(ui->history, &QPushButton::clicked, this, &MainWindow::onShowHistoryClicked);
    keyCardManager = new KeyCardManager(this);
    if (keyCardManager->connectSerial("COM3")) { // your port name
        qDebug() << "Serial connected!";
        connect(keyCardManager, &KeyCardManager::uidReceived, this, &MainWindow::handleUID);
    } else {
        qDebug() << "Failed to connect to Serial!";
    }


    displayTable();
    ui->tab_employee_view->setSortingEnabled(true);  // Enable sorting

    // first apply dark using just overrides
    applyDarkTheme();

    // wire up navList → stackedWidget & stats
    connect(ui->navList, &QListWidget::currentRowChanged,
            this, &MainWindow::on_navList_currentRowChanged);

    // if you want the stats on startup:
    updateDashboardStats();


    //equipment

    connect(ui->pb_afficher_2, &QPushButton::clicked, this, &MainWindow::displayTable_E);
    connect(ui->pb_ajouter_2, &QPushButton::clicked, this, &MainWindow::on_pb_ajouter_clicked_E);
    connect(ui->pb_modifier_2, &QPushButton::clicked, this, &MainWindow::on_pb_modifier_clicked_E);
    connect(ui->pb_supprimer_2, &QPushButton::clicked, this, &MainWindow::on_pb_supprimer_clicked_E);
    connect(ui->searchLineEdit_2, &QLineEdit::textChanged, this, &MainWindow::onSearchClicked_E);
    connect(ui->pb_sort_2, &QPushButton::clicked, this, &MainWindow::onSortClicked_E);
    connect(ui->pb_export_2, &QPushButton::clicked, this, &MainWindow::onExportButtonClicked_E);
    connect(ui->QR_2, &QPushButton::clicked, this, &MainWindow::on_QR_clicked_E);
    connect(ui->pb_stats_2, &QPushButton::clicked, this, &MainWindow::afficherStatistiques_E);
    connect(ui->pb_sort_status_2, &QPushButton::clicked, this, &MainWindow::onSortByStatusClicked_E);

    // Open COM port for your HC‑05/06 module; change COMx as needed:
    if (bt->connectToBluetooth("COM5")) {
        connect(bt, &EquipmentBluetooth::dataReceived,
                this, &MainWindow::onBluetoothDataReceived_E);
        qDebug() << "Passed:";

    }

    displayTable_E();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete employee;
     delete client;
     delete sponsor;
}

void MainWindow::on_pb_ajouter_clicked() {
    // For add, ID is auto-assigned; no need to read ui->id
    // Validate Name
    QString name = ui->name->text();
    QRegularExpression nameRegex("^[A-Za-z ]+$");
    if (name.isEmpty() || !nameRegex.match(name).hasMatch()) {
        QMessageBox::warning(this, tr("Invalid Name"), tr("The name must contain only letters and spaces."), QMessageBox::Ok);
        return;
    }

    // Validate Phone (basic validation: non-empty)
    QString phone = ui->phone->text();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid Phone"), tr("Please enter a valid phone number."), QMessageBox::Ok);
        return;
    }

    // Validate Email
    QString email = ui->email->text();
    QRegularExpression emailRegex("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("Invalid Email"), tr("Please enter a valid email address."), QMessageBox::Ok);
        return;
    }

    // Validate Salary
    bool salaryValid;
    double salary = ui->salary->text().toDouble(&salaryValid);
    if (!salaryValid || salary <= 0) {
        QMessageBox::warning(this, tr("Invalid Salary"), tr("Salary must be a positive number."), QMessageBox::Ok);
        return;
    }

    // Validate Work Hours (numeric conversion)
    bool workhoursValid;
    double workhours = ui->workhours->text().toDouble(&workhoursValid);
    if (!workhoursValid || workhours <= 0) {
        QMessageBox::warning(this, tr("Invalid Work Hours"), tr("Work hours must be a positive number."), QMessageBox::Ok);
        return;
    }

    // Assigned project (non-empty)
    QString assignedProject = ui->assigned_project->text();
    if (assignedProject.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid Assigned Projects number"), tr("Please enter a number above 1."), QMessageBox::Ok);
        return;
    }

    // Validate Performance Rating (should be numeric and <= 10)
    bool ratingValid;
    double performanceRating = ui->performance_rating->text().toDouble(&ratingValid);
    if (!ratingValid || performanceRating < 0 || performanceRating > 10) {
        QMessageBox::warning(this, tr("Invalid Performance Rating"), tr("Performance rating must be between 0 and 10."), QMessageBox::Ok);
        return;
    }

    Employee newEmployee(0, name, phone, email, salary, workhours, assignedProject, performanceRating);
    bool test = newEmployee.ajouter();

    if (test) {
        QMessageBox::information(this, tr("Success"), tr("Employee added successfully."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to add employee."), QMessageBox::Ok);
    }
}

void MainWindow::on_pb_supprimer_clicked() {
    int id = ui->id->text().toInt();
    if (!employee->checkIfExists(id)) {
        QMessageBox::critical(this, tr("Error"), tr("Employee not found."), QMessageBox::Ok);
        return;
    }
    bool test = employee->supprimer(id);
    if (test) {
        QMessageBox::information(this, tr("Success"), tr("Employee deleted successfully."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to delete employee."), QMessageBox::Ok);
    }
}

void MainWindow::on_pb_modifier_clicked() {
    bool idValid;
    int id = ui->id->text().toInt(&idValid);
    if (!idValid || id <= 0) {
        QMessageBox::warning(this, tr("Invalid ID"), tr("ID must be a positive integer."), QMessageBox::Ok);
        return;
    }
    if (!employee->checkIfExists(id)) {
        QMessageBox::critical(this, tr("Error"), tr("Employee not found."), QMessageBox::Ok);
        return;
    }

    // Validate Name
    QString name = ui->name->text();
    QRegularExpression nameRegex("^[A-Za-z ]+$");
    

    QString phone = ui->phone->text();
    

    QString email = ui->email->text();
    QRegularExpression emailRegex("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    

    bool salaryValid;
    double salary = ui->salary->text().toDouble(&salaryValid);
    

    bool workhoursValid;
    double workhours = ui->workhours->text().toDouble(&workhoursValid);
    

    QString assignedProject = ui->assigned_project->text();
    

    bool ratingValid;
    double performanceRating = ui->performance_rating->text().toDouble(&ratingValid);
    

    employee->setId(id);
    employee->setName(name);
    employee->setPhone(phone);
    employee->setEmail(email);
    employee->setSalary(salary);
    employee->setWorkhours(workhours);
    employee->setAssignedProject(assignedProject);
    employee->setPerformanceRating(performanceRating);

    bool test = employee->modifier(id);
    if (test) {
        QMessageBox::information(this, tr("Success"), tr("Employee modified successfully."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to modify employee."), QMessageBox::Ok);
    }
}

void MainWindow::on_tab_employee_clicked(const QModelIndex &index) {
    int row = index.row();
    ui->id->setText(ui->tab_employee_view->model()->index(row, 0).data().toString());
    ui->name->setText(ui->tab_employee_view->model()->index(row, 1).data().toString());
    ui->phone->setText(ui->tab_employee_view->model()->index(row, 2).data().toString());
    ui->email->setText(ui->tab_employee_view->model()->index(row, 3).data().toString());
    ui->salary->setText(ui->tab_employee_view->model()->index(row, 4).data().toString());
    double wh = ui->tab_employee_view->model()->index(row, 5).data().toDouble();
    ui->workhours->setText(QString::number(wh));
    ui->assigned_project->setText(ui->tab_employee_view->model()->index(row, 6).data().toString());
    ui->performance_rating->setText(ui->tab_employee_view->model()->index(row, 7).data().toString());
}

void MainWindow::displayTable() {
    QSqlQueryModel *model = employee->afficher();
    ui->tab_employee_view->setModel(model);
}

// ----- New feature implementations -----

// Search employees by filtering the table
void MainWindow::on_lineEditSearch_textChanged(const QString &text) {
    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    // This example filters by name containing the search text (adjust as needed)
    query.prepare("SELECT * FROM EMPLOYEES WHERE UPPER(NAME) LIKE UPPER(:search)");
    query.bindValue(":search", "%" + text + "%");
    query.exec();
    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, tr("Phone"));
    model->setHeaderData(3, Qt::Horizontal, tr("Email"));
    model->setHeaderData(4, Qt::Horizontal, tr("Salary"));
    model->setHeaderData(5, Qt::Horizontal, tr("Work Hours"));
    model->setHeaderData(6, Qt::Horizontal, tr("Assigned Projects"));
    model->setHeaderData(7, Qt::Horizontal, tr("Performance Rating"));
    ui->tab_employee_view->setModel(model);
}

// Export table to PDF.
void MainWindow::on_pb_exportPdf_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) return;

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&pdfWriter);
    painter.setFont(QFont("Arial", 10));

    int y = 1500;  // starting y position
    int rowHeight = 1200;
    int pageHeight = pdfWriter.height();

    // Draw headers
    painter.drawText(500, 1000, "ID");
    painter.drawText(1500, 1000, "Name");
    painter.drawText(2500, 1000, "Type");
    painter.drawText(3500, 1000, "Status");
    painter.drawLine(500, 1100, 4500, 1100);

    // Get the current model from the table view.
    QAbstractItemModel *model = ui->tab_employee_view->model();
    for (int row = 0; row < model->rowCount(); ++row) {
        if (y + rowHeight > pageHeight) {
            pdfWriter.newPage();
            y = 1000;
        }

        painter.drawText(500, y, model->index(row, 0).data().toString());
        painter.drawText(1500, y, model->index(row, 1).data().toString());
        painter.drawText(2500, y, model->index(row, 2).data().toString());
        painter.drawText(3500, y, model->index(row, 3).data().toString());
        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, "Exported", "PDF created successfully.");
}

// Show statistics (for example, average salary and total employees)
void MainWindow::on_pb_showStats_clicked() {
    QSqlQuery query;
    QPieSeries *series = new QPieSeries();

    // Query each employee's name and salary.
    if (query.exec("SELECT NAME, SALARY FROM EMPLOYEES")) {
        while(query.next()){
            QString name = query.value("NAME").toString();
            double salary = query.value("SALARY").toDouble();
            // Add a slice for each employee.
            series->append(name, salary);
        }
    }

    // Turn on labels so that each slice shows its percentage.
    series->setLabelsVisible(true);
    // Update each slice label to include the percentage.
    for (QPieSlice *slice : series->slices()) {
        // The percentage is automatically calculated when chart is shown.
        // Multiply by 100 and format to one decimal place.
        slice->setLabel(QString("%1: %2%")
                            .arg(slice->label())
                            .arg(slice->percentage() * 100, 0, 'f', 1));
        slice->setLabelVisible(true);
    }

    // Create the chart and configure it.
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Employee Salary Distribution");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    // Create a chart view to display it.
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Display the chart in a dialog.
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Salary Distribution");
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(chartView);
    dialog->resize(600, 400);
    dialog->exec();
}
void MainWindow::on_pb_clearForm_clicked() {
    ui->id->clear();
    ui->name->clear();
    ui->phone->clear();
    ui->email->clear();
    ui->salary->clear();
    ui->workhours->clear();
    ui->assigned_project->clear();
    ui->performance_rating->clear();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        // Additional event handling if needed.
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_actionToggleTheme_triggered() {
    darkTheme = !darkTheme;
    if (darkTheme) applyDarkTheme();
    else           applyLightTheme();
}

void MainWindow::applyDarkTheme()
{
    // only override the widgets you want—everything else remains as in baseStyleSheet
    const QString overrides = R"(
      QPushButton {
        background-color: #FFD700; color: #1E1E1E;
      }
      QPushButton:hover {
        background-color: #FFC107;
      }
    )";
    qApp->setStyleSheet(baseStyleSheet + overrides);
}

void MainWindow::applyLightTheme()
{
    // again, just override button colors; all other rules stay untouched
    const QString overrides = R"(
      QPushButton {
        background-color: #1E1E1E; color: #FFD700;
      }
      QPushButton:hover {
        background-color: #333333;
      }
    )";
    qApp->setStyleSheet(baseStyleSheet + overrides);
}

void MainWindow::on_navList_currentRowChanged(int row)
{
    ui->stackedWidget->setCurrentIndex(row);
    if (row == 0)       // Dashboard is row 0
        updateDashboardStats();
}

void MainWindow::updateDashboardStats()
{
    QSqlQuery q;
    int empCount = 0, projCount = 0;
    double totalSalary = 0.0;

    // 1) count employees
    if(!q.exec("SELECT COUNT(*) FROM EMPLOYEES")) {
        qDebug() << "EMPLOYEE COUNT ERROR:" << q.lastError().text();
    } else if(q.next()) {
        empCount = q.value(0).toInt();
    }

    // 2) count projects
    if(!q.exec("SELECT COUNT(*) FROM PROJECTS")) {
        qDebug() << "PROJECT COUNT ERROR:" << q.lastError().text();
    } else if(q.next()) {
        projCount = q.value(0).toInt();
    }

    // 3) sum salaries
    if(!q.exec("SELECT SUM(SALARY) FROM EMPLOYEES")) {
        qDebug() << "SUM SALARY ERROR:" << q.lastError().text();
    } else if(q.next()) {
        totalSalary = q.value(0).toDouble();
    }

    ui->lblEmployeeCountValue->setText(QString::number(empCount));
    ui->lblProjectCountValue ->setText(QString::number(projCount));
    ui->lblTotalSalaryValue  ->setText(QString::number(totalSalary, 'f', 2));
}
// sponsors

void MainWindow::on_pb_ajouter_s_clicked() {
    // Validate ID
    bool idValid;
    int id = ui->id_2->text().toInt(&idValid);
    /*if (!idValid || id <= 0) {
        QMessageBox::warning(this, tr("ID invalid"), tr("ID Must be a positive integer."), QMessageBox::Ok);

        return;
    }*/

    // Validate Name
    QString name = ui->name_3->text();
    QRegularExpression nameRegex("^[A-Za-z ]+$");
    if (name.isEmpty() || !nameRegex.match(name).hasMatch()) {
        QMessageBox::warning(this, tr("Invalid Name"), tr("Le nom ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }

    // Validate organization
    QString organization = ui->organization_2->text();
    QRegularExpression organizationRegex("^[A-Za-z ]+$");
    if (organization.isEmpty() || !organizationRegex.match(organization).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le organization ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }

    // Validate email
    QString email = ui->email_5->text();
    QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,4}$");
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("L'email n'est pas valide."), QMessageBox::Ok);
        return;
    }

    // Validate phone number
    QString phone = ui->phone_3->text();
    QRegularExpression phoneRegex("^\\d{8,15}$");
    if (phone.isEmpty() || !phoneRegex.match(phone).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le numéro de téléphone doit être valide."), QMessageBox::Ok);
        return;
    }

    // Validate contributionAmount
    bool contribution_amountValid;
    double contribution_amount = ui->contributionAmount_2->text().toDouble(&contribution_amountValid);
    if (!contribution_amountValid || contribution_amount <= 0) {
        QMessageBox::warning(this, tr("Contribution Amount invalide"), tr("Le montant de la contribution doit être un nombre valide."), QMessageBox::Ok);
        return;
    }

    // Validate contract details (if required)
    QString contract_details = ui->contractDetails_2->text();
    if (contract_details.isEmpty()) {
        QMessageBox::warning(this, tr("Détails du contrat invalides"), tr("Les détails du contrat ne peuvent pas être vides."), QMessageBox::Ok);
        return;
    }

    // Validate renewal date
    QString renewal_date = ui->renewalDate_2->text();
    QRegularExpression renewal_dateRegex("^\\d{4}-\\d{2}-\\d{2}$");  // Date format yyyy-mm-dd
    if (renewal_date.isEmpty() /*|| !renewal_dateRegex.match(renewal_date).hasMatch()*/) {
        QMessageBox::warning(this, tr("Date de renouvellement invalide"), tr("La date de renouvellement doit être au format yyyy-mm-dd."), QMessageBox::Ok);
        return;
    }

    // Create a new Sponsor object and try to add it to the database
    Sponsor newSponsor(id, name, organization , email, phone, contribution_amount, contract_details , renewal_date);
    bool test = newSponsor.ajouter();

    if (test) {
        QMessageBox::information(this, tr("Success"), tr("Sponsor ajouté avec succès."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("L'ajout du sponsor a échoué."), QMessageBox::Ok);
    }
}


void MainWindow::on_pb_supprimer_s_clicked() {
    int id = ui->id->text().toInt();


    if (!sponsor->checkIfExists(id)) {
        QMessageBox::critical(this, tr("Erreur"), tr("sponsor not found."), QMessageBox::Ok);
        return;
    }


    bool test = sponsor->supprimer(id);

    if (test) {
        QMessageBox::information(this, tr("Success"), tr("sponsor Deleted Successfully."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("sponsor Couldn't Be Deleted."), QMessageBox::Ok);
    }
}

void MainWindow::on_pb_modifier_s_clicked() {
    // Validate ID
    bool idValid;
    int id = ui->id_2->text().toInt(&idValid);
    /* if (!idValid || id <= 0) {
        QMessageBox::warning(this, tr("ID invalide"), tr("L'ID doit être un nombre entier positif."), QMessageBox::Ok);
        return;
    }*/

    // Validate Name
    QString name = ui->name_3->text();
    QRegularExpression nameRegex("^[A-Za-z ]+$");  // Use QRegularExpression
    if (name.isEmpty() || !nameRegex.match(name).hasMatch()) {
        QMessageBox::warning(this, tr("Nom invalide"), tr("Le nom ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }


    // Validate organization
    QString organization = ui->organization_2->text();
    QRegularExpression organizationRegex("^[A-Za-z ]+$");  // Use QRegularExpression
    if (organization.isEmpty() || !organizationRegex.match(organization).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le organization ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }

    QString email = ui->email_5->text();
    QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,4}$");
        // Use QRegularExpression
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le email ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }

    QString phone = ui->phone_3->text();
    QRegularExpression phoneRegex("^\\d{8,15}$");  // Allows 8-15 digits
        // Use QRegularExpression
    if (phone.isEmpty() || !phoneRegex.match(phone).hasMatch()) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le phone ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }

    // Validate contributionAmount
    bool contribution_amountValid;
    double contribution_amount = ui->contributionAmount_2->text().toInt(&contribution_amountValid);
    /*if (!contribution_amountValid || id <= 0) {
        QMessageBox::warning(this, tr(" contributionAmount invalide"), tr("L'contributionAmount doit être un nombre entier positif."), QMessageBox::Ok);
        return;
    }*/

    QString contract_details = ui->contractDetails_2->text();
    QRegularExpression contract_detailsRegex("^[A-Za-z ]+$");  // Use QRegularExpression
    if (contract_details.isEmpty() /*|| !contract_detailsRegex.match(contract_details).hasMatch()*/) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le contract_details ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }
    QString renewal_date = ui->renewalDate_2->text();
    QRegularExpression renewal_dateRegex("^[A-Za-z ]+$");  // Use QRegularExpression
    if (renewal_date.isEmpty() /*||!renewal_dateRegex.match(renewal_date).hasMatch()*/) {
        QMessageBox::warning(this, tr("Type invalide"), tr("Le renewal_date ne doit contenir que des lettres et des espaces."), QMessageBox::Ok);
        return;
    }


    // Update the sponsor
    sponsor->setId(id);
    sponsor->setName(name);
    sponsor->setOrganization(organization);
    sponsor->setEmail(email);
    sponsor->setPhone(phone);
    sponsor->setPhone(phone);
    sponsor->setContributionAmount(contribution_amount);
    sponsor->setContractDetails(contract_details);
    sponsor->setRenewalDate(renewal_date);

    bool test = sponsor->modifier(id);

    if (test) {
        QMessageBox::information(this, tr("Success"), tr("modification Was Successful."), QMessageBox::Ok);
        displayTable();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Modification Error."), QMessageBox::Ok);
    }
}

void MainWindow::on_tab_equipement_clicked(const QModelIndex &index) {
    int row = index.row();
    ui->id->setText(ui->table_sponsor->model()->index(row, 0).data().toString());
    ui->name->setText(ui->table_sponsor->model()->index(row, 1).data().toString());
    ui->organization->setText(ui->table_sponsor->model()->index(row, 2).data().toString());
    ui->email->setText(ui->table_sponsor->model()->index(row, 3).data().toString());
    ui->phone->setText(ui->table_sponsor->model()->index(row, 4).data().toString());
    ui->contributionAmount->setText(ui->table_sponsor->model()->index(row, 5).data().toString());
    ui->contractDetails->setText(ui->table_sponsor->model()->index(row, 7).data().toString());
    ui->label_id->setText(ui->table_sponsor->model()->index(row, 8).data().toString());
}

void MainWindow::displayTable_S() {
    QSqlQueryModel *model = sponsor->afficher();
    ui->table_sponsor->setModel(model);
}

bool MainWindow::eventFilter_s(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Check if the event is for the ID field
        if (obj == ui->id) {
            // Allow only digits and backspace
            if (!keyEvent->text().isEmpty() && !keyEvent->text().at(0).isDigit()) {
                QMessageBox::warning(this, tr("ID invalid"), tr("The ID must only include numbers."), QMessageBox::Ok);
                return true;  // Block the event
            }
        }

        // Check if the event is for the Name field
        if (obj == ui->name) {
            // Allow only alphabetic characters and spaces
            QRegularExpression nameRegex("^[A-Za-z ]*$");  // Use QRegularExpression
            if (!keyEvent->text().isEmpty() && !nameRegex.match(keyEvent->text()).hasMatch()) {
                QMessageBox::warning(this, tr("Invalid Name"), tr("The name must only include letters and spaces."), QMessageBox::Ok);
                return true;  // Block the event
            }
        }

        // Check if the event is for the Type field
        if (obj == ui->organization) {
            // Allow only alphabetic characters and spaces
            QRegularExpression organizationRegex("^[A-Za-z ]*$");  // Use QRegularExpression
            if (!keyEvent->text().isEmpty() && !organizationRegex.match(keyEvent->text()).hasMatch()) {
                QMessageBox::warning(this, tr("Invalid Type"), tr("The organization must only include letters and spaces"), QMessageBox::Ok);
                return true;  // Block the event
            }
        }
    }

    // Pass the event to the base class
    return QMainWindow::eventFilter(obj, event);
}




//beheee
void MainWindow::on_pushButton_trier_name_clicked() {
    Sponsor S ;
    ui->table_sponsor->setModel(S.afficherTrieParNom());
    QMessageBox::information(this, "Succès", "Tri effectué par nom dans l'ordre croissant.");
}
void MainWindow::on_pushButton_trier_id_clicked() {
    Sponsor S ;
    QSqlQueryModel* model = S.afficherTrieParId();
    ui->table_sponsor->setModel(model); // Assurez-vous que tableView est le bon nom de votre vue
    QMessageBox::information(this, "Succès", "Tri effectué par quantite de pièce dans l'ordre croissant.");
}

void MainWindow::on_pushButton_trier_mount_clicked() {
    Sponsor S ;
    QSqlQueryModel* model = S.afficherTrieParMount();
    ui->table_sponsor->setModel(model); // Assurez-vous que tableView est le bon nom de votre vue
    QMessageBox::information(this, "Succès", "Tri effectué par reference dans l'ordre croissant.");
}





void MainWindow::on_pushButton_recherche_clicked()
{
    // Récupérer la valeur saisie dans le lineEdit
    int ID = ui->lineEdit_recherche->text().toInt();

    // Construire la requête SQL
    QSqlQuery query;
    query.prepare("SELECT * FROM ela.SPONSORS WHERE ID = :id");
    query.bindValue(":id", ID);
    query.exec();

    // Créer un nouveau modèle et l'affecter au QTableView
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    ui->table_sponsor->setModel(model);


}


void MainWindow::on_pushButton_statistique_organisation_clicked()
{
    // Préparer la requête pour obtenir les données nécessaires
    QSqlQuery query;
    query.prepare("SELECT ORGANIZATION, COUNT(*) AS count FROM ela.SPONSORS GROUP BY ORGANIZATION");

    // Exécuter la requête et vérifier s'il y a des résultats
    if (!query.exec()) {
        QMessageBox::critical(this, tr("Erreur"), tr("Échec de l'obtention des statistiques : ") + query.lastError().text());
        return;
    }

    // Stocker les données dans une map pour compter les pièces par categorie
    QMap<QString, int> quantitesParOrganisation;
    while (query.next()) {
        QString organisation = query.value(0).toString();
        int count = query.value(1).toInt();
        quantitesParOrganisation[organisation] = count;
    }

    // Vérifier qu'il y a des données pour générer les statistiques
    if (quantitesParOrganisation.isEmpty()) {
        QMessageBox::information(this, tr("Aucune donnée"), tr("Aucune donnée disponible pour générer les statistiques."));
        return;
    }

    // Créer une série de camembert et remplir avec les données
    QPieSeries *series = new QPieSeries();
    int total = 0;
    for (auto count : quantitesParOrganisation.values()) {
        total += count;
    }

    // Fill the pie series with percentage labels
    for (auto it = quantitesParOrganisation.begin(); it != quantitesParOrganisation.end(); ++it) {
        QString organisation = it.key();
        int count = it.value();
        qreal percent = (static_cast<qreal>(count) / total) * 100.0;

        QString label = QString("%1 (%2%)").arg(organisation).arg(QString::number(percent, 'f', 1));
        series->append(label, count);

    }

    // Rendre les labels visibles pour chaque part de camembert
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    // Créer un objet QChart et ajouter la série de camembert
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("STAT BY ORGANISATION");
    chart->legend()->setAlignment(Qt::AlignRight);

    // Afficher le graphique dans un QChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Supprimer l'ancien layout de widget_stat (si existant)
    if (ui->widget_stat->layout() != nullptr) {
        QLayout *oldLayout = ui->widget_stat->layout();
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();  // Supprimer les widgets du layout
            delete item;
        }
        delete oldLayout;
    }

    // Ajouter un nouveau layout contenant le graphique
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_stat);
    layout->addWidget(chartView);
    ui->widget_stat->setLayout(layout);
}



void MainWindow::on_pushButton_exporter_p_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "save PDF", "", "PDF (*.pdf)");
    if (fileName.isEmpty()) {
        QMessageBox::information(this, "Annulation", " the export has been canceled.");
        return;
    }
    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(4, 4, 4, 4));
    QPainter painter(&pdfWriter);
    QImage logo("D:/ressource/logo2.png");
    QRect logoRect(30, 30, 2000, 2000);
    painter.drawImage(logoRect, logo);
    QFont titleFont("Times New Roman",15, QFont::Bold);
    painter.setFont(titleFont);
    QRect titleRect(200, logoRect.bottom()+50, pdfWriter.width() - 300, 80);
    painter.drawText(titleRect, Qt::AlignCenter, "LIST OF SPONSORS");
    QFont font("Times New Roman", 8);
    painter.setFont(font);
    QTableView *tableView = ui->table_sponsor;
    QAbstractItemModel *model = tableView->model();
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    int pageWidth = pdfWriter.width() - 60;
    QVector<int> columnWidths(columnCount);
    int totalColumnWidth = pageWidth;
    for (int col = 0; col < columnCount; ++col) {
        columnWidths[col] = totalColumnWidth / columnCount;
    }
    int x = 30;
    int y = titleRect.bottom() + 10;
    for (int col = 0; col < columnCount; ++col) {
        QRect rect(x, y, columnWidths[col], 200);
        painter.drawRect(rect);
        painter.drawText(rect, Qt::AlignCenter, model->headerData(col, Qt::Horizontal).toString());
        x += columnWidths[col];
    }
    y += 700;
    for (int row = 0; row < rowCount; ++row) {
        x = 50;
        for (int col = 0; col < columnCount; ++col) {
            QRect rect(x, y, columnWidths[col], 700);
            painter.drawRect(rect);
            QString cellText = model->data(model->index(row, col)).toString();
            painter.drawText(rect, Qt::AlignCenter, cellText);
            x += columnWidths[col];
        }
        y += 500;
    }

    painter.end();
    QMessageBox::information(this, "Succès", "pdf has been succefully saved.");
}
//
void MainWindow::onSendMessage()
{
    QString userMessage = ui->chatInputLineEdit->text();
    ui->chatInputLineEdit->clear();

    if (userMessage.isEmpty())
        return;

    ui->chatResponseTextEdit->append("You: " + userMessage);

    geminiHandler->sendMessage(userMessage);

}

void MainWindow::on_sendMessageButton_clicked()
{
    QString userMessage = ui->chatInputLineEdit->text();  // Adjust widget name
    if (!userMessage.isEmpty()) {
        geminiHandler->sendMessageToGemini(userMessage);
    }
}

void MainWindow::onResponseReceived(const QString &reply)
{
    ui->chatResponseTextEdit->append("Bot: " + reply);  // Display bot response in chat
}

void MainWindow::on_ENG_clicked()
{
    qApp->removeTranslator(&translator); // remove any existing translator
    bool loaded = translator.load("C:/Users/elaka/Desktop/test/translations/Atelier_Connexion_en.qm");
    qDebug() << "EN loaded:" << loaded;
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::on_FR_clicked()
{
    qApp->removeTranslator(&translator);
    bool loaded = translator.load("C:/Users/elaka/Desktop/test/translations/Atelier_Connexion_fr.qm");
    qDebug() << "FR loaded:" << loaded;
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::on_AR_clicked()
{
    qApp->removeTranslator(&translator);
    bool loaded = translator.load("C:/Users/elaka/Desktop/test/translations/Atelier_Connexion_ar.qm");
    qDebug() << "AR loaded:" << loaded;
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}


//dark mode
QString MainWindow::loadStyleSheet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "❌ Failed to open:" << filePath;
        return "";
    }

    QTextStream in(&file);
    return in.readAll();
}

void MainWindow::on_btnDarkMode_clicked()
{
    QString style = loadStyleSheet("C:/Users/elaka/Desktop/test/styles/dark.qss");

    if (style.isEmpty()) {
        qDebug() << "❌ Dark stylesheet not found!";
        return;
    }

    qApp->setStyleSheet("");
    qApp->setStyleSheet(style);
}
void MainWindow::on_btnLightMode_clicked()
{
    QString style = loadStyleSheet("C:/Users/elaka/Desktop/test/styles/light.qss");

    if (style.isEmpty()) {
        qDebug() << "❌ Light stylesheet not found!";
        return;
    }

    qApp->setStyleSheet("");
    qApp->setStyleSheet(style);
}

// client

void MainWindow::onSelectContractClicked() {

    QString fileName = QFileDialog::getOpenFileName(this, "Select Contract", "", "PDF Files (*.pdf)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray contractData = file.readAll();
            file.close();
            qDebug() << "Contract data size: " << contractData.size();
            contract = contractData;
            ui->contractLineEdit->setText(fileName);
        } else {
            QMessageBox::critical(this, "Error", "Failed to open the contract file.");
        }

    }
}

void MainWindow::onAddClicked() {
    int id = ui->id_clientLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QString email = ui->emailLineEdit->text();
    int phone = ui->phoneLineEdit->text().toInt();
    QString address = ui->addressLineEdit->text();
    int projects = ui->projectLineEdit->text().toInt();

    if (!client->validateId(id)) {
        QMessageBox::warning(this, "Invalid ID", "ID must be a positive number.");
        return;
    }

    if (!client->validatePhone(phone)) {
        // Message box already shown in validatePhone
        return;
    }

    if (client->checkIfExists(id)) {
        QMessageBox::warning(this, "Duplicate ID", "This ID already exists. Please enter a unique ID.");
        return;
    }

    Client newClient(id, name, email, phone, address, contract, projects);

    if (newClient.add()) {
        QMessageBox::information(this, "Success", "Client added successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to add client.");
    }
}


void MainWindow::onViewClicked() {
    int id = ui->id_clientLineEdit->text().toInt();
    QSqlQueryModel *model = client->view(id);
    ui->clientTableView->setModel(model);
}

void MainWindow::onUpdateClicked() {
    int id = ui->id_clientLineEdit->text().toInt();
    QString name = ui->nameLineEdit->text();
    QString email = ui->emailLineEdit->text();
    QString phoneStr = ui->phoneLineEdit->text();
    int phone = phoneStr.isEmpty() ? 0 : phoneStr.toInt();
    QString address = ui->addressLineEdit->text();
    int projects = ui->projectLineEdit->text().toInt();

    if (!client->validateId(id)) {
        QMessageBox::warning(this, "Invalid ID", "ID must be a positive number.");
        return;
    }

    if (phone != 0 && !client->validatePhone(phone)) {
        return; // Message shown in validatePhone
    }

    QByteArray contractToUse;

    if (!contract.isEmpty()) {

        contractToUse = contract;
    } else {

        QSqlQuery query;
        query.prepare("SELECT contract FROM clients WHERE id = :id");
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            contractToUse = query.value(0).toByteArray();
        } else {
            QMessageBox::critical(this, "Error", "Failed to retrieve existing contract from database.");
            return;
        }
    }

    Client updatedClient(id, name, email, phone, address, contractToUse, projects);

    if (updatedClient.update(id)) {
        QMessageBox::information(this, "Success", "Client updated successfully.");
        selectedContract.clear();
    } else {
        QMessageBox::critical(this, "Error", "Failed to update client.");
    }
}


void MainWindow::onDeleteClicked() {
    int id = ui->id_clientLineEdit->text().toInt();

    if (client->deleteClient(id)) {

        QMessageBox::information(this, "Success", "Client deleted successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete client.");
    }
}

void MainWindow::onExportButtonClicked() {
    if (client->exportAllToPdf()) {
        QMessageBox::information(this, "Success", "All clients exported to PDF successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Export failed.");
    }
}

void MainWindow::onSearchClicked() {
    QString name = ui->searchLineEdit->text();
    QSqlQueryModel *model = client->searchByName(name);
    ui->clientTableView->setModel(model);
}

void MainWindow::onSortClicked() {
    QString selectedOption = ui->sort->currentText();
    QString orderByClause;

    if (selectedOption == "Name A-Z") {
        orderByClause = "ORDER BY name ASC";
    } else if (selectedOption == "Name Z-A") {
        orderByClause = "ORDER BY name DESC";
    } else if (selectedOption == "Email A-Z") {
        orderByClause = "ORDER BY email ASC";
    } else if (selectedOption == "Email Z-A") {
        orderByClause = "ORDER BY email DESC";
    } else if (selectedOption == "ID Ascending") {
        orderByClause = "ORDER BY id ASC";
    } else if (selectedOption == "ID Descending") {
        orderByClause = "ORDER BY id DESC";
    }

    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery query;
    query.prepare("SELECT id, name, email, phone, address, projects FROM clients " + orderByClause);
    query.exec();
    model->setQuery(query);

    ui->clientTableView->setModel(model);
}

void MainWindow::onStatsClicked() {
    stats stats(this);
    stats.exec();
}


void MainWindow::onEmailClicked() {

    int clientId = ui->id_clientLineEdit->text().toInt();


    QString clientEmail = client->getClientEmailFromDatabase(clientId);

    if (clientEmail.isEmpty()) {
        QMessageBox::warning(this, "Error", "Client not found or has no email");
        return;
    }


    QString pdfPath = QFileDialog::getOpenFileName(this,
                                                   "Select Contract PDF",
                                                   "",
                                                   "PDF Files (*.pdf)");

    if (pdfPath.isEmpty()) {
        QMessageBox::information(this, "Canceled", "No PDF file selected.");
        return;
    }


    QFile file(pdfPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Could not open PDF file");
        return;
    }
    QByteArray pdfData = file.readAll();
    file.close();


    if (client->sendEmail(clientEmail, pdfData,
                          "Your Contract",
                          "Hello, we are happy to have you as our client. Please find attached your contract.")) {
        QMessageBox::information(this, "Success", "Email sent successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Failed to send email");
    }
}


void MainWindow::onShowHistoryClicked() {
    int clientId = ui->id_clientLineEdit->text().toInt();


    QSqlQuery query;
    query.prepare("SELECT ACTIONS, DATETIME, ADDITIONAL_INFO FROM CLIENTHISTORY WHERE client_id = :client_id ORDER BY DATETIME DESC");
    query.bindValue(":client_id", clientId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to fetch activity history: " + query.lastError().text());
        return;
    }


    QStandardItemModel *historyModel = new QStandardItemModel();
    historyModel->setColumnCount(3);
    historyModel->setHeaderData(0, Qt::Horizontal, "Action");
    historyModel->setHeaderData(1, Qt::Horizontal, "Details");
    historyModel->setHeaderData(2, Qt::Horizontal, "Timestamp");

    int row = 0;
    while (query.next()) {
        historyModel->setItem(row, 0, new QStandardItem(query.value(0).toString()));  // action
        historyModel->setItem(row, 1, new QStandardItem(query.value(1).toString()));  // details
        historyModel->setItem(row, 2, new QStandardItem(query.value(2).toString()));  // timestamp
        row++;
    }


    ui->clientTableView->setModel(historyModel);
    ui->clientTableView->setColumnWidth(0, 100);  // Client ID
    ui->clientTableView->setColumnWidth(1, 120);  // Action
    ui->clientTableView->setColumnWidth(2, 300);  // Details
    ui->clientTableView->setColumnWidth(3, 180);  // Timestamp

}

void MainWindow::handleUID(const QString &uid)
{
    qDebug() << "UID received:" << uid;
    if (uid.length() != 8 || !uid.contains(QRegularExpression("^[0-9A-Fa-f]{8}$"))) {
        qDebug() << "Ignored non-UID input";
        return;}

    QSqlQuery query;
    query.prepare("SELECT ID, NAME, PHONE , EMAIL, SALARY, WORK_HOURS, ASSIGNED_PROJECTS, PERFORMANCE_RATING FROM EMPLOYEES WHERE UID_ID = :uid");
    query.bindValue(":uid", uid);

    if (query.exec() && query.next()) {
        int id = query.value(0).toInt(); // ID
        QString name = query.value(1).toString(); // NAME
        int phone = query.value(2).toInt(); // PHONE
        QString email = query.value(3).toString(); // EMAIL
        int salary = query.value(4).toInt(); // SALARY
        int work_hours = query.value(5).toInt(); // WORK_HOURS
        QString assigned_projects = query.value(6).toString(); // ASSIGNED_PROJECTS
        int performance_rating = query.value(7).toInt(); // PERFORMANCE_RATING


        QString result = QString("Access Granted!\nID: %1\nName: %2\nPhone: %3\nEmail: %4\nSalary: %5\nAssigned Projects: %6\nWork Hours: %7\nPerformance Rating: %8")
                             .arg(id)
                             .arg(name)
                             .arg(phone)
                             .arg(email)
                             .arg(salary)
                             .arg(assigned_projects)
                             .arg(work_hours)
                             .arg(performance_rating);


        ui->textEditEmployee->setText(result);
        qDebug() << "Employee found:" << result;

    } else {
        ui->textEditEmployee->setText("Access Denied!\nCard not registered.");
    }
}

//equipment


void MainWindow::displayTable_E()
{
    QSqlQueryModel *model = equipment->afficher();
    proxy->setSourceModel(model);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy->setFilterKeyColumn(1); // Filter by name column
    ui->tab_equipement_2->setModel(proxy);
}


void MainWindow::on_pb_ajouter_clicked_E()
{
    int id = ui->id_3->text().toInt();
    QString name = ui->name_4->text();
    QString type = ui->type_2->text();
    QString status = ui->comboBox_status_2->currentText();

    Equipment eq(id, name, type, status);

    if (eq.ajouter()) {
        QMessageBox::information(this, "Success", "Equipment added.");
        displayTable_E();
    } else {
        QMessageBox::warning(this, "Error", "Add failed.");
    }
}

void MainWindow::on_pb_modifier_clicked_E()
{
    int id = ui->id_3->text().toInt();
    QString name = ui->name_4->text();
    QString type = ui->type_2->text();
    QString status = ui->comboBox_status_2->currentText();

    if (!equipment->checkIfExists(id)) {
        QMessageBox::warning(this, "Not Found", "ID not found.");
        return;
    }

    equipment->setId(id);
    equipment->setName(name);
    equipment->setType(type);
    equipment->setStatus(status);

    if (equipment->modifier(id)) {
        QMessageBox::information(this, "Success", "Equipment updated.");
        displayTable_E();
    } else {
        QMessageBox::warning(this, "Error", "Update failed.");
    }
}

void MainWindow::on_pb_supprimer_clicked_E()
{
    int id = ui->id_3->text().toInt();

    if (!equipment->checkIfExists(id)) {
        QMessageBox::warning(this, "Not Found", "ID not found.");
        return;
    }

    if (equipment->supprimer(id)) {
        QMessageBox::information(this, "Success", "Equipment deleted.");
        displayTable_E();
    } else {
        QMessageBox::warning(this, "Error", "Delete failed.");
    }
}

void MainWindow::on_pb_afficher_clicked_E()
{
    displayTable_E();
}

void MainWindow::on_tab_equipement_clicked_E(const QModelIndex &index)
{
    int row = index.row();
    ui->id->setText(proxy->index(row, 0).data().toString());
    ui->name->setText(proxy->index(row, 1).data().toString());
    ui->type_2->setText(proxy->index(row, 2).data().toString());
    ui->comboBox_status_2->setCurrentText(proxy->index(row, 3).data().toString());
}

void MainWindow::onSearchClicked_E()
{
    applySearchFilter();
}

void MainWindow::applySearchFilter()
{
    QString searchText = ui->searchLineEdit->text();
    QRegularExpression regex(QRegularExpression::escape(searchText), QRegularExpression::CaseInsensitiveOption);

    proxy->setFilterKeyColumn(-1); // Search across all columns
    proxy->setFilterRegularExpression(regex);

}

void MainWindow::onSortClicked_E()
{
    proxy->sort(1, Qt::AscendingOrder);
}

void MainWindow::onSortByStatusClicked_E()
{
    proxy->sort(3, Qt::AscendingOrder);  // 3 = index of the 'status' column
}

void MainWindow::onExportButtonClicked_E()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Export PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) return;

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&pdfWriter);
    painter.setFont(QFont("Arial", 10));

    int y = 1500;  // 1cm = 377.95 units (300 DPI)
    int rowHeight = 1200;
    int pageHeight = pdfWriter.height();

    painter.drawText(500, 1000, "ID");
    painter.drawText(1500, 1000, "Name");
    painter.drawText(2500, 1000, "Type");
    painter.drawText(3500, 1000, "Status");
    painter.drawLine(500, 1100, 4500, 1100);

    QAbstractItemModel *model = proxy;
    for (int row = 0; row < model->rowCount(); ++row) {
        if (y + rowHeight > pageHeight) {
            pdfWriter.newPage();
            y = 1000;
        }

        painter.drawText(500, y, model->index(row, 0).data().toString());
        painter.drawText(1500, y, model->index(row, 1).data().toString());
        painter.drawText(2500, y, model->index(row, 2).data().toString());
        painter.drawText(3500, y, model->index(row, 3).data().toString());
        y += rowHeight;
    }

    painter.end();
    QMessageBox::information(this, "Exported", "PDF created successfully.");
}


void MainWindow::on_QR_clicked_E()
{
    if(ui->tab_equipement_2->currentIndex().row() == -1) {
        QMessageBox::information(nullptr, QObject::tr("QR Code"),
                                 QObject::tr("Please select an equipment first.\n"
                                             "Click Ok to exit."), QMessageBox::Ok);
    } else {
        int row = ui->tab_equipement_2->currentIndex().row();

        int id = ui->tab_equipement_2->model()->data(
                                                ui->tab_equipement_2->model()->index(row, 0)).toInt();
        QString name = ui->tab_equipement_2->model()->data(
                                                      ui->tab_equipement_2->model()->index(row, 1)).toString();
        QString type = ui->tab_equipement_2->model()->data(
                                                      ui->tab_equipement_2->model()->index(row, 2)).toString();
        QString status = ui->tab_equipement_2->model()->data(
                                                        ui->tab_equipement_2->model()->index(row, 3)).toString();

        QString equipmentInfo = QString("Equipment ID: %1\n"
                                        "Name: %2\n"
                                        "Type: %3\n"
                                        "Status: %4")
                                    .arg(id)
                                    .arg(name)
                                    .arg(type)
                                    .arg(status);

        std::string infoToEncode = equipmentInfo.toStdString();

        const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(
            infoToEncode.c_str(), qrcodegen::QrCode::Ecc::LOW);

        std::ofstream myfile;
        myfile.open("qrcode.svg");
        myfile << qr.toSvgString(1);
        myfile.close();

        QPixmap pix("qrcode.svg");
        QPixmap scaledPix = pix.scaled(200, 200);
        ui->QRCODE_4->setPixmap(scaledPix);


        QMessageBox::information(this, "QR Code Content",
                                 "The QR code contains:\n" + equipmentInfo);
    }
}

void MainWindow::afficherStatistiques_E()
{
    QSqlQuery query;
    int onStock = 0, outOfStock = 0;

    if (query.exec("SELECT STATUS, COUNT(*) FROM EQUIPMENTS GROUP BY STATUS")) {
        while (query.next()) {
            QString status = query.value(0).toString().toLower();
            int count = query.value(1).toInt();

            if (status.contains("on stock"))
                onStock = count;
            else if (status.contains("out of stock"))
                outOfStock = count;
        }
    } else {
        qDebug() << "SQL Error:" << query.lastError().text();
        return;
    }

    // Pie chart setup
    QPieSeries *series = new QPieSeries();
    series->append("On Stock", onStock);
    series->append("Out of Stock", outOfStock);

    for (QPieSlice *slice : series->slices()) {
        slice->setLabel(QString("%1 (%2%)")
                            .arg(slice->label())
                            .arg(100 * slice->percentage(), 0, 'f', 1));
        slice->setExploded(true);
        slice->setLabelVisible(true);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Equipment Stock Statistics");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QGraphicsScene *scene = new QGraphicsScene();
    scene->addWidget(chartView);

    ui->graphicsView_2->setScene(scene);
    ui->graphicsView_2->show();
    chartView->setMinimumSize(400, 300);
}

void MainWindow::onBluetoothDataReceived_E(const QByteArray &data) {
    QString s = QString::fromUtf8(data).trimmed();
    qDebug() << "BT IN:" << s;
    bool ok = false;
    int id = s.toInt(&ok);

    QSqlQuery q;
    q.prepare("SELECT STATUS FROM EQUIPMENTS WHERE ID = :id");
    q.bindValue(":id", id);

    if (!q.exec()) {
        qDebug() << "SQL Error:" << q.lastError().text();
        bt->writeData("db error\n");
        return;
    }

    if (q.next()) {
        QString status = q.value(0).toString().toLower();
        if (status.contains("On stock")) {
            bt->writeData("ON STOCK\n");
        } else if (status.contains("Out Of Stock")) {
            bt->writeData("OUT OF STOCK\n");
        } else {
            bt->writeData(status.toUtf8() + "\n");
        }
    } else {
        bt->writeData("ID NOT FOUND\n");
    }

}

