#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui{class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    const QString extensionFilters{"*.txt *.csv *.log *.ini *.md *.xml *.json *.yaml *.yml *.tsv *.bat *.sh *.plist *.html *.htm"};

    QString filePath;
    QString folderPath;

    QString outPath;

    bool isFile;

    QStringList oldWord;
    QStringList newWord;

    bool isCaseSensitive;
    QString separator;

    bool isReplacingFileName;
    bool isAddingSuffix;
    QString suffix;

    bool isReplacingOriginalFile;
    bool isCreatingBackup;

private slots:
    void onInMethodChanged();

    // Input Row //
    void onInFileClicked();
    void onInFolderClicked();

    void inFileLineFocusLost();
    void inFolderLineFocusLost();

    // Replace //
    void oldWordLineFocusLost();
    void newWordLineFocusLost();

    // Option Row //
    void onCaseSensitiveClicked();

    void separatorLineFocusLost();

    // Setting Row //
    void onReplaceFileClicked();
    void onAddSuffixClicked();

    // Output Row //
    void onOpenOutputFolderClicked();
    void onCreateBackupClicked();
    void onReplaceOriginalClicked();
    void onSelectOutputFolderClicked();

    void outPathLineFocusLost();
    void suffixLineFocusLost();

    // Bottom Row //
    void onStartButtonClicked();
    void onPreviewButtonClicked();

private:
    Ui::MainWindow *ui;

private:
    void initWarnings();
    void initTips();
    void initConnects();

    void removeDuplicatedSeparatorFromSearchFor();
    void checkReplacementAndUpdateIfValid();

    void checkStartConditions();

    void updateInFileLine();
    void updateInFolderLine();

    void updateOutPath();
};
