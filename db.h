#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QString>

class DB {
public:
    static DB& instance();
    bool connect();
    QSqlDatabase& database();
    bool isConnected() const;

private:
    DB() = default;
    QSqlDatabase m_db;
};

#endif // DB_H