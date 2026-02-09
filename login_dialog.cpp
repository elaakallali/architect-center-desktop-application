#include "login_dialog.h"
#include "ui_login_dialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    authenticated(false)
{
    ui->setupUi(this);
    setWindowTitle("Login");

    connect(ui->buttonOk, &QPushButton::clicked, this, &LoginDialog::on_buttonOk_clicked);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::isAuthenticated() const
{
    return authenticated;
}

void LoginDialog::on_buttonOk_clicked()
{
    QString code = ui->lineEditCode->text();
    // Here you would validate the code against your authentication logic
    if (code == "expected_code") { // Replace with actual validation
        authenticated = true;
        accept();
    } else {
        QMessageBox::warning(this, "Authentication Failed", "Invalid code. Please try again.");
    }
}