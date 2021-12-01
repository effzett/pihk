#include "prefs.h"

// Enthält die Daten vom Preference Dialog

Prefs::Prefs(QObject *parent) : QObject(parent)
{

}

Prefs::Prefs(qint32 minutes,
             FILEPARTS d1,
             FILEPARTSDELIM t1,
             FILEPARTS d2,
             FILEPARTSDELIM t2,
             FILEPARTS d3,
             FILESPACECHAR space,
             QObject *parent) : QObject(parent)
{
    m_minutes = minutes;
    m_d1 = (qint32)d1;
    m_d2 = (qint32)d2;
    m_d3 = (qint32)d3;
    m_t1 = (qint32)t1;
    m_t2 = (qint32)t2;
    m_space = (qint32)space;
}

qint32 Prefs::minutes() const
{
    return m_minutes;
}

void Prefs::setMinutes(qint32 newMinutes)
{
    m_minutes = newMinutes;
}

const QString &Prefs::dfn() const
{
    return m_dfn;
}

void Prefs::setDfn(const QString &newDfn)
{
    m_dfn = newDfn;
}

qint32 Prefs::d1() const
{
    return m_d1;
}

void Prefs::setD1(qint32 newD1)
{
    m_d1 = newD1;
}

qint32 Prefs::d2() const
{
    return m_d2;
}

void Prefs::setD2(qint32 newD2)
{
    m_d2 = newD2;
}

qint32 Prefs::d3() const
{
    return m_d3;
}

void Prefs::setD3(qint32 newD3)
{
    m_d3 = newD3;
}

qint32 Prefs::t1() const
{
    return m_t1;
}

void Prefs::setT1(qint32 newT1)
{
    m_t1 = newT1;
}

qint32 Prefs::t2() const
{
    return m_t2;
}

void Prefs::setT2(qint32 newT2)
{
    m_t2 = newT2;
}

qint32 Prefs::space() const
{
    return m_space;
}

void Prefs::setSpace(qint32 newSpace)
{
    m_space = newSpace;
}

