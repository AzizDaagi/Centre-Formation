#ifndef RESERVATION_H
#define RESERVATION_H

#include <QDateTime>
#include <QList>
#include <QString>

class Reservation
{
public:
    Reservation() = default;
    Reservation(int id, int idStagiaire, const QString &stagiaire,
                const QString &salle, const QDateTime &dateDebut,
                const QDateTime &dateFin, const QString &statut)
        : m_id(id), m_idStagiaire(idStagiaire), m_stagiaire(stagiaire),
          m_salle(salle), m_dateDebut(dateDebut), m_dateFin(dateFin),
          m_statut(statut) {}

    int id() const { return m_id; }
    int idStagiaire() const { return m_idStagiaire; }
    QString stagiaire() const { return m_stagiaire; }
    QString salle() const { return m_salle; }
    QDateTime dateDebut() const { return m_dateDebut; }
    QDateTime dateFin() const { return m_dateFin; }
    QString statut() const { return m_statut; }

    static bool reserver(int idStagiaire, int idSalle,
                         const QDateTime &dateDebut, const QDateTime &dateFin,
                         QString *errorMessage = nullptr);
    static QList<Reservation> listerTout();
    static QList<Reservation> listerPourStagiaire(int idStagiaire);
    static bool annuler(int idReservation, int idStagiaire = -1,
                        QString *errorMessage = nullptr);

private:
    int m_id = -1;
    int m_idStagiaire = -1;
    QString m_stagiaire;
    QString m_salle;
    QDateTime m_dateDebut;
    QDateTime m_dateFin;
    QString m_statut;
};

#endif 
