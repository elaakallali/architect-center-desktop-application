#ifndef MEETINGS_H
#define MEETINGS_H

#include <QString>
#include <QDateTime>
#include <QSqlQueryModel>

class Meetings {
private:
    int id;
    QString title;
    QString description;
    QDateTime startTime;
    QDateTime endTime;
    QString location;

public:
    Meetings();
    Meetings(int id, QString title, QString description, QDateTime startTime, QDateTime endTime, QString location);

    int getId() const;
    QString getTitle() const;
    QString getDescription() const;
    QDateTime getStartTime() const;
    QDateTime getEndTime() const;
    QString getLocation() const;

    void setId(int id);
    void setTitle(QString title);
    void setDescription(QString description);
    void setStartTime(QDateTime startTime);
    void setEndTime(QDateTime endTime);
    void setLocation(QString location);

    bool schedule();
    QSqlQueryModel* display();
    bool cancel(int id);
    bool modify(int id);
};

#endif // MEETINGS_H