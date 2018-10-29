#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *ui;

    void getFileVersions();

private:
    QDir directory;
    QStringList files;
    QTimer *startTimer;

    void invertEndian(unsigned char *data, int count);

private slots:
    void on_actionBugIssueFeatureRequest_triggered();
    void on_actionCode_on_GitHub_triggered();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void startTimerFired();
    void on_selectDirectoryButton_clicked();
};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
