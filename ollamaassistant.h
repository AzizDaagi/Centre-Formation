#ifndef OLLAMAASSISTANT_H
#define OLLAMAASSISTANT_H

#include <QString>

class OllamaAssistant
{
public:
    static QString ask(const QString &prompt, QString *errorMessage = nullptr);
};

#endif // OLLAMAASSISTANT_H
