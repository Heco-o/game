#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <random>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include <iostream>
#include <stdexcept>

#include <chrono>
#include <ranges>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <concepts>

//constexpr int WIDTH = 1280;
//constexpr int HEIGHT = 700;

std::atomic<bool> running{true};
std::atomic<uint32_t> inputState{0};
std::atomic<uint32_t> FPS{0}, TPS{0};

std::random_device rd;
std::mt19937 gen(rd());

struct Input {
	glm::fvec2 v;
};

Input getInput() {
	const uint32_t state = inputState.load(std::memory_order_relaxed);

	return {{
		static_cast<int16_t>(state & 0xFFFF) / 32767.0f,
		static_cast<int16_t>(state >> 16) / 32767.0f
	}};
}

inline constexpr int pyMod(const int dividend, const int divisor) {
	const int result = dividend % divisor;
	return result < 0 ? result + divisor : result;
}

inline constexpr int floorDiv(const int value, const int divisor) {
	return value / divisor - (value % divisor < 0);
}

struct Position {
	glm::fvec2 v;
};

struct Velocity {
	glm::fvec2 v;
};

struct Rectangle {
	glm::fvec2 v;
};

struct Circle {
	float radius;
};

struct Renderable {
	sf::Color color;
};

struct Player{};
struct IsRenderUpdated{};
/*
struct Wall {
	glm::fvec2 pos, size;
};

template <std::size_t N>
struct Level {
	std::array<Wall, N> walls;
	glm::fvec2 startPosition, finishPosition;
};



constexpr std::array<Level<25>, 1> levels{{
	{
	.walls = {{
			// Outer boundaries: start opening on the left, exit at the top.
			{glm::fvec2{   0,   0}, glm::fvec2{1050, 12}},
			{glm::fvec2{1190,   0}, glm::fvec2{  90, 12}},
			{glm::fvec2{   0, 688}, glm::fvec2{1280, 12}},
			{glm::fvec2{   0,  12}, glm::fvec2{  12, 548}},
			{glm::fvec2{   0, 675}, glm::fvec2{  12,  13}},
			{glm::fvec2{1268,  12}, glm::fvec2{  12, 676}},
	
			// Main horizontal partitions.
			{glm::fvec2{  12, 560}, glm::fvec2{1148, 12}},
			{glm::fvec2{ 120, 420}, glm::fvec2{1148, 12}},
			{glm::fvec2{  12, 280}, glm::fvec2{1148, 12}},
			{glm::fvec2{ 120, 140}, glm::fvec2{1148, 12}},
	
			// First corridor: travel from right to left.
			{glm::fvec2{ 900, 438}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 650, 492}, glm::fvec2{  12, 56}},
			{glm::fvec2{ 400, 438}, glm::fvec2{  12, 60}},
	
			// Second corridor: travel from left to right.
			{glm::fvec2{ 300, 302}, glm::fvec2{  12, 58}},
			{glm::fvec2{ 560, 352}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 820, 302}, glm::fvec2{  12, 58}},
			{glm::fvec2{1080, 352}, glm::fvec2{  12, 60}},
	
			// Third corridor: travel from right to left.
			{glm::fvec2{ 900, 162}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 650, 212}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 400, 162}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 180, 212}, glm::fvec2{  12, 60}},
	
			// Final corridor: travel from left to right.
			{glm::fvec2{ 240,  12}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 500,  80}, glm::fvec2{  12, 48}},
			{glm::fvec2{ 760,  12}, glm::fvec2{  12, 60}},
			{glm::fvec2{ 980,  80}, glm::fvec2{  12, 48}}
		}},
		.startPosition = {24.f, 620.f},
		.finishPosition = {1080.f, 0.f}
	}
}};
*/

struct RenderObject {
	enum class Shape {
		Circle, Rectangle
	};
	
	Position pos;
	sf::Color color;
	Shape shape;
	float sizeX, sizeY; 
};

struct RenderCommand {
	enum class Type {
		Update, Destroy
	};
	
	Type type;
	RenderObject object;
};

struct RenderQueue {
	private:
	std::unordered_map<entt::entity, RenderCommand> renderCommandsBuffer;
	std::mutex mutex;
	bool updated = false;
	
	public:
	inline void update(std::invocable<std::unordered_map<entt::entity, RenderCommand>&> auto&& callback) {
		std::lock_guard<std::mutex> lock(mutex);
		std::forward<decltype(callback)>(callback)(renderCommandsBuffer);
		updated = true;
	}
	
	inline bool consume(std::unordered_map<entt::entity, RenderCommand>& consumerRenderCommands) {
		std::lock_guard<std::mutex> lock(mutex);
		if (updated) {
			consumerRenderCommands.clear();
			
			std::swap(renderCommandsBuffer, consumerRenderCommands);
			
			updated = false;
			return true;
		}
		
		return false;
	}
};

RenderQueue renderQueue;

void logicThread(entt::registry& registry) {
	const auto groupForRenderCommands = registry.group<IsRenderUpdated>(entt::get<const Position, const Renderable>);
	
	using Clock = std::chrono::steady_clock;
	
	//constexpr double TARGET_TPS = 480.0;
	//constexpr auto tickDuration = std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / TARGET_TPS));
	
	Clock::time_point previousTime = Clock::now(), currentTime;
	//Clock::time_point nextTick = previousTime;
	
	double deltaTime = 0.0, tpsTimer = 0.0, tps = 0.0;
	float dt = 0.0f;
	
	uint32_t tickCount = 0;
	
	const entt::entity playerEntity = registry.view<Player>().front();
	while (running.load(std::memory_order_relaxed)) {
		currentTime = Clock::now();
		
		deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
		dt = static_cast<float>(deltaTime);
		
		previousTime = currentTime;
		
		
		const Input input = getInput(); //Input Stage
		
		
		Position& currentPos = registry.get<Position>(playerEntity); // Calculation & Info
		const Velocity& currentVel = registry.get<const Velocity>(playerEntity);
		const Rectangle& size = registry.get<const Rectangle>(playerEntity);
		const Position nextPos{currentPos.v + input.v * currentVel.v * dt};
		
		
		if (currentPos.v != nextPos.v) { // Collision Dection & Resolve
			/*
			bool changeX = true, changeY = true;
			
			for (const auto& wall : levels[0].walls) {
				// Resolve X collision
				if (
					nextPos.v.x < wall.pos.x + wall.size.x &&
					nextPos.v.x + size.v.x > wall.pos.x &&
					currentPos.v.y < wall.pos.y + wall.size.y &&
					currentPos.v.y + size.v.y > wall.pos.y
				) {
					changeX = false;
				}
			
				// Resolve Y collision
				if (
					nextPos.v.y < wall.pos.y + wall.size.y &&
					nextPos.v.y + size.v.y > wall.pos.y &&
					currentPos.v.x < wall.pos.x + wall.size.x &&
					currentPos.v.x + size.v.x > wall.pos.x
				) {
					changeY = false;
				}
			}
			
			if (changeX && nextPos.v.x + size.v.x <= WIDTH && nextPos.v.x >= 0) {
				currentPos.v.x = nextPos.v.x;
				registry.emplace_or_replace<IsRenderUpdated>(playerEntity);
			} if (changeY && nextPos.v.y + size.v.y <= HEIGHT && nextPos.v.y >= 0) {
				currentPos.v.y = nextPos.v.y;
				registry.emplace_or_replace<IsRenderUpdated>(playerEntity);
			}
			*/
			
			currentPos.v = nextPos.v;
			registry.emplace_or_replace<IsRenderUpdated>(playerEntity);
		}
		
		if (!groupForRenderCommands.empty()) {
			renderQueue.update([&](std::unordered_map<entt::entity, RenderCommand>& renderCommandsBuffer) {
				groupForRenderCommands.each([&](const entt::entity entity, const Position& pos, const Renderable& ren) {
					RenderObject object{pos, ren.color};
					
					if (const auto* circle = registry.try_get<Circle>(entity)) {
						object.shape = RenderObject::Shape::Circle;
						object.sizeX = object.sizeY = circle->radius;
					} else if (const auto* rect = registry.try_get<Rectangle>(entity)) {
						object.shape = RenderObject::Shape::Rectangle;
						object.sizeX = rect->v.x;
						object.sizeY = rect->v.y;
					} else {
						throw std::runtime_error("Entity has no shape.");
					}
					
					renderCommandsBuffer.insert_or_assign(entity, RenderCommand{RenderCommand::Type::Update,
						std::move(object)
					});
				});
			});
			
			registry.clear<IsRenderUpdated>();
		}
		
		
		++tickCount;
		tpsTimer += deltaTime;
		
		if (tpsTimer >= 1.0) {
			tps = tickCount / tpsTimer;
			TPS.store(tps, std::memory_order_relaxed);
			--tpsTimer;
			tickCount = 0;
		}
		
		
		//nextTick += tickDuration;
		//std::this_thread::sleep_until(nextTick);
		
	}
}

entt::entity createPlayer(entt::registry& registry, const float posX, const float posY, const sf::Color color, const float size, const float vx, const float vy) {
	const entt::entity playerEntity = registry.create();
	
	registry.emplace<Player>(playerEntity);
	registry.emplace<Position>(playerEntity, glm::fvec2{posX, posY});
	registry.emplace<Velocity>(playerEntity, glm::fvec2{vx, vy});
	registry.emplace<Renderable>(playerEntity, color);
	registry.emplace<Rectangle>(playerEntity, glm::fvec2{size, size});
	
	registry.emplace<IsRenderUpdated>(playerEntity);
	
	return playerEntity;
}

int main() {
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "", sf::State::Fullscreen);
	
	//window.setVerticalSyncEnabled(false);
	//window.setFramerateLimit(60);
	
	sf::Font font;
	if (!font.openFromFile("ProggyClean.ttf")) {
		std::cerr << "Error loading font" << std::endl;
		return -1;
	}
	
	entt::registry registry;
	
	createPlayer(registry, window.getSize().x, window.getSize().y, sf::Color::Blue, 20, 180, 180);
	
	std::unordered_map<entt::entity, RenderCommand> renderCommands;
	std::unordered_map<entt::entity, RenderObject> renderObjects;
	
	std::jthread thread(logicThread, std::ref(registry));
	
	sf::Text fpsText(font);
	sf::Text tpsText(font);
	
	fpsText.setCharacterSize(30);
	tpsText.setCharacterSize(30);
	fpsText.setFillColor(sf::Color::Black);
	tpsText.setFillColor(sf::Color::Black);
	fpsText.setPosition({0.f, 0.f});
	tpsText.setPosition({0.f, 40.f});
	
	using Clock = std::chrono::steady_clock;

	Clock::time_point lastUpdate = Clock::now();
	std::uint32_t frameCount = 0;

	double fps = 0.0;
	
	sf::CircleShape circleShape;
	sf::RectangleShape rectangleShape;
	while (running.load(std::memory_order_relaxed)) {
		while (const std::optional<sf::Event> event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				running.store(false, std::memory_order_relaxed);
			}
		}
		
		if (renderQueue.consume(renderCommands)) {
			for (const auto& [entity, command] : renderCommands) {
				if (command.type == RenderCommand::Type::Update) {
					renderObjects.insert_or_assign(entity, command.object);
				} else if (command.type == RenderCommand::Type::Destroy) {
					renderObjects.erase(entity);
				}
			}
		}
		
		window.clear(sf::Color::White);
		/*
		for (const Wall& wall : levels[0].walls) {
			rectangleShape.setPosition({wall.pos.x, wall.pos.y});
			rectangleShape.setSize({wall.size.x, wall.size.y});
			rectangleShape.setFillColor(sf::Color(2155905279));
			
			window.draw(rectangleShape);
		}
		*/
		for (const RenderObject& object : std::views::values(renderObjects)) {
			if (object.shape == RenderObject::Shape::Circle) {
				circleShape.setRadius(object.sizeX);
				circleShape.setPosition({object.pos.v.x, object.pos.v.y});
				circleShape.setFillColor(object.color);
				
				window.draw(circleShape);
			} else if (object.shape == RenderObject::Shape::Rectangle) {
				rectangleShape.setSize({object.sizeX, object.sizeY});
				rectangleShape.setPosition({object.pos.v.x, object.pos.v.y});
				rectangleShape.setFillColor(object.color);
				
				window.draw(rectangleShape);
			}
		}
		
		++frameCount;
		
		const auto now = Clock::now();
		const std::chrono::duration<double> elapsed = now - lastUpdate;
		
		if (elapsed.count() >= 1.0) {
			fps = frameCount / elapsed.count();
			FPS.store(fps, std::memory_order_relaxed);
			frameCount = 0;
			lastUpdate = now;
			
			fpsText.setString("FPS: " + std::to_string(std::lround(fps)));
			tpsText.setString("TPS: " + std::to_string(std::lround(TPS.load(std::memory_order_relaxed))));
		}
		
		window.draw(fpsText);
		window.draw(tpsText);
		
		window.display();
	}
}