#include "roleworkspace.h"
#include "db.h"
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QSqlQuery>
#include <QTableWidget>
#include <QVBoxLayout>

RoleWorkspace::RoleWorkspace(Mode mode, QWidget *parent) : QWidget(parent), m_mode(mode) { setupUi(); }
QWidget *RoleWorkspace::createMetricCard(const QString &label, QLabel **value, const QString &accent) {
    auto *card = new QFrame(this); card->setStyleSheet(QString("background:#fff;border:1px solid #e2e8f0;border-left:5px solid %1;border-radius:16px;").arg(accent));
    auto *layout = new QVBoxLayout(card); layout->setContentsMargins(18,14,18,14);
    auto *caption = new QLabel(label, card); caption->setStyleSheet("color:#64748b;font-size:10pt;font-weight:600;");
    *value = new QLabel("—", card); (*value)->setStyleSheet("color:#0f172a;font-size:24pt;font-weight:800;"); layout->addWidget(caption); layout->addWidget(*value); return card;
}
void RoleWorkspace::setupUi() {
    auto *root = new QVBoxLayout(this); root->setContentsMargins(24,24,24,24); root->setSpacing(18);
    auto *hero = new QFrame(this); hero->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #0f172a,stop:1 #2563eb);border-radius:22px;");
    auto *heroLayout = new QVBoxLayout(hero); heroLayout->setContentsMargins(28,24,28,24);
    m_welcome = new QLabel(hero); m_welcome->setStyleSheet("color:white;font-size:22pt;font-weight:800;");
    auto *description = new QLabel(m_mode == Mode::Formateur ? "Votre studio pédagogique : cours, stagiaires et charge de formation." : "Votre espace d'apprentissage : parcours, progression et repères.", hero); description->setStyleSheet("color:#dbeafe;font-size:11pt;"); heroLayout->addWidget(m_welcome); heroLayout->addWidget(description);
    auto *metrics = new QGridLayout();
    if (m_mode == Mode::Formateur) { metrics->addWidget(createMetricCard("MES COURS", &m_primaryMetric,"#38bdf8"),0,0); metrics->addWidget(createMetricCard("MES STAGIAIRES", &m_secondaryMetric,"#a78bfa"),0,1); metrics->addWidget(createMetricCard("HEURES À ASSURER", &m_tertiaryMetric,"#2dd4bf"),0,2); }
    else { metrics->addWidget(createMetricCard("HEURES VALIDÉES", &m_primaryMetric,"#38bdf8"),0,0); metrics->addWidget(createMetricCard("PROGRESSION", &m_secondaryMetric,"#a78bfa"),0,1); metrics->addWidget(createMetricCard("STATUT", &m_tertiaryMetric,"#2dd4bf"),0,2); }
    auto *card = new QFrame(this); card->setObjectName("glassCard"); auto *tableLayout = new QVBoxLayout(card); tableLayout->setContentsMargins(22,20,22,22);
    auto *title = new QLabel(m_mode == Mode::Formateur ? "Mon activité pédagogique" : "Mon parcours de formation", card); title->setStyleSheet("font-size:14pt;font-weight:800;color:#0f172a;");
    m_table = new QTableWidget(card); m_table->setEditTriggers(QAbstractItemView::NoEditTriggers); m_table->setSelectionBehavior(QAbstractItemView::SelectRows); m_table->setAlternatingRowColors(true); m_table->verticalHeader()->setVisible(false); m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); m_table->setMinimumHeight(270); tableLayout->addWidget(title); tableLayout->addWidget(m_table);
    root->addWidget(hero); root->addLayout(metrics); root->addWidget(card,1);
}
void RoleWorkspace::setUser(int id, const QString &firstName, const QString &) { m_userId=id; m_welcome->setText(QString("Bonjour, %1").arg(firstName)); refresh(); }
void RoleWorkspace::refresh() {
    if(m_userId<0) return; m_table->setRowCount(0); QSqlQuery query(DB::instance().database());
    if(m_mode==Mode::Formateur) {
        query.prepare("SELECT COUNT(*),NVL(SUM(HEURES_REQUISES),0) FROM COURS WHERE ID_FORMATEUR_RESP=:id"); query.bindValue(":id",m_userId); if(query.exec()&&query.next()){m_primaryMetric->setText(query.value(0).toString());m_tertiaryMetric->setText(query.value(1).toString()+" h");}
        query.prepare("SELECT COUNT(*) FROM STAGIAIRE WHERE ID_FORMATEUR=:id");query.bindValue(":id",m_userId);if(query.exec()&&query.next())m_secondaryMetric->setText(query.value(0).toString());
        m_table->setColumnCount(4);m_table->setHorizontalHeaderLabels({"Cours","Heures","Stagiaires inscrits","Statut"});query.prepare("SELECT c.TITRE,c.HEURES_REQUISES,COUNT(s.ID_STAGIAIRE),NVL(c.REPORT_STATUS,'ACTIF') FROM COURS c LEFT JOIN STAGIAIRE s ON s.ID_COURS=c.ID_COURS WHERE c.ID_FORMATEUR_RESP=:id GROUP BY c.TITRE,c.HEURES_REQUISES,c.REPORT_STATUS ORDER BY c.TITRE");query.bindValue(":id",m_userId);
    } else { m_table->setColumnCount(5);m_table->setHorizontalHeaderLabels({"Cours","Formateur","Salle","Période","Statut"});query.prepare("SELECT NVL(c.TITRE,'Non attribué'),NVL(f.PRENOM||' '||f.NOM,'Non attribué'),NVL(sa.NOM_SALLE,'Non attribuée'),TO_CHAR(s.DATE_DEBUT,'DD/MM/YYYY')||' - '||TO_CHAR(s.DATE_FIN_PREVUE,'DD/MM/YYYY'),s.STATUT,s.HEURES_VALIDEES,NVL(c.HEURES_REQUISES,0) FROM STAGIAIRE s LEFT JOIN COURS c ON c.ID_COURS=s.ID_COURS LEFT JOIN FORMATEUR f ON f.ID_FORMATEUR=s.ID_FORMATEUR LEFT JOIN SALLE sa ON sa.ID_SALLE=s.ID_SALLE_ATTITREE WHERE s.ID_STAGIAIRE=:id");query.bindValue(":id",m_userId); }
    if(!query.exec())return;int row=0;while(query.next()){if(m_mode==Mode::Stagiaire){double done=query.value(5).toDouble(),required=query.value(6).toDouble();m_primaryMetric->setText(QString::number(done,'f',0)+" h");m_secondaryMetric->setText(required>0?QString::number(qRound(done*100.0/required))+"%":"—");m_tertiaryMetric->setText(query.value(4).toString());}m_table->insertRow(row);int cols=m_mode==Mode::Formateur?4:5;for(int col=0;col<cols;++col)m_table->setItem(row,col,new QTableWidgetItem(query.value(col).toString()));++row;}
}
