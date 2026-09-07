#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QString>

class DB {
public:
    static DB& instance();
    bool connect();
    QSqlDatabase database();
    bool isConnected() const;
    QString lastError() const;

private:
    DB() = default;
    ~DB() = default;
    DB(const DB&) = delete;
    DB& operator=(const DB&) = delete;

    QString m_lastError;
};

#endif 