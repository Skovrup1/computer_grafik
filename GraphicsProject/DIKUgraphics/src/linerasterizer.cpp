#include "linerasterizer.h"

/*
 * \class LineRasterizer
 * A class which scanconverts a straight line. It computes the pixels such that
 * they are as close to the the ideal line as possible.
 */

/*
 * Parameterized constructor creates an instance of a line rasterizer
 * \param x1 - the x-coordinate of the first vertex
 * \param y1 - the y-coordinate of the first vertex
 * \param x2 - the x-coordinate of the second vertex
 * \param y2 - the y-coordinate of the second vertex
 */
LineRasterizer::LineRasterizer(int x1, int y1, int x2, int y2) {
    this->initialize_line(x1, y1, x2, y2);
}

/*
 * Destroys the current instance of the line rasterizer
 */
LineRasterizer::~LineRasterizer() {}

/*
 * Initializes the LineRasterizer with a new line
 * \param x1 - The x-coordinate of the first vertex
 * \param y1 - The y-coorsinate of the first vertex
 * \param x2 - The x-coordinate of the second vertex
 * \param y2 - The y-coordinate of the second vertex
 */
void LineRasterizer::Init(int x1, int y1, int x2, int y2) {
    this->initialize_line(x1, y1, x2, y2);
}

/*
 * Checks if there are fragments/pixels of the line ready for use
 * \return true if there are more fragments of the line, else false is returned
 */
bool LineRasterizer::MoreFragments() const { return this->valid; }

/*
 * Computes the next fragment of the line
 */
void LineRasterizer::NextFragment() {
    // Run the innerloop once
    // Dereference the pointer to the private member function
    // It looks strange; but this is the way it is done!
    (this->*innerloop)();
}

/*
 * Returns a vector which contains all the pixels of the line
 */
std::vector<glm::vec3> LineRasterizer::AllFragments() {
    std::vector<glm::vec3> points;
    while (MoreFragments()) {
        glm::vec3 point(this->x(), this->y(), 0.0f);
        points.push_back(point);

        NextFragment();
    }

    return points;
}

/*
 * Returns the coordinates of the current fragment/pixel of the line.
 * It is only valid to call this function if "MoreFragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The coordinates of the current line fragment/pixel
 */
glm::vec3 LineRasterizer::Fragment() const {
    return glm::vec3(float(this->x()), float(this->y()), 0.0f);
}

/*
 * Returns the current x-coordinate of the current fragment/pixel of the line
 * It is only valid to call this function if "more_fragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The x-coordinate of the current line fragment/pixel
 */
int LineRasterizer::x() const {
    if (!this->valid) {
        throw std::runtime_error("LineRasterizer::x(): Invalid State");
    }
    return this->x_current;
}

/*
 * Returns the current y-coordinate of the current fragment/pixel of the line
 * It is only valid to call this function if "more_fragments()" returns true,
 * else a "runtime_error" exception is thrown
 * \return The y-coordinate of the current line fragment/pixel
 */
int LineRasterizer::y() const {
    if (!this->valid) {
        throw std::runtime_error("LineRasterizer::y(): Invalid State");
    }
    return this->y_current;
}

/*
 * Protected functions
 */

/*
 * Private functions
 */

/*
 * Initializes the LineRasterizer with the two vertices
 */
void LineRasterizer::initialize_line(int x1, int y1, int x2, int y2) {
    // Store start and end points
    this->x_start = x1;
    this->y_start = y1;
    this->x_stop  = x2;
    this->y_stop  = y2;

    x_current = x_start;
    y_current = y_start;

    dx = x_stop - x_start;
    dy = y_stop - y_start;

    x_step = (dx >= 0) ? 1 : -1;
    y_step = (dy >= 0) ? 1 : -1;

    dx = abs(dx);
    dy = abs(dy);

    abs_2dx = 2 * dx;
    abs_2dy = 2 * dy;

    valid = true;

    if (dx >= dy) {
        d = abs_2dy - dx;
        innerloop = &LineRasterizer::x_dominant_innerloop;
    } else {
        d = abs_2dx - dy; 
        innerloop = &LineRasterizer::y_dominant_innerloop;
    }
}

/*
 * Runs the x-dominant innerloop
 */
void LineRasterizer::x_dominant_innerloop() {
    if (x_current == x_stop) {
        valid = false;
        return;
    }

    if (d > 0) {
        y_current += y_step;
        d -= abs_2dx;
    }

    x_current += x_step;
    d += abs_2dy;
}

/*
 * Runs the y-dominant innerloop
 */
void LineRasterizer::y_dominant_innerloop() {
    if (y_current == y_stop) {
        valid = false;
        return;
    }

    if (d > 0) {
        x_current += x_step;
        d -= abs_2dy;
    }

    y_current += y_step;
    d += abs_2dx;
}
