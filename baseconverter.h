#ifndef BASECONVERTER_H
#define BASECONVERTER_H

#include <QMainWindow>
#include <cmath>

namespace Ui {
class BaseConverter;
}

class BaseConverter : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit BaseConverter(QWidget *parent = 0);
    ~BaseConverter();
    int alpha2Num(QChar ch);
    QChar num2Alpha(int num);
    QString base2Base(QString input, int initialBase, int targetBase);
    QString reverseStr(QString str);
    bool isInputValid();
    
public slots:
    void setValidator();
    void displayConversion();
    void swapBases();
    void clearAll();

private:
    Ui::BaseConverter *ui;
};

#endif // BASECONVERTER_H
