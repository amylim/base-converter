/*******************************************************
Author: Amy Lim
Project: Converts input to a different base
Date Created: August 17, 2012
Last Updated: August 20, 2012
******************************************************/

#include <QtGui/QApplication>
#include "baseconverter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaseConverter w;
    w.show();
    
    return a.exec();
}
