#include "db.h"
#include <QDebug>
#include <QSqlError>
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

DB& DB::instance() {
    static DB instance;
    return instance;
}

bool DB::connect() {
    QString settingsPath = "settings.ini";
    if (!QFileInfo::exists(settingsPath)) {
        QString appDirSettings = QCoreApplication::applicationDirPath() + "/settings.ini";
        if (QFileInfo::exists(appDirSettings)) {
            settingsPath = appDirSettings;
        }
    }

    QSettings settings(settingsPath, QSettings::IniFormat);
    QString driver = settings.value("Database/Driver", "Oracle in OraDB21Home1").toString();
    QString host = settings.value("Database/Host", "localhost").toString();
    int port = settings.value("Database/Port", 1521).toInt();
    QString service = settings.value("Database/Service", "XEPDB1").toString();
    QString user = settings.value("Database/User", "centre_formation").toString();
    QString password = settings.value("Database/Password", "09403098").toString();

    
    if (!QFileInfo::exists(settingsPath)) {
        settings.setValue("Database/Driver", driver);
        settings.setValue("Database/Host", host);
        settings.setValue("Database/Port", port);
        settings.setValue("Database/Service", service);
        settings.setValue("Database/User", user);
        settings.setValue("Database/Password", password);
        settings.sync();
    }

    QSqlDatabase db = QSqlDatabase::contains(QSqlDatabase::defaultConnection)
                          ? QSqlDatabase::database(QSqlDatabase::defaultConnection)
                          : QSqlDatabase::addDatabase("QODBC");

    QString dsn = QString("Driver={%1};DBQ=%2:%3/%4;Uid=%5;Pwd=%6;")
                      .arg(driver, host, QString::number(port), service, user, password);
    db.setDatabaseName(dsn);

    if (!db.open()) {
        m_lastError = db.lastError().text();
        qDebug() << "Connection failed:" << m_lastError;
        return false;
    }

    qDebug() << "Connected to Oracle successfully.";
    return true;
}

QSqlDatabase DB::database() {
    return QSqlDatabase::database();
}

bool DB::isConnected() const {
    return QSqlDatabase::database().isOpen();
}

QString DB::lastError() const {
    return m_lastError;
}