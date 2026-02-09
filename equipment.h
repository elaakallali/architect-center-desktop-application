#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <QString>
#include <QSqlQueryModel>

class Equipment {
public:
    Equipment();
    Equipment(int, QString, QString, QString);

    int getId();
    QString getName();
    QString getType();
    QString getStatus();

    void setId(int);
    void setName(QString);
    void setType(QString);
    void setStatus(QString);

    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int);
    bool checkIfExists(int);
    bool modifier(int id);

protected:
    bool eventFilter(QObject *obj, QEvent *event) ;

private:
    int id;
    QString name;
    QString type;
    QString status;
};

#endif // EQUIPMENT_H
