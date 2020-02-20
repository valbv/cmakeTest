#pragma once

#include <memory>

#include <QGraphicsProxyWidget>

class PanelWidget;

class PanelItem : public QGraphicsProxyWidget
{
	Q_OBJECT

public:
	explicit PanelItem(QGraphicsItem* parent = nullptr);
	~PanelItem();

//	void OnKeyPressed(QKeyEvent* event) override;
//	void OnKeyReleased(QKeyEvent* event) override;

private:
	PanelWidget* m_panelWidget;
};
