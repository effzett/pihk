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
    
private:
    Ui::Ihk *ui;
};

#endif // IHK_H
