#include "courswidget.h"
#include "cours.h"
#include "formateur.h"
#include "moduletools.h"
#include <QtCharts/QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

static QFrame* makeCardC(){
    QFrame* c=new QFrame(); c->setObjectName("glassCard");
    QGraphicsDropShadowEffect* sh=new QGraphicsDropShadowEffect(c);
    sh->setBlurRadius(25); sh->setColor(QColor(15,23,42,35)); sh->setOffset(0,8); c->setGraphicsEffect(sh);
    return c;
}

CoursWidget::CoursWidget(QWidget *parent):QWidget(parent){ setAttribute(Qt::WA_StyledBackground,true); setupUi(); rafraichirTable(); }
void CoursWidget::setupUi(){
    QVBoxLayout* root=new QVBoxLayout(this); root->setContentsMargins(10,10,10,10);
    m_stack=new QStackedWidget(this);
    m_stack->addWidget(creerPageListe()); m_stack->addWidget(creerPageFormulaire());
    root->addWidget(m_stack);
}
QWidget* CoursWidget::creerPageListe(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardC(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(24,20,24,20); cl->setSpacing(14);
    QHBoxLayout* hdr=new QHBoxLayout();
    QLabel* lbl=new QLabel("<b>Cours et Modules de Formation</b>"); lbl->setStyleSheet("font-size:15pt;color:#0f172a;");
    m_lblCount=new QLabel("0 cours"); m_lblCount->setStyleSheet("color:#64748b;font-size:10pt;background:rgba(100,116,139,0.1);border-radius:10px;padding:3px 10px;");
    hdr->addWidget(lbl); hdr->addStretch(); hdr->addWidget(m_lblCount);
    m_tableCours=new QTableWidget(); m_tableCours->setColumnCount(4);
    m_tableCours->setHorizontalHeaderLabels({"ID","Titre du Cours","Heures","Formateur"});
    m_tableCours->setSelectionBehavior(QAbstractItemView::SelectRows); m_tableCours->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableCours->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCours->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableCours->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    m_tableCours->verticalHeader()->setVisible(false); m_tableCours->setShowGrid(false);
    m_tableCours->setAlternatingRowColors(true); m_tableCours->setMinimumHeight(320);
    connect(m_tableCours,&QTableWidget::itemSelectionChanged,this,&CoursWidget::onSelectionChanged);
    QHBoxLayout* bar=new QHBoxLayout(); bar->setSpacing(10);
    m_btnAjouter=new QPushButton("+ Nouveau Cours"); m_btnModifier=new QPushButton("Modifier"); m_btnSupprimer=new QPushButton("Supprimer");
    m_btnAjouter->setObjectName("btnAjouter"); m_btnModifier->setObjectName("btnModifier"); m_btnSupprimer->setObjectName("btnSupprimer");
    m_btnModifier->setEnabled(false); m_btnSupprimer->setEnabled(false);
    bar->addWidget(m_btnAjouter); bar->addStretch(); bar->addWidget(m_btnModifier); bar->addWidget(m_btnSupprimer);
    connect(m_btnAjouter,&QPushButton::clicked,this,&CoursWidget::ouvrirFormulaireAjout);
    connect(m_btnModifier,&QPushButton::clicked,this,&CoursWidget::ouvrirFormulaireModification);
    connect(m_btnSupprimer,&QPushButton::clicked,this,&CoursWidget::supprimerCours);
    auto *tools=ModuleTools::createMultiCriteriaTools(m_tableCours,{1,2,3},{"titre","heures","formateur"});
    m_chart=new QChartView(); m_chart->setMinimumHeight(210);
    QPushButton *pdf=new QPushButton("Exporter PDF"); pdf->setObjectName("btnVider"); bar->insertWidget(1,pdf);
    connect(pdf,&QPushButton::clicked,this,[this]{ ModuleTools::exportTableToPdf(m_tableCours,"Rapport des cours"); });
    cl->addLayout(hdr); cl->addWidget(tools); cl->addWidget(m_tableCours);
    cl->addWidget(ModuleTools::createPaginationControls(m_tableCours));
    cl->addWidget(m_chart); cl->addLayout(bar); lay->addWidget(card); return page;
}
QWidget* CoursWidget::creerPageFormulaire(){
    QWidget* page=new QWidget(); QVBoxLayout* lay=new QVBoxLayout(page); lay->setContentsMargins(0,0,0,0);
    QFrame* card=makeCardC(); QVBoxLayout* cl=new QVBoxLayout(card); cl->setContentsMargins(32,28,32,28); cl->setSpacing(20);
    QHBoxLayout* bc=new QHBoxLayout();
    QPushButton* back=new QPushButton("<- Retour a la liste"); back->setObjectName("btnVider"); back->setFixedWidth(180);
    m_lblFormTitre=new QLabel("Nouveau Cours"); m_lblFormTitre->setStyleSheet("font-size:14pt;font-weight:bold;color:#0f172a;");
    bc->addWidget(back); bc->addSpacing(16); bc->addWidget(m_lblFormTitre); bc->addStretch();
    connect(back,&QPushButton::clicked,this,&CoursWidget::retourListe);
    QFrame* sep=new QFrame(); sep->setFrameShape(QFrame::HLine); sep->setStyleSheet("color:#e2e8f0;");
    QFormLayout* form=new QFormLayout(); form->setSpacing(14); form->setLabelAlignment(Qt::AlignRight); form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_editTitre=new QLineEdit(); m_editTitre->setPlaceholderText("ex: Administration Systemes Linux");
    m_editDescription=new QLineEdit(); m_editDescription->setPlaceholderText("Courte description du module");
    m_spinHeures=new QSpinBox(); m_spinHeures->setRange(1,1000); m_spinHeures->setValue(30); m_spinHeures->setSuffix("  heures");
    m_comboFormateur=new QComboBox(); rafraichirComboFormateurs();
    form->addRow("Titre du cours *:",m_editTitre); form->addRow("Description:",m_editDescription);
    form->addRow("Volume horaire:",m_spinHeures); form->addRow("Formateur responsable:",m_comboFormateur);
    QHBoxLayout* btnRow=new QHBoxLayout(); btnRow->setSpacing(10);
    m_btnEnregistrer=new QPushButton("Enregistrer"); m_btnAnnuler=new QPushButton("Annuler");
    m_btnEnregistrer->setObjectName("btnAjouter"); m_btnAnnuler->setObjectName("btnVider");
    m_btnEnregistrer->setMinimumHeight(40); m_btnAnnuler->setMinimumHeight(40);
    btnRow->addStretch(); btnRow->addWidget(m_btnAnnuler); btnRow->addWidget(m_btnEnregistrer);
    connect(m_btnEnregistrer,&QPushButton::clicked,this,&CoursWidget::enregistrer);
    connect(m_btnAnnuler,&QPushButton::clicked,this,&CoursWidget::retourListe);
    cl->addLayout(bc); cl->addWidget(sep); cl->addLayout(form); cl->addStretch(); cl->addLayout(btnRow);
    lay->addWidget(card); return page;
}
void CoursWidget::rafraichirComboFormateurs(){
    m_comboFormateur->clear(); m_comboFormateur->addItem("-- Aucun / Non assigne --",-1);
    for(const auto& f:Formateur::listerTout()) m_comboFormateur->addItem(f.prenom()+" "+f.nom(),f.id());
}
void CoursWidget::rafraichirTable(){
    rafraichirComboFormateurs();
    QList<Cours> list=Cours::listerTout(); m_tableCours->setRowCount(0);
    for(int i=0;i<list.size();++i){
        const Cours& c=list[i]; m_tableCours->insertRow(i);
        m_tableCours->setItem(i,0,new QTableWidgetItem(QString::number(c.id())));
        m_tableCours->setItem(i,1,new QTableWidgetItem(c.titre()));
        m_tableCours->setItem(i,2,new QTableWidgetItem(QString::number(c.heuresRequises())+" h"));
        QString nomF="Non assigne"; if(c.idFormateurResp()>0){ Formateur f=Formateur::trouverParId(c.idFormateurResp()); if(f.id()>0) nomF=f.prenom()+" "+f.nom(); }
        m_tableCours->setItem(i,3,new QTableWidgetItem(nomF));
    }
    m_lblCount->setText(QString::number(list.size())+" cours"); mettreAJourBoutonsListe();
    ModuleTools::updateCategoryChart(m_chart,m_tableCours,3,"Répartition des cours par formateur");
    ModuleTools::refreshPagination(m_tableCours);
}
void CoursWidget::afficherListe(){ retourListe(); rafraichirTable(); }
void CoursWidget::afficherFormulaireAjout(){ ouvrirFormulaireAjout(); }
void CoursWidget::onSelectionChanged(){
    QList<QTableWidgetItem*> sel=m_tableCours->selectedItems();
    m_idCoursSelectionne=!sel.isEmpty()?m_tableCours->item(m_tableCours->row(sel.first()),0)->text().toInt():-1;
    mettreAJourBoutonsListe();
}
void CoursWidget::mettreAJourBoutonsListe(){ bool s=m_idCoursSelectionne!=-1; m_btnModifier->setEnabled(s); m_btnSupprimer->setEnabled(s); }
void CoursWidget::ouvrirFormulaireAjout(){
    m_modeAjout=true; m_idCoursSelectionne=-1; m_lblFormTitre->setText("Nouveau Cours");
    m_editTitre->clear(); m_editTitre->setStyleSheet(""); m_editDescription->clear();
    m_spinHeures->setValue(30); m_comboFormateur->setCurrentIndex(0); m_stack->setCurrentIndex(1);
}
void CoursWidget::ouvrirFormulaireModification(){ if(m_idCoursSelectionne==-1) return; m_modeAjout=false; remplirFormulaire(m_idCoursSelectionne); m_stack->setCurrentIndex(1); }
void CoursWidget::remplirFormulaire(int id){
    Cours c=Cours::trouverParId(id); m_lblFormTitre->setText(QString("Modifier : %1").arg(c.titre()));
    m_editTitre->setText(c.titre()); m_editTitre->setStyleSheet(""); m_editDescription->setText(c.description());
    m_spinHeures->setValue(c.heuresRequises());
    int idx=m_comboFormateur->findData(c.idFormateurResp()); m_comboFormateur->setCurrentIndex(idx!=-1?idx:0);
}
void CoursWidget::enregistrer(){
    QString titre=m_editTitre->text().trimmed();
    if(titre.isEmpty()){ m_editTitre->setStyleSheet("border:1.5px solid #f43f5e;border-radius:8px;"); m_editTitre->setPlaceholderText("Ce champ est obligatoire"); return; }
    m_editTitre->setStyleSheet("");
    if(m_modeAjout){
        Cours c; c.setTitre(titre); c.setDescription(m_editDescription->text().trimmed());
        c.setHeuresRequises(m_spinHeures->value()); c.setIdFormateurResp(m_comboFormateur->currentData().toInt());
        if(!c.ajouter()){QMessageBox::critical(this,"Erreur","Echec de l'enregistrement."); return;}
    } else {
        Cours c=Cours::trouverParId(m_idCoursSelectionne); c.setTitre(titre); c.setDescription(m_editDescription->text().trimmed());
        c.setHeuresRequises(m_spinHeures->value()); c.setIdFormateurResp(m_comboFormateur->currentData().toInt());
        if(!c.modifier()){QMessageBox::critical(this,"Erreur","Echec de la modification."); return;}
    }
    rafraichirTable(); retourListe();
}
void CoursWidget::supprimerCours(){
    if(m_idCoursSelectionne==-1) return;
    QList<QTableWidgetItem*> sel=m_tableCours->selectedItems();
    QString titre; if(!sel.isEmpty()) titre=m_tableCours->item(m_tableCours->row(sel.first()),1)->text();
    auto rep=QMessageBox::question(this,"Confirmer la suppression",QString("Supprimer le cours <b>%1</b> ?").arg(titre),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(rep==QMessageBox::Yes){ Cours c=Cours::trouverParId(m_idCoursSelectionne); if(c.supprimer()){m_idCoursSelectionne=-1; rafraichirTable();} else QMessageBox::critical(this,"Erreur","Impossible de supprimer ce cours."); }
}
void CoursWidget::retourListe(){ m_idCoursSelectionne=-1; m_tableCours->clearSelection(); mettreAJourBoutonsListe(); m_stack->setCurrentIndex(0); }
