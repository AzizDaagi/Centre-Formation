#include "cours.h"
#include "db.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool Cours::ajouter()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "INSERT INTO COURS "
        "(TITRE, DESCRIPTION, HEURES_REQUISES, ID_FORMATEUR_RESP) "
        "VALUES (:titre, :description, :heures, :formateur) "
        "RETURNING ID_COURS INTO :out_id"
        );

    query.bindValue(":titre", m_titre);
    query.bindValue(":description", m_description);
    query.bindValue(":heures", m_heuresRequises);
    if (m_idFormateurResp == -1)
        query.bindValue(":formateur", QVariant());
    else
        query.bindValue(":formateur", m_idFormateurResp);
    query.bindValue(":out_id", 0, QSql::Out);

    if (!query.exec()) {
        qDebug() << "Erreur ajout cours:"
                 << query.lastError().text();
        return false;
    }

    m_id = query.boundValue(":out_id").toInt();
    return true;
}

bool Cours::modifier()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "UPDATE COURS SET "
        "TITRE = :titre, "
        "DESCRIPTION = :description, "
        "HEURES_REQUISES = :heures, "
        "ID_FORMATEUR_RESP = :formateur "
        "WHERE ID_COURS = :id"
        );

    query.bindValue(":titre", m_titre);
    query.bindValue(":description", m_description);
    query.bindValue(":heures", m_heuresRequises);
    if (m_idFormateurResp == -1)
        query.bindValue(":formateur", QVariant());
    else
        query.bindValue(":formateur", m_idFormateurResp);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur modification cours:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

bool Cours::supprimer()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "DELETE FROM COURS "
        "WHERE ID_COURS = :id"
        );

    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression cours:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

QList<Cours> Cours::listerTout()
{
    QList<Cours> resultats;

    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_COURS, TITRE, DESCRIPTION, "
        "HEURES_REQUISES, ID_FORMATEUR_RESP "
        "FROM COURS "
        "ORDER BY ID_COURS"
        );

    if (!query.exec()) {
        qDebug() << "Erreur liste cours:"
                 << query.lastError().text();
        return resultats;
    }

    while (query.next()) {
        Cours c(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toInt(),
            query.value(4).toInt()
            );

        resultats.append(c);
    }

    return resultats;
}

Cours Cours::trouverParId(int id)
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_COURS, TITRE, DESCRIPTION, "
        "HEURES_REQUISES, ID_FORMATEUR_RESP "
        "FROM COURS "
        "WHERE ID_COURS = :id"
        );

    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Cours(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toInt(),
            query.value(4).toInt()
            );
    }

    return Cours();
}