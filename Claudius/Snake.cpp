#include "Snake.h"
#include <ranges>
#include <cmath>

// Constructors
Snake::Snake(Vector2 position, int size, int bodyParts)
    : m_position{ position }
    , m_size{ size }
{
    SDL_Rect bodyPartRect{ position.x, position.y, size, size };

    m_head() = bodyPartRect;
    m_tail() = bodyPartRect;

	for (; bodyParts > minimumBodyParts; bodyParts--)
	{
		Grow();
	}
}

void Snake::Update(float deltaTime) noexcept
{
    ProcessInput();

	m_position += speed * deltaTime * m_direction;

	UpdateHead();
	UpdateBody();
}

void Snake::Render(SDL_Renderer* renderer) const noexcept
{
    // Draw all body parts except for the head.
    SDL_SetRenderDrawColor(renderer, bodyColor.r, bodyColor.g, bodyColor.b, bodyColor.a);
    for (const auto& bodyPart : m_bodyParts | std::views::drop(1))
    {
        SDL_RenderFillRect(renderer, &bodyPart);
    }

	// Draw the head.
	SDL_SetRenderDrawColor(renderer, headColor.r, headColor.g, headColor.b, headColor.a);
	SDL_RenderFillRect(renderer, &Head());
}

void Snake::Grow()
{
    m_bodyParts.push_back(Tail());
}

void Snake::Shrink() noexcept
{
    if (BodyPartsSize() <= minimumBodyParts)
    {
        return;
    }
    
    m_bodyParts.pop_back();
}

void Snake::ProcessInput() noexcept
{
	Vector2 newDirection{ Vector2::zero() };
	if (Input::GetKey(SDL_Scancode::SDL_SCANCODE_LEFT))
	{
		newDirection = Vector2::left();
	}
	else if (Input::GetKey(SDL_Scancode::SDL_SCANCODE_RIGHT))
	{
		newDirection = Vector2::right();
	}
	else if (Input::GetKey(SDL_Scancode::SDL_SCANCODE_UP))
	{
		newDirection = Vector2::down();
	}
	else if (Input::GetKey(SDL_Scancode::SDL_SCANCODE_DOWN))
	{
		newDirection = Vector2::up();
	}

	// Change the snake's direction. Fails if the direction wouldn't change or if it would completely turn around.
	if (newDirection == Vector2::zero()
		|| newDirection == this->m_direction
		|| newDirection == -this->m_direction)
	{
		return;
	}

	this->m_direction = newDirection;

	// Snap the head to the grid and updaate the head and body.
	m_position /= m_size;
	m_position.x = std::roundf(m_position.x);
	m_position.y = std::roundf(m_position.y);
	m_position *= m_size;

	UpdateHead();
	UpdateBody();

	// Move the head by an amount that will ensure that the snake can never collide with itself when performing a U-turn.
	m_position += m_size * 0.5f * m_direction;
}

// Update the head: round the floating position.
void Snake::UpdateHead() noexcept
{
	auto& head = m_head();
	head.x = std::roundf(m_position.x);
	head.y = std::roundf(m_position.y);
}

// Update the body: for every factor of 'size' that the head is ahead of the body, take the last body part and move it towards the head.
void Snake::UpdateBody() noexcept
{
	const SDL_Rect head{ Head() };
	const SDL_Rect secondBodyPart = m_bodyPartAt(1);

	// Calculate the number of body parts we need to update in order for our snake to stay in one piece.
	const size_t headDistance = static_cast<size_t>(std::abs(secondBodyPart.x - head.x)) + std::abs(secondBodyPart.y - head.y);
	size_t bodyPartIndex = std::min(headDistance / m_size, BodyPartsSize() - 1);

	for (; bodyPartIndex > 0; bodyPartIndex--)
	{
		// Take a reference of the head. We are calling it "nextBodyPartRef" because we'll be adding the head anew later on. Taking a reference of the head like this is free optimization.
		auto& nextBodyPartRef = m_head();

		// Move the next body part behind the head based on the snakes movement direction.
		nextBodyPartRef.x = head.x - m_direction.x * m_size * (bodyPartIndex - 1);
		nextBodyPartRef.y = head.y - m_direction.y * m_size * (bodyPartIndex - 1);

		// In the case where the head is not perfectly on the grid and our direction is a factor of minus one or less, solve for incorrect snapping.
		if (m_direction.x <= -1.0f && head.x % m_size != 0)
		{
			nextBodyPartRef.x += m_size;
		}
		if (m_direction.y <= -1.0f && head.y % m_size != 0)
		{
			nextBodyPartRef.y += m_size;
		}

		// Snap the body part to size (done implicitely by int division).
		nextBodyPartRef.x /= m_size;
		nextBodyPartRef.x *= m_size;

		nextBodyPartRef.y /= m_size;
		nextBodyPartRef.y *= m_size;

		// Pop the last body part and push the head to the front.
		m_bodyParts.pop_back();
		m_bodyParts.push_front(head);
	}
}

SDL_Rect& Snake::m_head() noexcept
{
	return m_bodyParts.front();
}

const SDL_Rect& Snake::Head() const noexcept
{
    return m_bodyParts.front();
}

SDL_Rect& Snake::m_tail() noexcept
{
	return m_bodyParts.back();
}

const SDL_Rect& Snake::Tail() const noexcept
{
    return m_bodyParts.back();
}

const SDL_Rect& Snake::BodyPartAt(size_t index) const
{
    return m_bodyParts.at(index);
}

SDL_Rect& Snake::m_bodyPartAt(size_t index)
{
	return m_bodyParts.at(index);
}

size_t Snake::BodyPartsSize() const noexcept
{
    return m_bodyParts.size();
}

Vector2 Snake::GetPosition() const noexcept
{
    return m_position;
}
