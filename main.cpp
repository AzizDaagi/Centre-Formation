#include "mainwindow.h"
#include "db.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Load stylesheet (adjust path if using Qt Resource file like ":/style.qss")
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
        styleFile.close();
    } else {
        qDebug() << "Could not open QSS file:" << styleFile.errorString();
    }

    // 2. Database connection check with user-facing error dialog
    if (!DB::instance().connect()) {
        QMessageBox::critical(
            nullptr,
            "Erreur de Connexion Base de Données",
            "Impossible d'établir la connexion avec la base de données Oracle.\n\n"
            "Détail de l'erreur :\n" + DB::instance().lastError() + "\n\n"
            "Vérifiez que le service Oracle (XE) est démarré et que les paramètres dans settings.ini sont corrects."
        );
        return -1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}