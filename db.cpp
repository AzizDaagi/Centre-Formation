#include "db.h"
#include <QDebug>
#include <QSqlError>

DB& DB::instance() {
    static DB instance;   // created once, reused everywhere
    return instance;
}

bool DB::connect() {
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setDatabaseName(
        "Driver={Oracle in OraDB21Home1};DBQ=localhost:1521/XEPDB1;Uid=centre_formation;Pwd=09403098;"
        );

    if (!m_db.open()) {
        qDebug() << "Connection failed:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "Connected to Oracle successfully.";
    return true;
}

QSqlDatabase& DB::database() {
    return m_db;
}

bool DB::isConnected() const {
    return m_db.isOpen();
}