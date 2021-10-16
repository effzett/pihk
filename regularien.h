#ifndef REGULARIEN_H
#define REGULARIEN_H

#include <QDialog>

namespace Ui {
class Regularien;
}

class Regularien : public QDialog
{
    Q_OBJECT

public:
    explicit Regularien(QWidget *parent = nullptr);
    ~Regularien();

private:
    Ui::Regularien *ui;
};

#endif // REGULARIEN_H
