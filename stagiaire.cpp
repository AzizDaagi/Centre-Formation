#include "stagiaire.h"
#include "db.h"

#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

bool Stagiaire::ajouter()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "INSERT INTO STAGIAIRE "
        "(NOM, PRENOM, EMAIL, PASSWORD_HASH, "
        "ID_FORMATEUR, ID_COURS, ID_SALLE_ATTITREE, "
        "DATE_DEBUT, DATE_FIN_PREVUE, HEURES_VALIDEES, STATUT) "
        "VALUES (:nom, :prenom, :email, :password, "
        ":formateur, :cours, :salle, "
        ":dateDebut, :dateFin, :heures, :statut) "
        "RETURNING ID_STAGIAIRE INTO :out_id"
        );

    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":email", m_email);
    query.bindValue(":password", m_passwordHash);

    if (m_idFormateur == -1)
        query.bindValue(":formateur", QVariant());
    else
        query.bindValue(":formateur", m_idFormateur);

    if (m_idCours == -1)
        query.bindValue(":cours", QVariant());
    else
        query.bindValue(":cours", m_idCours);

    if (m_idSalleAttitree == -1)
        query.bindValue(":salle", QVariant());
    else
        query.bindValue(":salle", m_idSalleAttitree);

    query.bindValue(":dateDebut", m_dateDebut);
    query.bindValue(":dateFin", m_dateFinPrevue);
    query.bindValue(":heures", m_heuresValidees);
    query.bindValue(":statut", m_statut);
    query.bindValue(":out_id", 0, QSql::Out);

    if (!query.exec()) {
        qDebug() << "Erreur ajout stagiaire:"
                 << query.lastError().text();
        return false;
    }

    m_id = query.boundValue(":out_id").toInt();
    return true;
}

bool Stagiaire::modifier()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "UPDATE STAGIAIRE SET "
        "NOM = :nom, "
        "PRENOM = :prenom, "
        "EMAIL = :email, "
        "PASSWORD_HASH = :password, "
        "ID_FORMATEUR = :formateur, "
        "ID_COURS = :cours, "
        "ID_SALLE_ATTITREE = :salle, "
        "DATE_DEBUT = :dateDebut, "
        "DATE_FIN_PREVUE = :dateFin, "
        "HEURES_VALIDEES = :heures, "
        "STATUT = :statut "
        "WHERE ID_STAGIAIRE = :id"
        );

    query.bindValue(":nom", m_nom);
    query.bindValue(":prenom", m_prenom);
    query.bindValue(":email", m_email);
    query.bindValue(":password", m_passwordHash);

    if (m_idFormateur == -1)
        query.bindValue(":formateur", QVariant());
    else
        query.bindValue(":formateur", m_idFormateur);

    if (m_idCours == -1)
        query.bindValue(":cours", QVariant());
    else
        query.bindValue(":cours", m_idCours);

    if (m_idSalleAttitree == -1)
        query.bindValue(":salle", QVariant());
    else
        query.bindValue(":salle", m_idSalleAttitree);

    query.bindValue(":dateDebut", m_dateDebut);
    query.bindValue(":dateFin", m_dateFinPrevue);
    query.bindValue(":heures", m_heuresValidees);
    query.bindValue(":statut", m_statut);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur modification stagiaire:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

bool Stagiaire::supprimer()
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "DELETE FROM STAGIAIRE "
        "WHERE ID_STAGIAIRE = :id"
        );

    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression stagiaire:"
                 << query.lastError().text();
        return false;
    }

    return true;
}

QList<Stagiaire> Stagiaire::listerTout()
{
    QList<Stagiaire> resultats;

    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_STAGIAIRE, NOM, PRENOM, EMAIL, PASSWORD_HASH, "
        "ID_FORMATEUR, ID_COURS, ID_SALLE_ATTITREE, "
        "DATE_DEBUT, DATE_FIN_PREVUE, HEURES_VALIDEES, STATUT "
        "FROM STAGIAIRE "
        "ORDER BY ID_STAGIAIRE"
        );

    if (!query.exec()) {
        qDebug() << "Erreur liste stagiaires:"
                 << query.lastError().text();
        return resultats;
    }

    while (query.next()) {

        int idSalle = query.value(7).isNull()
        ? -1
        : query.value(7).toInt();

        Stagiaire s(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toInt(),
            query.value(6).toInt(),
            idSalle,
            query.value(8).toDate(),
            query.value(9).toDate(),
            query.value(10).toDouble(),
            query.value(11).toString()
            );

        resultats.append(s);
    }

    return resultats;
}

Stagiaire Stagiaire::trouverParId(int id)
{
    QSqlQuery query(DB::instance().database());

    query.prepare(
        "SELECT ID_STAGIAIRE, NOM, PRENOM, EMAIL, PASSWORD_HASH, "
        "ID_FORMATEUR, ID_COURS, ID_SALLE_ATTITREE, "
        "DATE_DEBUT, DATE_FIN_PREVUE, HEURES_VALIDEES, STATUT "
        "FROM STAGIAIRE "
        "WHERE ID_STAGIAIRE = :id"
        );

    query.bindValue(":id", id);

    if (query.exec() && query.next()) {

        int idSalle = query.value(7).isNull()
        ? -1
        : query.value(7).toInt();

        return Stagiaire(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toInt(),
            query.value(6).toInt(),
            idSalle,
            query.value(8).toDate(),
            query.value(9).toDate(),
            query.value(10).toDouble(),
            query.value(11).toString()
            );
    }

    return Stagiaire();
}