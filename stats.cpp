#include "stats.h"
#include "ui_stats.h"
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

stats::stats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stats)
{
    ui->setupUi(this);
    QPieSeries *series = new QPieSeries();

    QSqlQuery query("SELECT name, projects FROM clients");

    int totalProjects = 0;
    QList<QPair<QString, int>> data;

    // First loop: calculate total and store the data
    while (query.next()) {
        QString name = query.value("name").toString();
        int projectCount = query.value("projects").toInt();
        totalProjects += projectCount;
        data.append(qMakePair(name, projectCount));
    }

    // Second loop: add slices with percentage in label
    for (const auto &item : data) {
        double percentage = (totalProjects > 0) ? (100.0 * item.second) / totalProjects : 0;
        QString label = QString("%1 (%2%)").arg(item.first).arg(QString::number(percentage, 'f', 1));
        QPieSlice *slice = series->append(label, item.second);
        slice->setExploded(true);
        slice->setLabelVisible(true);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Number of Projects per Client");
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(ui->chartViewPlaceholder);
    layout->addWidget(chartView);
}

stats::~stats()
{
    delete ui;
}
