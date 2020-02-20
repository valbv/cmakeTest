#pragma once

#include <memory>

#include <QMainWindow>

#include "MyGraphicView.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
