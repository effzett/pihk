#ifndef PREFS_H
#define PREFS_H

#include <QObject>
#include <mypihk.h>

class Prefs : public QObject
{
    Q_OBJECT
public:
    explicit Prefs(QObject *parent = nullptr);
    explicit Prefs(qint32 minutes,
                   FILEPARTS d1,
                   FILEPARTSDELIM t1,
                   FILEPARTS d2,
                   FILEPARTSDELIM t2,
                   FILEPARTS d3,
                   FILESPACECHAR space,
                   QObject *parent = nullptr);
    qint32 minutes() const;
    void setMinutes(qint32 newMinutes);
    const QString &dfn() const;
    void setDfn(const QString &newDfn);
    qint32 d1() const;
    void setD1(qint32 newD1);
    qint32 d2() const;
    void setD2(qint32 newD2);
    qint32 d3() const;
    void setD3(qint32 newD3);
    qint32 t1() const;
    void setT1(qint32 newT1);
    qint32 t2() const;
    void setT2(qint32 newT2);
    qint32 space() const;
    void setSpace(qint32 newSpace);

signals:

private:
    qint32 m_minutes;  // Prüfungszeit
    QString m_dfn="";    // dynamisch generierter Filename
    qint32 m_d1,m_d2,m_d3;   // filename komponenten
    qint32 m_t1,m_t2;      // Trenner im Filename
    qint32 m_space;  // Ersatz für Leerzeichen im Filename
};

#endif // PREFS_H
