#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>

enum class UserRole {
    SUPER_ADMIN,
    ADMIN,
    FORMATEUR,
    STAGIAIRE,
    UNKNOWN
};

struct UserSession {
    bool isValid = false;
    int userId = -1;
    QString nom;
    QString prenom;
    QString email;
    UserRole role = UserRole::UNKNOWN;
    QString roleStr;
};

class Authentification {
public:
    static QString hashPassword(const QString& rawPassword);
    static UserSession authentifier(const QString& email, const QString& password);
    static UserRole stringToRole(const QString& roleStr);
};

#endif // AUTHENTIFICATION_H