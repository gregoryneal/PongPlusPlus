#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>

float GetBallAngle() {
	float angle;

	do {
		angle = ((float)rand() / (float)RAND_MAX) * 365.f;
	} while (std::abs(std::cos(angle)) < 0.99f);

	return angle;
}

void Normalize(sf::Vector2f& vec) {
	float magnitude = std::sqrtf((vec.x * vec.x) + (vec.y * vec.y));
	vec.x /= magnitude;
	vec.y /= magnitude;
}

int main() {

	std::srand(static_cast<unsigned int>(std::time(NULL)));
	
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
	const float playerXPos = 20.f;
	const float scoreHeight = 20.f;
	const float scoreOffset = 300.f;

	bool paused = true;
	bool restart = false;

	sf::Font pauseFont;
	if (!pauseFont.loadFromFile("resources/fonts/vt323.ttf")) {
		// Error
	}

	// moveSpeed needs to be set this high because it's being multiplied by
	// deltaTime which is a small number.
	float defaultMoveSpeed = 500.f;
	float moveSpeed = defaultMoveSpeed;
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight), "Pong++", sf::Style::Titlebar | sf::Style::Close);

	// Create the player
	sf::Vector2f playerDimensions = sf::Vector2f(30.f, 120.f);
	sf::RectangleShape player(playerDimensions);
	player.setFillColor(sf::Color::White);
	player.setOrigin(0.f, playerDimensions.y / 2.f);
	player.setPosition(sf::Vector2f(playerXPos, gameHeight / 2.f));

	// Player score
	int pScore = 0;
	sf::Text playerScore;
	playerScore.setCharacterSize(100);
	playerScore.setFont(pauseFont);
	playerScore.setString(std::to_string(pScore));
	playerScore.setOrigin(playerScore.getLocalBounds().width / 2.f, playerScore.getLocalBounds().height / 2.f);
	playerScore.setPosition((gameWidth / 2.f) - scoreOffset, scoreHeight);

	// Create the opponent
	sf::Vector2f enemyDimensions = sf::Vector2f(30.f, 120.f);
	sf::RectangleShape enemy(enemyDimensions);
	enemy.setFillColor(sf::Color::Red);
	enemy.setOrigin(enemyDimensions.x, enemyDimensions.y / 2.f);
	enemy.setPosition(sf::Vector2f(gameWidth - playerXPos, gameHeight / 2.f));

	// Opponent score
	int eScore = 0;
	sf::Text enemyScore;
	enemyScore.setCharacterSize(100);
	enemyScore.setFont(pauseFont);
	enemyScore.setString(std::to_string(eScore));
	enemyScore.setOrigin(enemyScore.getLocalBounds().width / 2.f, enemyScore.getLocalBounds().height / 2.f);
	enemyScore.setPosition((gameWidth / 2.f) + scoreOffset, scoreHeight);

	// Create the ball
	float ballSpeed = 500.f;
	float radius = 10.f;
	float ballAngle = GetBallAngle();
	sf::CircleShape ball(radius);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(ball.getLocalBounds().width / 2.f, ball.getLocalBounds().height / 2.f);
	ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
	sf::Vector2f ballDirection = sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle));

	// Create pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(pauseFont);
	pauseMessage.setString("Paused, press space to start.\nPress escape to quit.\nR to restart.");
	pauseMessage.setCharacterSize(50);
	pauseMessage.setColor(sf::Color::White);
	float textWidth = pauseMessage.getLocalBounds().width;
	float textHeight = pauseMessage.getLocalBounds().height;
	pauseMessage.setPosition((gameWidth - textWidth) / 2.f, ((gameHeight / 2.f) - textHeight) / 2.f);

	// Starts the clock which measures the frame rate
	sf::Clock clock;

	// The time it takes to run a single game loop
	float deltaTime;

	while (window.isOpen()) {

		// Get the time it took this frame to run.
		deltaTime = clock.restart().asSeconds();

		sf::Event event;

		// Pop any pending events off the event stack and process them
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (paused && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close();
			}

			if (paused && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
				player.setPosition(sf::Vector2f(playerXPos, gameHeight / 2.f));
				enemy.setPosition(sf::Vector2f(gameWidth - playerXPos, gameHeight / 2.f));
				ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
				pauseMessage.setString("Paused, press space to start.\nPress escape to quit.\nR to restart.");
				float textWidth = pauseMessage.getLocalBounds().width;
				float textHeight = pauseMessage.getLocalBounds().height;
				pauseMessage.setPosition((gameWidth - textWidth) / 2.f, ((gameHeight / 2.f) - textHeight) / 2.f);
				ballAngle = GetBallAngle();
				ballDirection = sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle));

				restart = false;
				paused = false;
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				paused = !paused;

				// If we win or lose
				if (restart) {

					player.setPosition(sf::Vector2f(playerXPos, gameHeight / 2.f));
					enemy.setPosition(sf::Vector2f(gameWidth - playerXPos, gameHeight / 2.f));
					ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
					pauseMessage.setString("Paused, press space to start.\nPress escape to quit.\nR to restart.");
					float textWidth = pauseMessage.getLocalBounds().width;
					float textHeight = pauseMessage.getLocalBounds().height;
					pauseMessage.setPosition((gameWidth - textWidth) / 2.f, ((gameHeight / 2.f) - textHeight) / 2.f);
					ballAngle = GetBallAngle();
					ballDirection = sf::Vector2f(std::cos(ballAngle), std::sin(ballAngle));

					restart = false;
				}
			}
		}

		if (!paused) {

			// Get keyboard input for player movement
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {

				// Check our bounds to make sure we aren't off screen
				if (enemy.getGlobalBounds().top >= 0) {
					enemy.move(sf::Vector2f(0, -moveSpeed * deltaTime));
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {

				// Check our bounds to make sure we aren't off screen
				if (enemy.getGlobalBounds().top + enemyDimensions.y <= gameHeight) {
					enemy.move(0, moveSpeed * deltaTime);
				}
			}

			// Second player movement
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				if (player.getGlobalBounds().top >= 0) {
					player.move(0, -moveSpeed * deltaTime);
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				if (player.getGlobalBounds().top + playerDimensions.y <= gameHeight) {
					player.move(0, moveSpeed * deltaTime);
				}
			}

			// Check ball collisions
			// Player paddle
			sf::Vector2f ballPos = ball.getPosition();
			sf::Vector2f playerPos = player.getPosition();
			sf::Vector2f enemyPos = enemy.getPosition();
			if (ballPos.x - radius <= playerPos.x + playerDimensions.x && std::abs(ballPos.y - playerPos.y) <= (playerDimensions.y / 2.f) + radius) {
				// Hit player paddle
				ballDirection.x *= -1.f;
				float mag = ballPos.y >= playerPos.y ? 1.f : -1.f;
				ballDirection.y += mag * (float)(rand() % 10) / 60.f;
				Normalize(ballDirection);
			}

			// Enemy paddle
			if (ballPos.x + radius >= enemyPos.x - enemyDimensions.x && std::abs(ballPos.y - enemyPos.y) <= (enemyDimensions.y / 2.f) + radius) {
				// Hit enemy paddle
				ballDirection.x *= -1.f;
				float mag = ballPos.y >= enemyPos.y ? 1.f : -1.f;
				ballDirection.y += mag * (float)(rand() % 10) / 60.f;
				Normalize(ballDirection);
			}

			// Top wall
			if (ballPos.y - radius <= 0) {
				// Hit top wall
				ballDirection.y *= -1.f;
			}

			// Bottom wall
			if (ballPos.y + radius >= gameHeight) {
				// Hit bottom wall
				ballDirection.y *= -1.f;
			}

			// Left wall
			if (ballPos.x - radius <= 0) {
				// Lose
				paused = true;
				restart = true;
				pauseMessage.setString("Red wins! Press space to restart.\nPress escape to quit.\nR to restart.");
				float textWidth = pauseMessage.getLocalBounds().width;
				float textHeight = pauseMessage.getLocalBounds().height;
				pauseMessage.setPosition((gameWidth - textWidth) / 2.f, ((gameHeight / 2.f) - textHeight) / 2.f);

				enemyScore.setString(std::to_string(++eScore));
				enemyScore.setOrigin(enemyScore.getLocalBounds().width / 2.f, enemyScore.getLocalBounds().height / 2.f);
				enemyScore.setPosition((gameWidth / 2.f) + scoreOffset, scoreHeight);
			}

			// Right wall
			if (ballPos.x + radius >= gameWidth) {
				// Win
				paused = true;
				restart = true;
				pauseMessage.setString("White win! Press space to restart.\nPress escape to quit.");
				float textWidth = pauseMessage.getLocalBounds().width;
				float textHeight = pauseMessage.getLocalBounds().height;
				pauseMessage.setPosition((gameWidth - textWidth) / 2.f, ((gameHeight / 2.f) - textHeight) / 2.f);

				playerScore.setString(std::to_string(++pScore));
				playerScore.setOrigin(playerScore.getLocalBounds().width / 2.f, playerScore.getLocalBounds().height / 2.f);
				playerScore.setPosition((gameWidth / 2.f) - scoreOffset, scoreHeight);
			}

			sf::Vector2f ballVelocity = ballDirection * ballSpeed * deltaTime;
			ball.move(ballVelocity);
		}

		// Clear the entire screen with black
		window.clear(sf::Color::Black);

		// Draw graphics
		// Draw pause message if paused
		if (paused) {
			window.draw(pauseMessage);
		}
		window.draw(player);
		window.draw(enemy);
		window.draw(ball);
		window.draw(enemyScore);
		window.draw(playerScore);

		window.display();
	}
	
	return 0;
}
