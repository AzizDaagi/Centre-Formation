#ifndef EMAILNOTIFIER_H
#define EMAILNOTIFIER_H

#include <QString>

class EmailNotifier
{
public:
    static QString lastError();
    static bool notifyReportCreated(const QString &type, const QString &objectName,
                                    const QString &description, const QString &author,
                                    const QString &authorEmail);
    static bool notifyReportResolved(const QString &type, const QString &objectName,
                                     const QString &description, const QString &author,
                                     const QString &status);
};

#endif // EMAILNOTIFIER_H
