#ifndef IHK_H
#define IHK_H

#include <QDialog>

namespace Ui {
class Ihk;
}

class Ihk : public QDialog
{
    Q_OBJECT
    
public:
    explicit Ihk(QWidget *parent = nullptr);
    ~Ihk();
    void fillIhk(qint32 t1, qint32 t21, qint32 t22, qint32 t23, qint32 doku, qint32 prfg ,qint32 mepr, qint32 meprnr=0);

private:
    Ui::Ihk *ui;
    QString getGrade(qint32 points);
};

#endif // IHK_H
