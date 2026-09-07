#include "ollamaassistant.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QTimer>
#include <QUrl>

QString OllamaAssistant::ask(const QString &prompt, QString *errorMessage)
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    const QUrl endpoint(settings.value("Ollama/Host", "http://localhost:11434/api/generate").toString());
    const QString model = settings.value("Ollama/Model", "qwen2.5:7b").toString();

    QNetworkAccessManager manager;
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    const QJsonObject payload{
        {"model", model},
        {"prompt", prompt},
        {"stream", false},
        {"options", QJsonObject{{"temperature", 0.2}, {"num_ctx", 2048}}}
    };

    QNetworkReply *reply = manager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.start(45000);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.exec();

    if (!reply->isFinished()) {
        reply->abort();
        if (errorMessage) *errorMessage = "Ollama n'a pas répondu dans le délai prévu.";
        reply->deleteLater();
        return {};
    }
    if (reply->error() != QNetworkReply::NoError) {
        if (errorMessage) *errorMessage = "Ollama est indisponible. Lancez 'ollama serve' et vérifiez le modèle configure.";
        reply->deleteLater();
        return {};
    }

    const QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    if (!response.isObject() || response.object().value("response").toString().trimmed().isEmpty()) {
        if (errorMessage) *errorMessage = "Ollama a renvoyé une réponse vide.";
        return {};
    }
    return response.object().value("response").toString().trimmed();
}
