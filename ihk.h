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
    void fillIhk(qint32);

private:
    Ui::Ihk *ui;
    QString getGrade(qint32 points);
};

#endif // IHK_H
