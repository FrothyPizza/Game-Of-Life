#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>



class Cell {
public:
	int m_width;
	int m_height;
	float m_cellSize;

	Cell(int w, int h, float size); // Construct with cell size and the number of cells in x and y axis
	virtual~Cell();

	void update(sf::RenderWindow& window, bool shouldUpdate);

	void placeCell(int x, int y) {
		if (x > 0 && x < m_width - 1 && y > 0 && y < m_height - 1) {
			m_state[y * m_width + x] = 1;
		}
	}

	void reset() {
		for (int i = 0; i < m_width * m_height; ++i) {
			m_state[i] = 0;
		}
	}

private:

	bool* m_output;
	bool* m_state;

	void setUp();

	sf::VertexArray m_renderPoints;

	sf::RectangleShape* m_rect;

};


void Cell::setUp() {

	// Set all of state to what you want
	// If you are going to use set(), use m_state[i] = 0;
	for (int i = 0; i < m_width * m_height; ++i)
		//m_state[i] = rand() % 2;
		m_state[i] = 0;

	auto set = [&](int x, int y, std::wstring s) {
		int p = 0;
		for (auto c : s) {
			m_state[y * m_width + x + p] = c == L'#' ? 1 : 0;
			p++;
		}
	};

	// Infinite Growth
	//set(20, 50, L"########.#####...###......#######.#####");

	// Gosper Glider Gun
	set(10, 15, L"........................#............");
	set(10, 16, L"......................#.#............");
	set(10, 17, L"............##......##............##.");
	set(10, 18, L"...........#...#....##............##.");
	set(10, 19, L"##........#.....#...##...............");
	set(10, 20, L"##........#...#.##....#.#............");
	set(10, 21, L"..........#.....#.......#............");
	set(10, 22, L"...........#...#.....................");
	set(10, 23, L"............##.......................");

	

	// R-Pentomino
	/*
	set(40, 50, L"..##.");
	set(40, 51, L".##..");
	set(40, 52, L"..#..");
	*/

}


Cell::Cell(int w, int h, float size) :
	m_width{ w }, m_height{ h }, m_cellSize{ size }, m_renderPoints{ sf::Points, m_height * m_width }
{
	m_output = new bool[m_width * m_height];
	m_state = new bool[m_width * m_height];

	m_rect = new sf::RectangleShape(sf::Vector2f(m_cellSize, m_cellSize));
	m_rect->setFillColor(sf::Color(0, 0, 0));
	
	setUp();

}

Cell::~Cell() {
	delete[] m_output;
	delete[] m_state;
}

void Cell::update(sf::RenderWindow& window, bool shouldUpdate) {
	auto cell = [&](int x, int y) {
		return m_output[y * m_width + x];
	};

	if (shouldUpdate) {
		bool* swap = m_output;
		m_output = m_state;
		m_state = swap;
	}
	else { // this breaks it
		m_state = m_output;
	}


	for (int x = 1; x < m_width - 1; ++x) 
		for (int y = 1; y < m_height - 1; ++y) {

			if (shouldUpdate) {
				int nNeighbors = cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1)
					+ cell(x - 1, y) + cell(x + 1, y)
					+ cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);


				if (cell(x, y) == 1)
					m_state[y * m_width + x] = (nNeighbors == 2) || (nNeighbors == 3);
				else
					m_state[y * m_width + x] = (nNeighbors == 3);
			}

			// Draw here to not have to loop again
			if (cell(x, y) == 1) {
				//m_renderPoints[y * m_width + x].color = sf::Color::White;
				m_rect->setFillColor(sf::Color::White);
				m_rect->setPosition(x * m_cellSize, y * m_cellSize);
				window.draw(*m_rect);
			}
			else {
				//m_renderPoints[y * m_width + x].color = sf::Color::White;
			}

		}
		


}


int main() {
	srand(static_cast<unsigned int>(time(0)));
	
	float cellSize = 5.f;
	int gridWidth = 213;
	int gridHeight = 120;
	Cell cell{ gridWidth, gridHeight, cellSize };

	const int fps{ 10 };

	sf::RenderWindow window(sf::VideoMode(cell.m_width * cell.m_cellSize, cell.m_height * cell.m_cellSize), " ", sf::Style::Close);
	window.setFramerateLimit(fps);

	//start game loop
	while (window.isOpen()) {
		int x = sf::Mouse::getPosition(window).x / cellSize;
		int y = sf::Mouse::getPosition(window).y / cellSize;
		//std::cout << x << " " << y << std::endl;

		//process events
		sf::Event event;

		while (window.pollEvent(event)) {
			//close window : exit
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) return 0;
			}

		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			cell.placeCell(x, y);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) cell.reset();

		bool shouldUpdate = !sf::Keyboard::isKeyPressed(sf::Keyboard::Space);


		//window.clear(sf::Color(255, 255, 255));
		window.clear(sf::Color::Black);

		cell.update(window, shouldUpdate);

		window.display();

		//// Draw and update and pause if space is held
		//if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

		//	window.clear(sf::Color(255, 255, 255));

		//	cell.update(window);

		//	window.display();

		//}


	}
}
