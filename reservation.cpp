#include "reservation.h"
#include "db.h"

#include <QSqlError>
#include <QSqlQuery>

namespace {
void setError(QString *errorMessage, const QSqlQuery &query)
{
    if (errorMessage) {
        *errorMessage = query.lastError().text();
    }
}

QList<Reservation> readReservations(QSqlQuery &query)
{
    QList<Reservation> resultats;
    if (!query.exec()) {
        return resultats;
    }
    while (query.next()) {
        resultats.append(Reservation(
            query.value(0).toInt(), query.value(1).toInt(), query.value(2).toString(),
            query.value(3).toString(), query.value(4).toDateTime(),
            query.value(5).toDateTime(), query.value(6).toString()
        ));
    }
    return resultats;
}
}

bool Reservation::reserver(int idStagiaire, int idSalle,
                           const QDateTime &dateDebut, const QDateTime &dateFin,
                           QString *errorMessage)
{
    QSqlQuery conflict(DB::instance().database());
    conflict.prepare(
        "SELECT COUNT(*) FROM SALLE_RESERVATION "
        "WHERE ID_SALLE = :salle AND STATUT = 'CONFIRMEE' "
        "AND DATE_DEBUT < :fin AND DATE_FIN > :debut"
    );
    conflict.bindValue(":salle", idSalle);
    conflict.bindValue(":debut", dateDebut);
    conflict.bindValue(":fin", dateFin);
    if (!conflict.exec() || !conflict.next()) {
        setError(errorMessage, conflict);
        return false;
    }
    if (conflict.value(0).toInt() > 0) {
        if (errorMessage) {
            *errorMessage = "Cette salle est déjà réservée sur le créneau sélectionné.";
        }
        return false;
    }

    QSqlQuery query(DB::instance().database());
    query.prepare(
        "INSERT INTO SALLE_RESERVATION "
        "(ID_STAGIAIRE, ID_SALLE, DATE_DEBUT, DATE_FIN, STATUT) "
        "VALUES (:stagiaire, :salle, :debut, :fin, 'CONFIRMEE')"
    );
    query.bindValue(":stagiaire", idStagiaire);
    query.bindValue(":salle", idSalle);
    query.bindValue(":debut", dateDebut);
    query.bindValue(":fin", dateFin);
    if (!query.exec()) {
        setError(errorMessage, query);
        return false;
    }
    return true;
}

QList<Reservation> Reservation::listerTout()
{
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "SELECT r.ID_RESERVATION, r.ID_STAGIAIRE, "
        "st.PRENOM || ' ' || st.NOM, sa.NOM_SALLE, "
        "r.DATE_DEBUT, r.DATE_FIN, r.STATUT "
        "FROM SALLE_RESERVATION r "
        "JOIN STAGIAIRE st ON st.ID_STAGIAIRE = r.ID_STAGIAIRE "
        "JOIN SALLE sa ON sa.ID_SALLE = r.ID_SALLE "
        "WHERE r.STATUT = 'CONFIRMEE' AND r.DATE_FIN >= SYSDATE "
        "ORDER BY r.DATE_DEBUT"
    );
    return readReservations(query);
}

QList<Reservation> Reservation::listerPourStagiaire(int idStagiaire)
{
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "SELECT r.ID_RESERVATION, r.ID_STAGIAIRE, "
        "st.PRENOM || ' ' || st.NOM, sa.NOM_SALLE, "
        "r.DATE_DEBUT, r.DATE_FIN, r.STATUT "
        "FROM SALLE_RESERVATION r "
        "JOIN STAGIAIRE st ON st.ID_STAGIAIRE = r.ID_STAGIAIRE "
        "JOIN SALLE sa ON sa.ID_SALLE = r.ID_SALLE "
        "WHERE r.ID_STAGIAIRE = :stagiaire AND r.STATUT = 'CONFIRMEE' "
        "AND r.DATE_FIN >= SYSDATE ORDER BY r.DATE_DEBUT"
    );
    query.bindValue(":stagiaire", idStagiaire);
    return readReservations(query);
}

bool Reservation::annuler(int idReservation, int idStagiaire, QString *errorMessage)
{
    QSqlQuery query(DB::instance().database());
    query.prepare(
        "UPDATE SALLE_RESERVATION SET STATUT = 'ANNULEE' "
        "WHERE ID_RESERVATION = :id "
        "AND STATUT = 'CONFIRMEE' "
        "AND DATE_DEBUT > SYSDATE "
        "AND (:stagiaire = -1 OR ID_STAGIAIRE = :stagiaire)"
    );
    query.bindValue(":id", idReservation);
    query.bindValue(":stagiaire", idStagiaire);
    if (!query.exec()) {
        setError(errorMessage, query);
        return false;
    }
    if (query.numRowsAffected() != 1) {
        if (errorMessage) {
            *errorMessage = "La réservation est introuvable, déjà annulée ou déjà commencée.";
        }
        return false;
    }
    return true;
}
