#ifndef CORE_H
#define CORE_H

#include <QMainWindow>
#include <QJsonDocument>

class Core : public QMainWindow
{
    Q_OBJECT
public:
    Core(QWidget *parent = 0);
    ~Core();
    void load_config();
    void about_multibox();
private:
    QJsonDocument cfg;
};

#endif // CORE_H