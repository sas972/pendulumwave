#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <cmath>
#include <numbers>
#include <ranges>
#include <algorithm>
#include <optional> // Required for SFML 3 Event Polling

// --- Simulation Constants ---
// SFML 3 often prefers Vector2u for window sizes
constexpr unsigned int SCREEN_WIDTH = 1800;
constexpr unsigned int SCREEN_HEIGHT = 1000;
constexpr int NUM_PENDULUMS = 25;
constexpr float TOTAL_PERIOD_S = 60.0f;
constexpr int BASE_OSCILLATIONS = 50;

constexpr float MAX_AMPLITUDE_DEG = 22.0f;
constexpr float BOB_RADIUS = 12.0f;

// Colors
const sf::Color BACKGROUND_COLOR{ 15, 15, 30 };
const sf::Color PIVOT_COLOR{ 200, 200, 200 };
const sf::Color STRING_COLOR{ 70, 70, 90 };

/**
 * @struct Pendulum
 * @brief Represents a single pendulum (Updated for SFML 3.0)
 */
struct Pendulum {
    // SFML 3: VertexArray usage is largely the same, but PrimitiveType is a scoped enum
    sf::VertexArray m_string{ sf::PrimitiveType::Lines, 2 };
    sf::CircleShape m_bob;

    float m_angularFrequency{ 0.0f };
    float m_visualLength{ 0.0f };
    float m_amplitudeRad{ 0.0f };
    sf::Vector2f m_pivotPoint;

    void setup(float freq, float length, float amplitude, sf::Vector2f pivot, sf::Color bobColor) {
        m_angularFrequency = freq;
        m_visualLength = length;
        m_amplitudeRad = amplitude;
        m_pivotPoint = pivot;

        m_string[0].position = m_pivotPoint;
        m_string[0].color = STRING_COLOR;
        m_string[1].color = STRING_COLOR;

        m_bob.setRadius(BOB_RADIUS);
        m_bob.setOrigin({BOB_RADIUS, BOB_RADIUS}); // SFML 3: Brace init for Vector2f
        m_bob.setFillColor(bobColor);
        m_bob.setOutlineThickness(2);
        m_bob.setOutlineColor(sf::Color(bobColor.r / 2, bobColor.g / 2, bobColor.b / 2));
    }

    void update(float totalSimTime) {
        float currentAngle = m_amplitudeRad * std::cos(m_angularFrequency * totalSimTime);

        float x = m_pivotPoint.x + m_visualLength * std::sin(currentAngle);
        float y = m_pivotPoint.y + m_visualLength * std::cos(currentAngle);

        sf::Vector2f pos{x, y};
        m_bob.setPosition(pos);
        m_string[1].position = pos;
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_string);
        window.draw(m_bob);
    }
};

sf::Color getColorFromRatio(float ratio) {
    float r = std::max(0.0f, 1.0f - ratio * 2.0f);
    float g = 1.0f - std::abs(ratio - 0.5f) * 2.0f;
    float b = std::max(0.0f, (ratio - 0.5f) * 2.0f);
    
    return sf::Color(
        static_cast<std::uint8_t>(r * 255),
        static_cast<std::uint8_t>(g * 255),
        static_cast<std::uint8_t>(b * 255)
    );
}

int main() {
    // --- SFML 3.0: ContextSettings ---
    // Field names are mostly consistent, but usage is stricter.
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8; // Note: camelCase 'antiAliasingLevel' in some 3.x versions or strict naming
    settings.depthBits = 0;
    settings.stencilBits = 0;
    
    // --- SFML 3.0: Window Creation ---
    // VideoMode takes a Vector2u. Title is a string view.
    // Style is now a scoped enum or bitmask of scoped enums.
    sf::RenderWindow window(
        sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}),
        "C++20 Pendulum Wave Simulator (SFML 3.0)",
        sf::Style::Default,
        sf::State::Windowed, // SFML 3 often requires State in constructor overload, or just settings
        settings
    );

    window.setFramerateLimit(120);

    std::vector<Pendulum> pendulums(NUM_PENDULUMS);

    // --- Physics Setup ---
    const float g = 9.81f;
    const float pi = std::numbers::pi_v<float>;
    const float maxAmplitudeRad = MAX_AMPLITUDE_DEG * (pi / 180.0f);

    float longestPeriod = TOTAL_PERIOD_S / BASE_OSCILLATIONS;
    float longestPhysicsL = g * std::pow(longestPeriod / (2.0f * pi), 2.0f);
    float maxVisualLength = static_cast<float>(SCREEN_HEIGHT) * 0.8f;
    float pixelsPerMeter = maxVisualLength / longestPhysicsL;

    sf::Vector2f pivotPoint{ static_cast<float>(SCREEN_WIDTH) / 2.0f, 50.0f };
    
    sf::CircleShape pivotVisual{ 10.f };
    pivotVisual.setOrigin({10.f, 10.f});
    pivotVisual.setPosition(pivotPoint);
    pivotVisual.setFillColor(PIVOT_COLOR);

    // Initialize Pendulums
    for (int i = 0; i < NUM_PENDULUMS; ++i) {
        float period = TOTAL_PERIOD_S / (BASE_OSCILLATIONS + i);
        float angularFreq = (2.0f * pi) / period;
        float physicsLength = g * std::pow(period / (2.0f * pi), 2.0f);
        float visualLength = physicsLength * pixelsPerMeter;
        float colorRatio = static_cast<float>(i) / (NUM_PENDULUMS - 1);

        pendulums[i].setup(angularFreq, visualLength, maxAmplitudeRad, pivotPoint, getColorFromRatio(colorRatio));
    }

    sf::Clock deltaClock;
    float totalSimTime = 0.0f;
    float timeScale = 1.0f;
    bool isPaused = false;

    // --- SFML 3.0 Game Loop ---
    while (window.isOpen()) {
        // --- NEW EVENT HANDLING ---
        // window.pollEvent() now returns an std::optional<sf::Event>
        // sf::Event is a discriminated union (like std::variant)
        while (const std::optional event = window.pollEvent()) {
            
            // 1. Handle Window Closed
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            // 2. Handle Key Pressed
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // Scoped Enum: sf::Keyboard::Key::...
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Space:
                        isPaused = !isPaused;
                        break;
                    case sf::Keyboard::Key::R:
                        totalSimTime = 0.0f;
                        break;
                    case sf::Keyboard::Key::Up:
                    case sf::Keyboard::Key::Right:
                        timeScale *= 1.2f;
                        break;
                    case sf::Keyboard::Key::Down:
                    case sf::Keyboard::Key::Left:
                        timeScale /= 1.2f;
                        break;
                    case sf::Keyboard::Key::Escape:
                        window.close();
                        break;
                    default: break;
                }
            }
            // 3. Handle Resize (Optional, but good practice)
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea({0, 0}, {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
                window.setView(sf::View(visibleArea));
            }
        }

        // Update
        float dt = deltaClock.restart().asSeconds();
        if (!isPaused) {
            totalSimTime += dt * timeScale;
        }

        std::ranges::for_each(pendulums, [totalSimTime](auto& p) {
            p.update(totalSimTime);
        });

        // Draw
        window.clear(BACKGROUND_COLOR);
        window.draw(pivotVisual);
        
        std::ranges::for_each(pendulums, [&window](const auto& p) {
            p.draw(window);
        });

        window.display();
    }

    return 0;
}