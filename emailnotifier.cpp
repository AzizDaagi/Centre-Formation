#include "emailnotifier.h"
#include "db.h"

#include <QRegularExpression>
#include <QSettings>
#include <QCoreApplication>
#include <QFileInfo>
#include <QSqlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace {
QString g_lastError;

struct EmailConfig {
    QString apiKey;
    QString senderEmail;
    QString senderName;
    bool enabled = false;
};

EmailConfig loadConfig()
{
    QString settingsPath = "settings.ini";
    if (!QFileInfo::exists(settingsPath)) {
        const QString appSettings = QCoreApplication::applicationDirPath() + "/settings.ini";
        if (QFileInfo::exists(appSettings)) settingsPath = appSettings;
    }
    QSettings settings(settingsPath, QSettings::IniFormat);
    EmailConfig config;
    config.apiKey = settings.value("Email/ApiKey").toString().trimmed();
    config.senderEmail = settings.value("Email/SenderEmail").toString().trimmed();
    config.senderName = settings.value("Email/SenderName", "CentrePro").toString().trimmed();
    config.enabled = settings.value("Email/Enabled", false).toBool();
    return config;
}

bool sendMessage(const EmailConfig &config, const QStringList &recipients,
                 const QString &subject, const QString &body)
{
    if (!config.enabled || config.apiKey.isEmpty() || config.senderEmail.isEmpty() || recipients.isEmpty()) {
        g_lastError = QString("Configuration email invalide: enabled=%1, apiKey=%2, sender=%3, destinataires=%4")
                          .arg(config.enabled ? "true" : "false")
                          .arg(config.apiKey.isEmpty() ? "absente" : "presente")
                          .arg(config.senderEmail.isEmpty() ? "absent" : "present")
                          .arg(recipients.size());
        qWarning() << "Email notification skipped:" << g_lastError;
        return false;
    }

    QJsonArray to;
    for (const QString &recipient : recipients)
        to.append(QJsonObject{{"email", recipient}});

    const QJsonObject payload{
        {"sender", QJsonObject{{"name", config.senderName}, {"email", config.senderEmail}}},
        {"to", to},
        {"subject", subject},
        {"textContent", body}
    };

    QNetworkRequest request(QUrl("https://api.brevo.com/v3/smtp/email"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api-key", config.apiKey.toUtf8());

    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    const bool sent = reply->error() == QNetworkReply::NoError;
    if (!sent) {
        g_lastError = QString("%1: %2").arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())
                                      .arg(QString::fromUtf8(reply->readAll()));
        qWarning() << "Brevo email failed:" << g_lastError << reply->errorString();
    } else {
        g_lastError.clear();
    }
    reply->deleteLater();
    return sent;
}

QString extractEmail(const QString &author)
{
    static const QRegularExpression pattern("<([^>]+@[^>]+)>");
    const auto match = pattern.match(author);
    return match.hasMatch() ? match.captured(1).trimmed() : QString();
}

QStringList adminEmails()
{
    QStringList result;
    QSqlQuery query(DB::instance().database());
    query.prepare("SELECT TRIM(EMAIL) FROM FORMATEUR "
                  "WHERE UPPER(TRIM(ROLE)) IN ('ADMIN', 'SUPER_ADMIN') "
                  "AND UPPER(TRIM(STATUT_COMPTE)) = 'ACTIF' AND EMAIL IS NOT NULL");
    if (query.exec()) {
        while (query.next()) result.append(query.value(0).toString().trimmed());
    }
    return result;
}

} // namespace

QString EmailNotifier::lastError()
{
    return g_lastError;
}

bool EmailNotifier::notifyReportCreated(const QString &type, const QString &objectName,
                                        const QString &description, const QString &author,
                                        const QString &authorEmail)
{
    const QStringList recipients = adminEmails();
    return sendMessage(loadConfig(), recipients,
                       QString("Nouveau signalement - %1").arg(type),
                       QString("Un nouveau signalement a été créé.\n\nÉlément : %1\nAuteur : %2\nEmail : %3\n\nDescription :\n%4")
                           .arg(objectName, author, authorEmail, description));
}

bool EmailNotifier::notifyReportResolved(const QString &type, const QString &objectName,
                                         const QString &description, const QString &author,
                                         const QString &status)
{
    const QString recipient = extractEmail(author);
    if (recipient.isEmpty()) {
        qWarning() << "Cannot notify report author: no email in REPORT_AUTHOR.";
        return false;
    }
    return sendMessage(loadConfig(), {recipient},
                       QString("Signalement mis à jour - %1").arg(status),
                       QString("Votre signalement a été mis à jour.\n\nÉlément : %1\nStatut : %2\n\nDescription :\n%3")
                           .arg(objectName, status, description));
}
