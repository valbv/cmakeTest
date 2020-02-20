#pragma once

#include <memory>

#include <QWidget>

class PanelWidget : public QWidget
{
    Q_OBJECT

signals:
    void RequestAreaChange(const QSize& areaSize);

public:
    explicit PanelWidget(QWidget* parent = 0);
    ~PanelWidget();

//    void HandleKeyEvent(QKeyEvent* event);
//
//protected:
//    void changeEvent(QEvent* event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
