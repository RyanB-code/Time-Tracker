#pragma once

#include <iomanip>
#include <iostream>

#include "DateTime.h"

namespace Display {


	class DisplayManager
	{
	public:
		static DisplayManager& getInstance();
		~DisplayManager();

		bool run();

		void home();

	private:
		DisplayManager();
		const DisplayManager& operator=(const DisplayManager& app) = delete;

		int m_screenWidth	{ 50 };
		int m_screenHeight	{ 50 };

		Timestamp m_currentTime{};
	};



	std::string drawHorizontalLine(int width);

}