#include "ComplexPlane.h"


//Public functions
ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
    m_pixel_size = Vector2f(pixelWidth, pixelHeight);
    m_aspectRatio = static_cast<float>(pixelWidth)/pixelHeight;
    m_plane_center = Vector2f(0, 0);
    m_plane_size = Vector2f(BASE_WIDTH, BASE_HEIGHT * m_aspectRatio);
    m_zoomCount = 0;
    m_state = State::CALCULATING;
    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(pixelWidth * pixelHeight);

    //my addition
    for (int j = 0; j < static_cast<int>(m_pixel_size.y); j++)
    {
        for (int i = 0; i < static_cast<int>(m_pixel_size.x); i++)
        {
            int index = j * m_pixel_size.x + i;
            m_vArray[index].position = Vector2f(i, j);
        }
    }
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray, states);
}

void ComplexPlane::zoomIn()
{
    m_zoomCount++;
    float zoomFactor = pow(BASE_ZOOM, m_zoomCount);
    float tempX = BASE_WIDTH * zoomFactor;
    float tempY = BASE_HEIGHT * m_aspectRatio * zoomFactor;
    m_plane_size = Vector2f(tempX, tempY);
    Vector2f mouseWorldPos = m_mouseLocation;
    m_plane_center = mouseWorldPos - (mouseWorldPos - m_plane_center) * zoomFactor;
    m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
    m_zoomCount--;
    float zoomFactor = pow(BASE_ZOOM, m_zoomCount);
    float tempX = BASE_WIDTH * zoomFactor;
    float tempY = BASE_HEIGHT * m_aspectRatio * zoomFactor;
    m_plane_size = Vector2f(tempX, tempY);
    Vector2f mouseWorldPos = m_mouseLocation;
    m_plane_center = mouseWorldPos - (mouseWorldPos - m_plane_center) * zoomFactor;
    m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2f mousePixel)
{

    m_plane_center = mapPixelToCoords(mousePixel);
    m_state = State::CALCULATING;

}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(static_cast<Vector2f>(mousePixel));
}

void ComplexPlane::loadText(Text& text)
{

    std::stringstream output;
    output << "Mandlebrot Set" << endl
        << "Center: " << '(' << m_plane_center.x << ',' << m_plane_center.y << ')' << endl
        << "Cursor: " << '(' << m_mouseLocation.x << ',' << m_mouseLocation.y << ')' << endl
        << "Left-click to Zoom in" << endl 
        << "Right-click to Zoom out" << endl;
    text.setString(output.str());
}

void ComplexPlane::updateRender()
{
    if (m_state == State::CALCULATING)
    {
        unsigned int numThreads = thread::hardware_concurrency();
        if (numThreads == 0)
        {
            numThreads = 1;
        }
        vector<thread> threads;
        int height = static_cast<int>(m_pixel_size.y);
        int width = static_cast<int>(m_pixel_size.x);
        int rowsPerThread = height/numThreads;
        for (size_t t = 0; t < numThreads; t++)
        {
            int startRow = t*rowsPerThread;
            int endRow = (t == numThreads - 1) ? height : startRow + rowsPerThread;
            threads.emplace_back([=]()
            {
                for (int j = startRow; j < endRow; j++)
                {
                    for (int i = 0; i < width; i++)
                    {
                        int iterations = countIterations(mapPixelToCoords(Vector2f(i, j)));
                        Uint8 r, g, b;
                        iterationsToRGB(iterations, g, b, r);
                        int index = j * m_pixel_size.x + i;
                        m_vArray[index].color = Color(g,b,r);
                    }
                }
            });
        }
        for (auto& thread : threads)
        {
            thread.join();
        }
        m_state = State::DISPLAYING;
    }   
}

//Private functions
int ComplexPlane::countIterations(Vector2f coord)
{
    int count = 0;
    complex<float> z(0, 0);
    complex<float> c(coord.x, coord.y);
    while (abs(z) <= 2.0 && static_cast<unsigned int>(count) < MAX_ITER)
    {
        z = z * z + c;
        count++;
    }
    return count;
}

void ComplexPlane::hsvToRgb(float h, float s, float v, Uint8& r, Uint8& g, Uint8& b)
{
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float r1, g1, b1;

    if (h < 60) { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120) { r1 = x; g1 = c; b1 = 0; }
    else if (h < 180) { r1 = 0; g1 = c; b1 = x; }
    else if (h < 240) { r1 = 0; g1 = x; b1 = c; }
    else if (h < 300) { r1 = x; g1 = 0; b1 = c; }
    else { r1 = c; g1 = 0; b1 = x; }

    r = static_cast<Uint8>((r1 + m) * 255);
    g = static_cast<Uint8>((g1 + m) * 255);
    b = static_cast<Uint8>((b1 + m) * 255);
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& g, Uint8& b, Uint8& r)
{
    if (count >= MAX_ITER)
    {
        r = g = b = 0;
        return;
    }
    float t = static_cast<float>(count) / MAX_ITER;
    t = powf(t, 0.6f);
    float hue = fmod(360.0f * t * 3.0f, 360.0f);
    float saturation = 0.85f;
    float value = 1.0f;

    hsvToRgb(hue, saturation, value, r, g, b);
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2f mousePixel)
{
    float tempX = mousePixel.x / m_pixel_size.x;
    float tempY = mousePixel.y / m_pixel_size.y;


    Vector2f worldPos((m_plane_center.x - (m_plane_size.x / 2) + tempX * m_plane_size.x), 
                      (m_plane_center.y - (m_plane_size.y / 2) + tempY * m_plane_size.y));

    
    return worldPos;
}
