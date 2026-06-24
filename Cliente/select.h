#ifndef SELECT_H
#define SELECT_H
#include <QLabel>

class Select:public QLabel
{
    Q_OBJECT
public:
    explicit Select(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
