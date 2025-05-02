#pragma once

#include <SFML/Graphics.hpp>
#include <sstream>
#include <complex>
#include <iostream>
#include <thread>

using namespace sf;
using namespace std;

const unsigned int MAX_ITER = 255;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

enum class State
{
	CALCULATING,
	DISPLAYING
};

class ComplexPlane : public sf::Drawable
{
	public:
		ComplexPlane(int pixelWidth, int pixelHeight);
		void draw(RenderTarget& target, RenderStates states) const override;
		void zoomIn();
		void zoomOut();
		void setCenter(Vector2f mousePixel);
		void setMouseLocation(Vector2i mousPixel);
		void loadText(Text& text);
		void updateRender();
		void hsvToRgb(float h, float s, float v, Uint8& r, Uint8& g, Uint8& b);

	private:
		VertexArray m_vArray;
		State m_state;
		Vector2f m_mouseLocation;
		Vector2f m_pixel_size;
		Vector2f m_plane_center;
		Vector2f m_plane_size;
		int m_zoomCount;
		float m_aspectRatio;
		int countIterations(Vector2f coord);
		void iterationsToRGB(size_t count, Uint8& g, Uint8& b, Uint8& r);
		Vector2f mapPixelToCoords(Vector2f mousePixel);
};
