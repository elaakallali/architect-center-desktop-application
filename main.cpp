#include "mainwindow.h"
#include "login_dialog.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"
#include "ai_dialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Connection c;
    if(!c.createconnect()){
        QMessageBox::critical(nullptr, QObject::tr("Database Connection Failed"),
                              QObject::tr("Connection failed. Click Cancel to exit."), QMessageBox::Cancel);
        return -1;
    }

    // Show authenticator dialog first
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted && loginDialog.isAuthenticated()) {
        MainWindow w;
        w.show();
        AIDialog x;
        x.show();

        return a.exec();
    } else {
        QMessageBox::critical(nullptr, "Authentication", "Authentication failed. Exiting application.");
        return -1;
    }
}
