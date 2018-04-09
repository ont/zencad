#ifndef DZENCAD_DZENWIDGET_H
#define DZENCAD_DZENWIDGET_H

#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMainWindow>
#include <zencad/DisplayWidget.h>

class ZenWidget : public QMainWindow {
	Q_OBJECT

    //! the exit action.
    QAction* mStlExport;
    QAction* mExitAction;

    //! show the about info action.
    QAction* mAboutAction;

    //! the menus of the application.
    QMenu* mFileMenu;
    //QMenu* mViewMenu;
    //QMenu* mPrimitiveMenu;
    //QMenu* mModelingMenu;
    QMenu* mHelpMenu;

    //! the toolbars of the application.
    //QToolBar* mViewToolBar;
    //QToolBar* mNavigateToolBar;
    //QToolBar* mPrimitiveToolBar;
    //QToolBar* mModelingToolBar;
    //QToolBar* mHelpToolBar;

public:
    DisplayWidget* display;

private:
    void createActions();
    void createMenus();
    void createToolbars();

private slots:
    void about(void);
    void export_stl();

public:
	ZenWidget(QWidget* parent = nullptr);


};

#endif