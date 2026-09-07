#include "salle.h"
#include "db.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

bool Salle::ajouter() {
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "INSERT INTO SALLE (NOM_SALLE, CAPACITE, TYPE_SALLE, STATUT) "
        "VALUES (:nom, :capacite, :type, :statut) "
        "RETURNING ID_SALLE INTO :out_id"
    );
    query.bindValue(":nom", m_nom);
    query.bindValue(":capacite", m_capacite);
    query.bindValue(":type", m_type);
    query.bindValue(":statut", m_statut);
    query.bindValue(":out_id", 0, QSql::Out);

    if (!query.exec()) {
        qDebug() << "Erreur ajout salle:" << query.lastError().text();
        return false;
    }

    m_id = query.boundValue(":out_id").toInt();
    return true;
}

bool Salle::modifier() {
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "UPDATE SALLE SET NOM_SALLE = :nom, CAPACITE = :capacite, "
        "TYPE_SALLE = :type, STATUT = :statut WHERE ID_SALLE = :id"
        );
    query.bindValue(":nom", m_nom);
    query.bindValue(":capacite", m_capacite);
    query.bindValue(":type", m_type);
    query.bindValue(":statut", m_statut);
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur modification salle:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Salle::supprimer() {
    QSqlQuery query(DB::instance().database());
    query.prepare("DELETE FROM SALLE WHERE ID_SALLE = :id");
    query.bindValue(":id", m_id);

    if (!query.exec()) {
        qDebug() << "Erreur suppression salle:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Salle> Salle::listerTout() {
    QList<Salle> resultats;
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "SELECT s.ID_SALLE, s.NOM_SALLE, s.CAPACITE, s.TYPE_SALLE, "
        "CASE WHEN UPPER(TRIM(s.STATUT)) = 'DISPONIBLE' "
        "AND EXISTS (SELECT 1 FROM SALLE_RESERVATION r "
        "WHERE r.ID_SALLE = s.ID_SALLE AND r.STATUT = 'CONFIRMEE' AND r.DATE_FIN >= SYSDATE) "
        "THEN 'RESERVEE' ELSE s.STATUT END "
        "FROM SALLE s ORDER BY s.ID_SALLE"
    );

    if (!query.exec()) {
        qDebug() << "Erreur liste salles:" << query.lastError().text();
        return resultats;
    }

    while (query.next()) {
        resultats.append(Salle(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toInt(),
            query.value(3).toString(),
            query.value(4).toString()
            ));
    }
    return resultats;
}

Salle Salle::trouverParId(int id) {
    QSqlQuery query(DB::instance().database());
    query.prepare("SELECT ID_SALLE, NOM_SALLE, CAPACITE, TYPE_SALLE, STATUT FROM SALLE WHERE ID_SALLE = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Salle(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toInt(),
            query.value(3).toString(),
            query.value(4).toString()
            );
    }
    return Salle(); 
}

bool Salle::mettreAJourSignalement(int id, const QString &statut,
                                   const QString &description, const QString &auteur)
{
    if (id <= 0) {
        qWarning() << "Cannot update room report: invalid room id" << id;
        return false;
    }
    QSqlQuery query(DB::instance().database());
    query.prepare("UPDATE SALLE SET REPORT_STATUS = :statut, REPORT_DESCRIPTION = :description, "
                  "REPORT_AUTHOR = :auteur WHERE ID_SALLE = :id");
    query.bindValue(":statut", statut);
    query.bindValue(":description", description);
    query.bindValue(":auteur", auteur);
    query.bindValue(":id", id);
    const bool ok = query.exec();
    if (!ok) qWarning() << "Room report update failed:" << query.lastError().text();
    return ok;
}

bool Salle::effacerSignalement(int id)
{
    QSqlQuery query(DB::instance().database());
    query.prepare("UPDATE SALLE SET REPORT_STATUS = 'NONE', REPORT_DESCRIPTION = NULL, "
                  "REPORT_AUTHOR = NULL WHERE ID_SALLE = :id");
    query.bindValue(":id", id);
    return query.exec();
}