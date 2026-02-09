#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>

namespace Ui { class LoginDialog; }

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    bool isAuthenticated() const;

private slots:
    void on_buttonOk_clicked();

private:
    Ui::LoginDialog *ui;
    bool authenticated;
    QByteArray secretKeyBytes;

    QByteArray base32Decode(const QString &base32);
    QByteArray hmacSha1(const QByteArray &key, const QByteArray &message);
    QString generateTOTP(quint64 forTime);
};

#endif // LOGIN_DIALOG_H