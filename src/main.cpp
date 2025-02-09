#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define SUN_RADIUS 80.0f
#define SUN_COLOR_R 242
#define SUN_COLOR_G 242
#define SUN_COLOR_B 36

#define OBJECT_RADIUS 80.0f

#define BEAM_THICKNESS 1.5f
#define BEAM_COLOR_R 245
#define BEAM_COLOR_G 220
#define BEAM_COLOR_B 0
#define BEAM_COUNT 360

constexpr float RADIAN_TO_DEGREE = static_cast<float>(180.0f / M_PI);
constexpr float DEGREE_TO_RADIAN = static_cast<float>(M_PI / 180.0f);

sf::Vector2f checkCollision(sf::CircleShape &sun, sf::CircleShape &object) {
  const sf::Vector2f sunPos = sun.getPosition() + sf::Vector2f{SUN_RADIUS, SUN_RADIUS};
  const sf::Vector2f objectPos = object.getPosition() + sf::Vector2f{OBJECT_RADIUS, OBJECT_RADIUS};

  const float distanceX = objectPos.x - sunPos.x;
  const float distanceY = objectPos.y - sunPos.y;
  const float distance = std::hypot(distanceX, distanceY);

  if (distance < OBJECT_RADIUS) {
    return sf::Vector2f({0.0f, 360.0f});
  }

  const float offset = std::asin(OBJECT_RADIUS / distance);
  const float centerDegree = std::atan2(distanceY, distanceX);

  const float firstDegree = (centerDegree - offset) * RADIAN_TO_DEGREE;
  const float secondDegree = (centerDegree + offset) * RADIAN_TO_DEGREE;

  return {std::fmod(firstDegree + 360.0f, 360.0f), std::fmod(secondDegree + 360.0f, 360.0f)};
}

float getBeamSize(sf::CircleShape &sun, sf::CircleShape &object, float angle) {
  const sf::Vector2f sunPos = sun.getPosition() + sf::Vector2f{SUN_RADIUS, SUN_RADIUS};
  const sf::Vector2f objectPos = object.getPosition() + sf::Vector2f{OBJECT_RADIUS, OBJECT_RADIUS};

  const sf::Vector2f direction = {std::cos(angle * DEGREE_TO_RADIAN), std::sin(angle * DEGREE_TO_RADIAN)};
  const sf::Vector2f m = sunPos - objectPos;

  const float md = m.x * direction.x + m.y * direction.y;
  const float mLenSq = m.x * m.x + m.y * m.y;
  const float rSq = OBJECT_RADIUS * OBJECT_RADIUS;

  const float discriminant = md * md - (mLenSq - rSq);
  float t = -md - std::sqrt(discriminant);

  const sf::Vector2f hitPoint = sunPos + t * direction;
  return std::hypot(hitPoint.x - sunPos.x, hitPoint.y - sunPos.y);
};

int main() {
  sf::ContextSettings settings;
  settings.antiAliasingLevel = 8;

  sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Ray Tracing",
                          sf::Style::Default, sf::State::Windowed, settings);
  window.setVerticalSyncEnabled(true);

  sf::CircleShape sun(SUN_RADIUS);
  sun.setPosition({300.0f, 200.0f});
  sun.setFillColor(sf::Color(SUN_COLOR_R, SUN_COLOR_G, SUN_COLOR_B));

  sf::CircleShape object(OBJECT_RADIUS);
  object.setPosition({800.0f, 200.0f});
  object.setFillColor(sf::Color::White);

  std::vector<sf::RectangleShape> beams;
  beams.reserve(BEAM_COUNT);

  for (int i = 0; i < BEAM_COUNT; i++) {
    const float angle = 360.0f / BEAM_COUNT * i;
    sf::RectangleShape beam({1000.0f, BEAM_THICKNESS});

    beam.setPosition({380.0f, 280.0f});
    beam.setRotation(sf::degrees(angle));
    beam.setFillColor({BEAM_COLOR_R, BEAM_COLOR_G, BEAM_COLOR_B});

    beams.push_back(beam);
  };

  bool holdingMouse = false;
  while (window.isOpen()) {
    window.clear(sf::Color::Black);

    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();

      } else if (event->getIf<sf::Event::MouseButtonPressed>()) {
        holdingMouse = true;

      } else if (event->getIf<sf::Event::MouseButtonReleased>()) {
        holdingMouse = false;

      } else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        if (!holdingMouse) {
          continue;
        };

        object.setPosition(static_cast<sf::Vector2f>(mouseMoved->position) - sf::Vector2f(OBJECT_RADIUS, OBJECT_RADIUS));
      }
    };

    const auto collision = checkCollision(sun, object);
    for (int i = 0; i < BEAM_COUNT; i++) {
      sf::RectangleShape &beam = beams[i];
      beam.setSize({1000.0f, BEAM_THICKNESS});

      if (collision.x == 0.0f && collision.y == 360.0f) {
        beam.setSize({0.0f, BEAM_THICKNESS});
        continue;
      }

      const float angle = 360.0f / BEAM_COUNT * i;
      if (collision.x > collision.y) {
        if (angle > collision.x || angle < collision.y) {
          const float beamSize = getBeamSize(sun, object, angle);
          beam.setSize({beamSize, BEAM_THICKNESS});
        };
      } else {
        if (angle >= collision.x && angle <= collision.y) {
          const float beamSize = getBeamSize(sun, object, angle);
          beam.setSize({beamSize, BEAM_THICKNESS});
        };
      }

      window.draw(beam);
    };

    window.draw(object);
    window.draw(sun);
    window.display();
  };

  return 0;
}
