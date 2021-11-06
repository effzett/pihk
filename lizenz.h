#ifndef LIZENZ_H
#define LIZENZ_H

#include <QDialog>

namespace Ui {
class Lizenz;
}

class Lizenz : public QDialog
{
    Q_OBJECT

public:
    explicit Lizenz(QWidget *parent = nullptr);
    ~Lizenz();

private:
    Ui::Lizenz *ui;
};

#endif // LIZENZ_H
