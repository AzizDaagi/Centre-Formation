#include "authentification.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QString Authentification::hashPassword(const QString& rawPassword) {
    QByteArray hash = QCryptographicHash::hash(rawPassword.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex()).toLower();
}

UserRole Authentification::stringToRole(const QString& roleStr) {
    QString upper = roleStr.trimmed().toUpper();
    if (upper == "SUPER_ADMIN") return UserRole::SUPER_ADMIN;
    if (upper == "ADMIN") return UserRole::ADMIN;
    if (upper == "FORMATEUR") return UserRole::FORMATEUR;
    if (upper == "STAGIAIRE") return UserRole::STAGIAIRE;
    return UserRole::UNKNOWN;
}

UserSession Authentification::authentifier(const QString& email, const QString& password) {
    UserSession session;
    QString cleanedEmail = email.trimmed().toLower();
    QString hashedInput = hashPassword(password);

    if (cleanedEmail.isEmpty() || password.isEmpty()) {
        return session;
    }

    
    QSqlQuery queryFormateur;
    queryFormateur.prepare(
        "SELECT ID_FORMATEUR, NOM, PRENOM, EMAIL, ROLE, STATUT_COMPTE "
        "FROM FORMATEUR "
        "WHERE LOWER(TRIM(EMAIL)) = :email AND LOWER(TRIM(PASSWORD_HASH)) = :hash"
    );
    queryFormateur.bindValue(":email", cleanedEmail);
    queryFormateur.bindValue(":hash", hashedInput);

    if (queryFormateur.exec() && queryFormateur.next()) {
        QString statut = queryFormateur.value("STATUT_COMPTE").toString().trimmed().toUpper();
        if (statut != "ACTIF") {
            qWarning() << "[AUTH WARN] Formateur account is inactive:" << cleanedEmail;
            return session;
        }

        session.isValid = true;
        session.userId = queryFormateur.value("ID_FORMATEUR").toInt();
        session.nom = queryFormateur.value("NOM").toString();
        session.prenom = queryFormateur.value("PRENOM").toString();
        session.email = queryFormateur.value("EMAIL").toString();
        session.roleStr = queryFormateur.value("ROLE").toString().trimmed().toUpper();
        session.role = stringToRole(session.roleStr);
        qDebug() << "[AUTH SUCCESS] Formateur login:" << session.email << "Role:" << session.roleStr;
        return session;
    }

    
    QSqlQuery queryStagiaire;
    queryStagiaire.prepare(
        "SELECT ID_STAGIAIRE, NOM, PRENOM, EMAIL, STATUT "
        "FROM STAGIAIRE "
        "WHERE LOWER(TRIM(EMAIL)) = :email AND LOWER(TRIM(PASSWORD_HASH)) = :hash"
    );
    queryStagiaire.bindValue(":email", cleanedEmail);
    queryStagiaire.bindValue(":hash", hashedInput);

    if (queryStagiaire.exec() && queryStagiaire.next()) {
        QString statut = queryStagiaire.value("STATUT").toString().trimmed().toUpper();
        if (statut != "ACTIF" && statut != "DIPLOME") {
            qWarning() << "[AUTH WARN] Stagiaire account is not active or graduated (" << statut << "):" << cleanedEmail;
            return session;
        }

        session.isValid = true;
        session.userId = queryStagiaire.value("ID_STAGIAIRE").toInt();
        session.nom = queryStagiaire.value("NOM").toString();
        session.prenom = queryStagiaire.value("PRENOM").toString();
        session.email = queryStagiaire.value("EMAIL").toString();
        session.roleStr = "STAGIAIRE";
        session.role = UserRole::STAGIAIRE;
        qDebug() << "[AUTH SUCCESS] Stagiaire login:" << session.email;
        return session;
    }

    qDebug() << "[AUTH FAIL] No matching active credentials for:" << cleanedEmail;
    return session;
}