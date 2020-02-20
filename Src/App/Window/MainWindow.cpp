#include "MainWindow.h"

#include <QBoxLayout>
#include <QSpinBox>

#include "../../Lib/Widget/MFrame.h"
#include "../../Lib/Widget/WidgetUtils.h"

struct MainWindow::Impl
{
    Impl(MainWindow* mainWindow)
        : mainWindow(mainWindow)
        , myGraphicView(new MyGraphicView(mainWindow))
    {
        mainWindow->setCentralWidget(myGraphicView);
    }

    MainWindow* mainWindow;
    MyGraphicView* myGraphicView;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_impl(std::make_unique<Impl>(this))
{}

MainWindow::~MainWindow() = default;
