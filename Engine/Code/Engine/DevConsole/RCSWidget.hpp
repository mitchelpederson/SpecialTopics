#pragma once


class RCSWidget {

public:
	RCSWidget();
	~RCSWidget();

	void Update();
	void Render() const;


private:
	bool m_shouldDisplay = false;
};