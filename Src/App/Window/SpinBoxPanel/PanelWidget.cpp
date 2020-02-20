#include "PanelWidget.h"

#include <QBoxLayout>
#include <QSpinBox>

#include "Widget/MFrame.h"
#include "Widget/WidgetUtils.h"

struct PanelWidget::Impl
{
    Impl(PanelWidget* panelWidget)
        : CONSTRUCT_WIDGET(mainFrame, panelWidget)
        , CONSTRUCT_WIDGET(widthSpinBox, mainFrame)
        , CONSTRUCT_WIDGET(heightSpinBox, mainFrame)
    {
        QHBoxLayout* mainLayout = Util::createZeroLayout<QHBoxLayout>(mainFrame);
        mainLayout->addWidget(widthSpinBox);
        mainLayout->addStretch();
        mainLayout->addWidget(heightSpinBox);

        QVBoxLayout* panelWidgetLayout = Util::createZeroLayout<QVBoxLayout>(panelWidget);
        panelWidgetLayout->addWidget(mainFrame);
    }

    PanelWidget* panelWidget;
    
    MFrame* mainFrame;
    QSpinBox* widthSpinBox;
    QSpinBox* heightSpinBox;
};

PanelWidget::PanelWidget(QWidget* parent)
    : m_impl(std::make_unique<Impl>(this))
{}

PanelWidget::~PanelWidget() = default;
