#include "formateur.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool Formateur::ajouter()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "INSERT INTO FORMATEUR "
        "(NOM, PRENOM, EMAIL, PASSWORD_HASH, ROLE, STATUT_COMPTE) "
        "VALUES (:nom, :prenom, :email, :password, :role, :statut) "
        "RETURNING ID_FORMATEUR INTO :out_id"
        );

    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":email", m_email);
    query.bindValue(":password", m_passwordHash);
    query.bindValue(":role", m_role);
    query.bindValue(":statut", m_statutCompte);
    query.bindValue(":out_id", 0, QSql::Out);

    if (!query.exec()) {
        qDebug() << "Erreur ajout formateur:"
                 << query.lastError().text();
        return false;
    }

    m_id = query.boundValue(":out_id").toInt();
    return true;
}

bool Formateur::modifier()
{
    QSqlQuery query(DB::instance().database());

    if (m_passwordHash.trimmed().isEmpty()) {
        query.prepare(
            "UPDATE FORMATEUR SET "
            "NOM = :nom, "
            "PRENOM = :prenom, "
            "EMAIL = :email, "
            "ROLE = :role, "
            "STATUT_COMPTE = :statut "
            "WHERE ID_FORMATEUR = :id"
            );
    } else {
        query.prepare(
            "UPDATE FORMATEUR SET "
            "NOM = :nom, "
            "PRENOM = :prenom, "
            "EMAIL = :email, "
            "PASSWORD_HASH = :password, "
            "ROLE = :role, "
            "STATUT_COMPTE = :statut "
            "WHERE ID_FORMATEUR = :id"
            );
        query.bindValue(":password", m_passwordHash);
    }

    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":email", m_email);
    query.bindValue(":role", m_role);
    query.bindValue(":statut", m_statutCompte);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur modification formateur:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

bool Formateur::supprimer()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "DELETE FROM FORMATEUR "
        "WHERE ID_FORMATEUR = :id"
        );

    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression formateur:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

QList<Formateur> Formateur::listerTout()
{
    QList<Formateur> resultats;

    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_FORMATEUR, NOM, PRENOM, EMAIL, "
        "PASSWORD_HASH, ROLE, STATUT_COMPTE "
        "FROM FORMATEUR "
        "ORDER BY ID_FORMATEUR"
        );

    if (!query.exec()) {
        qDebug() << "Erreur liste formateurs:"
                 << query.lastError().text();
        return resultats;
    }

    while (query.next()) {
        Formateur f(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString(),
            query.value(6).toString()
            );

        resultats.append(f);
    }

    return resultats;
}

Formateur Formateur::trouverParId(int id)
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_FORMATEUR, NOM, PRENOM, EMAIL, "
        "PASSWORD_HASH, ROLE, STATUT_COMPTE "
        "FROM FORMATEUR "
        "WHERE ID_FORMATEUR = :id"
        );

    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Formateur(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString(),
            query.value(6).toString()
            );
    }

    return Formateur();
}