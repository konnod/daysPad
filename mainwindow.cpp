#include "mainwindow.h"
#include "ui_mainwindow.h"

//TODO: config file
//number of past days to keep in file and memory
#define  MAX_DAYS_OUT 40

std::string filename = "dayspad.dmap";
/*
 * It's necessary to set the delimiter to the value
 * that'll be never entered by user
*/
char delimiter = '\0';
std::map<QDate, std::string> map;

//TODO: remove these ones, make one fstream
std::ifstream file_i;
std::ofstream file_o;
QDate selected_day;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Initialization of GUI
 *
 * Method fills the text pane by the today's day description
 */
void MainWindow::start()
{
    selected_day = ui->calendarWidget->selectedDate();
    std::map<QDate, std::string>::iterator it = map.find(selected_day);
    if(it != map.end())
    {
        std::string data = map.at(selected_day);
        ui->plainTextEdit->insertPlainText(QString::fromStdString(data));
    }
}

/**
 * @brief Runs when another day was selected
 *
 * Saves the description of the previous selected day and fills the
 * description of currently selected day if it presents in map
 */
void MainWindow::on_calendarWidget_selectionChanged()
{
    saveToMap();
    ui->plainTextEdit->clear();
    selected_day = ui->calendarWidget->selectedDate();
    if(map.size() > 0)
    {
        std::map<QDate, std::string>::iterator it = map.find(selected_day);
        //Current date has description
        if(it != map.end())
        {
            std::string text = map.at(selected_day);
            ui->plainTextEdit->insertPlainText(QString::fromStdString(text));
        }
    }

}
/**
 * @brief Writes all the map's content to file
 *
 * Method preserves characters ' ' and '\n' from writing to file,
 * 'cause file stream's delimiter is one of that chars.
 * Thats why string that represents one day is placed in one line
 *
 * @return true, if there was no fails while working with file and false otherwise
 */
bool MainWindow::writeMapToFile()
{
    //clear file's content
    file_o.open(filename.c_str(), std::fstream::trunc);
    if (file_o.bad())
    {
        std::cerr<<"Failed to open file\n";
        return false;
    }
    std::map<QDate, std::string>::const_iterator iter = map.begin();
    while(iter != map.end())
    {
        std::string text = iter->second;
        std::replace(text.begin(), text.end(), '\n', '\a');
        std::replace(text.begin(), text.end(), ' ', '\b');
        file_o<<iter->first.toString("dd/MM/yyyy").toStdString()<<delimiter<<text;
        file_o<<"\n";
        iter++;
    }
    file_o.clear();
    file_o.close();
    if(file_i.bad()){
        //TODO: write to log
        std::cerr<<"Failed to close file\n";
        return false;
    }
    return true;
}

/**
 * @brief Reads data, saved to file
 *
 * Each line in file represnts one day
 * The date is separated from its description by the delimiter zero char - '\0'
 *
 * @return true, if there was no fails while working with file and false otherwise
 */
bool MainWindow::readMapFromFile(){
    file_i.open(filename.c_str(), std::fstream::in);
    if (file_i.bad())
    {
        std::cerr<<"Failed to open file\n";
        return false;
    }
    std::string date_string;
    std::string token;
    size_t pos = 0;
    while(file_i>>date_string)
    {
        std::cout<<date_string<<std::endl;
        pos = date_string.find(delimiter);
        if(pos == std::string::npos)
        {
            std::cerr<<"String is not valid\n";
            break;
        }
        token = date_string.substr(0, pos);
        std::cout<<token<<std::endl;
        date_string.erase(0, pos + 1);
        std::cout<<date_string<<std::endl;
        QString str = QString::fromStdString(token);
        QDate date = QDate::fromString(str, "dd/MM/yyyy");
        std::replace(date_string.begin(), date_string.end(), '\a', '\n');
        std::replace(date_string.begin(), date_string.end(), '\b', ' ');
        map.insert(std::pair<QDate,std::string>(date,date_string));
    }
    file_i.close();
    if(file_i.bad()){
        //TODO: write to log
        std::cerr<<"Failed to close file\n";
        return false;
    }
    return true;
}

void MainWindow::on_pushButton_clicked()
{
    saveToMap();
}

/**
 * @brief Saves entered text into map as selected day's description
 *
 * If text is empty, the corresponding element from map is simply deleted
 */
void MainWindow::saveToMap(){
    std::map<QDate, std::string>::iterator it = map.find(selected_day);
    if(it != map.end())
    {
        map.erase(it);
    }
    std::string text = ui->plainTextEdit->toPlainText().toStdString();
    if(!text.empty())
        map.insert(std::pair<QDate,std::string>(selected_day,text));
}

/**
 * @brief Cleanup records, that are out of date
 *
 * The number of days out are defined by MAX_DAYS_OUT constant
 */
void MainWindow::cleanUpMap()
{
    for(std::map<QDate, std::string>::iterator iter = map.begin(); iter != map.end();)
    {
        //case if today is more than MAX_DAYS_OUT ahead
        if(iter->first.year() == QDate::currentDate().year()
                && QDate::currentDate().dayOfYear() - iter->first.dayOfYear() >= MAX_DAYS_OUT)
        {
            map.erase(iter++);
        }
        //if the new year started, calculations somehow changes
        else if(iter->first.year() < QDate::currentDate().year()
                && QDate::currentDate().dayOfYear() + iter->first.daysInYear() - iter->first.dayOfYear() >= MAX_DAYS_OUT)
        {
            map.erase(iter++);
        }
        else
            iter++;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    writeMapToFile();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    writeMapToFile();
}
