#include "baseconverter.h"
#include "ui_baseconverter.h"

BaseConverter::BaseConverter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BaseConverter)
{
    ui->setupUi(this);

    //populates the combo boxes with different choices of base conversions
    QStringList bases;
    for(int i = 2; i <= 16; i++)
        bases.append(QString::number(i));
    ui->initialBase->addItems(bases);
    ui->targetBase->addItems(bases);

    //sets up the input line to only accept valid numbers/letters
    setValidator();
    connect(ui->initialBase, SIGNAL(currentIndexChanged(int)), this, SLOT(setValidator()));

    //conversion will take place when the user enters/changes their input or the value of the bases in the combo boxes
    connect(ui->initialBaseInput, SIGNAL(textChanged(QString)), this, SLOT(displayConversion()));
    connect(ui->initialBase, SIGNAL(currentIndexChanged(int)), this, SLOT(displayConversion()));
    connect(ui->targetBase, SIGNAL(currentIndexChanged(int)), this, SLOT(displayConversion()));

    //set up Swap Base button to swap between the initial value base and the target base values
    connect(ui->swapBaseButton, SIGNAL(clicked()), this, SLOT(swapBases()));

    //set up Clear button to clear all the input
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearAll()));
}

BaseConverter::~BaseConverter()
{
    delete ui;
}

/*************************************************************
  Name: isInputValid()
  Purpose: checks if the input line is valid based on the initial base
  Arguments/Requirements: initialBaseInput(string), initialBase(int)
  Returns: boolean value
  ***********************************************************/
bool BaseConverter::isInputValid()
{
    QString input = ui->initialBaseInput->text();
    int base = ui->initialBase->currentIndex() + 2;
    int index = input.length() - 1;

    while(index >= 0 && (alpha2Num(input[index]) < base || input[index] == '.'))
        index--;

    if(index == -1)
        return true;
    return false;
}

/*************************************************************
  Name: setValidator()
  Purpose: sets up the input line to only accept valid
           numbers/letters
  Arguments/Requirements: needs a value set in the initalBase
           combo box
  Returns: Nothing
  ***********************************************************/
void BaseConverter::setValidator()
{
    //the base determines which values are valid input
    int base = ui->initialBase->currentIndex() + 2;

    //clears the current input if the input is invalid
    if(!isInputValid())
        ui->initialBaseInput->clear();

    int digitUpperBound = (base <= 10) ? base - 1 : 9;
    QString letterRange = (base > 10) ? "A-" + QString(QChar(base + 54)) : "";
        //QChar(base + 54) is the upper bound of the letter range: the ASCII value of 'A' + (base - 11)

    //the valid characters include none or unlimited amount of numbers and letters from the values calculated above
    QString validChars = "[0-" + QString::number(digitUpperBound) + letterRange + "]{0,}";

    //one optional decimal point is allowed for the input
    QRegExp rx(validChars + ".?" + validChars);

    rx.setCaseSensitivity(Qt::CaseInsensitive);

    QValidator *validator = new QRegExpValidator(rx, this);
    ui->initialBaseInput->setValidator(validator);
}

/*************************************************************
  Name: displayConversion()
  Purpose: displays the conversion from one base to another
           base in targetBaseInput
  Arguments/Requirements: requires valid input in initialBaseInput
  Returns: None
  ***********************************************************/
void BaseConverter::displayConversion()
{
    if(!isInputValid())
        ui->targetBaseInput->setText("Your input is invalid");
    else
    {
        QString input = ui->initialBaseInput->text();
        if(!input.isEmpty())
            ui->targetBaseInput->setText(base2Base(input, ui->initialBase->currentIndex() + 2, ui->targetBase->currentIndex() + 2));
        else
            ui->targetBaseInput->clear();
    }
}

/*************************************************************
  Name: alpha2Num(QChar ch)
  Purpose: converts a character to its integer value
           (ex. 'A'&'a'->11 and '1'->1)
  Arguments/Requirements: characters [0-9, a-z, A-Z]
  Returns: an integer value of a character,
           returns 255 if the character is ivalid
  ***********************************************************/
int BaseConverter::alpha2Num(QChar ch)
{
    if(ch.isDigit())
        return ch.toAscii() - 48;
    else if((ch <= 'a' && ch >= 'z') || (ch <= 'A' && ch >= 'Z'))
        return ch.toUpper().toAscii() - 55;
    else
        return 255;
}

/*************************************************************
  Name: num2Alpha(int num)
  Purpose: converts a number to its character value
           (ex. 11->'A' and 1->'1')
  Arguments/Requirements: integer between 0 and 37
  Returns: a the character value of an integer
  ***********************************************************/
QChar BaseConverter::num2Alpha(int num)
{
    if(num < 10)
        return QChar(num + 48);
    return QChar(num + 55);
}

/*************************************************************
  Name: base2base(QString input, int initialBase, int targetBase)
  Purpose: converts the input from an initial base (initialBase)
           to the target base (targetBase)
  Arguments/Requirements: a string of numbers/letters,
           inital base (int), target base (int)
  Returns: conversion of the number of initialBase to targetBase (int)
  ***********************************************************/
QString BaseConverter::base2Base(QString input, int initialBase, int targetBase)
{
    //---------------------initialBase to decimal conversion-------------------------

    double decConversion = 0;
    if(initialBase == 10)
        decConversion = input.toDouble();
    else
    {
        int decPos = input.indexOf(".");
        if(decPos != -1)    //if there is a decimal point
        {
            //base10 conversion of the whole numbers side
            int leftTotal = 0;
            for(int exp = decPos - 1, index = 0; exp >= 0; exp--, index++)
                leftTotal += alpha2Num(input[index]) * pow(initialBase, exp);

            //base10 conversion of the decimal side
            double rightTotal = 0;
            for(int exp = -1, index = decPos + 1; index < input.length(); exp--, index++)
                rightTotal += alpha2Num(input[index]) * pow(initialBase, exp);
            decConversion = leftTotal + rightTotal;
        }
        else
        {
            //base10 conversion of an integer
            int total = 0;
            for(int exp = input.length() - 1, index = 0; exp >= 0; exp--, index++)
                total += alpha2Num(input[index]) * pow(initialBase, exp);
            decConversion = total;
        }
    }

    //---------------------decimal to targetBase conversion-------------------------
    if(targetBase == 10)
        return QString::number(decConversion);
    QString baseConversion = "";
    int intInput = floor(decConversion);                //whole number portion of the decimal conversion
    double decInput = decConversion - intInput;         //decimal portion of the decimal conversion

    //--------converting the whole number portion of the input---------
    int remainder;
    while(intInput != 0)
    {
        remainder = intInput % targetBase;
        baseConversion += num2Alpha(remainder);
        intInput = intInput / targetBase;
    }
    baseConversion = reverseStr(baseConversion);

    //--------converting the decimal portion of the input--------
    if(decInput)
    {
        //set a max number of repetitions for repeating decimals
        int maxRepetitions = (QString::number(decInput).length()) * 4;

        //add a decimal point
        baseConversion += ".";

        //start calculation
        decInput = decInput * targetBase;
        baseConversion += num2Alpha(floor(decInput));

        int count = 0;
        while(count < maxRepetitions && (decInput - floor(decInput) != 0))
        {
            decInput = (decInput - floor(decInput)) * targetBase;
            baseConversion += num2Alpha(floor(decInput));
            count++;
        }
    }
    return baseConversion;
}

/*************************************************************
  Name: reverseStr()
  Purpose: reverses the letters in a string
  Arguments/Requirements: a string
  Returns: a reversed string
  ***********************************************************/
QString BaseConverter::reverseStr(QString str)
{
    int mid = str.length()/2;
    int last = str.length()-1;
    for(int index = 0; index < mid; index++)
    {
        QChar temp = str[index];
        str[index] = str[last-index];
        str[last-index] = temp;
    }
    return str;
}

/*************************************************************
  Name: swapBases()
  Purpose: swaps between the initial value base and the target
           base values and their inputs
  Arguments/Requirements:
  Returns: Nothing
  ***********************************************************/
void BaseConverter::swapBases()
{
    if(isInputValid())
    {
        QString targetText = ui->targetBaseInput->text();
        QString initialText = ui->initialBaseInput->text();
        ui->initialBaseInput->setText(targetText);
        ui->targetBaseInput->setText(initialText);
    }
    else
    {
        ui->initialBaseInput->clear();
        ui->targetBaseInput->clear();
    }

    int initialBase = ui->initialBase->currentIndex();
    int targetBase = ui->targetBase->currentIndex();
    ui->initialBase->setCurrentIndex(targetBase);
    ui->targetBase->setCurrentIndex(initialBase);
}

/*************************************************************
  Name: clearAll()
  Purpose: clears all input
  Arguments/Requirements: None
  Returns: Nothing
  ***********************************************************/
void BaseConverter::clearAll()
{
    ui->initialBaseInput->clear();
    ui->targetBaseInput->clear();
}
