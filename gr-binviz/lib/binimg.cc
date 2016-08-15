#include "binimg.h"
#include <vector>
#include <string>
#include <iostream>

// Define ON and OFF as black and white. Syntax follows CImg colors
const unsigned char BinImg::ON[] = {255};
const unsigned char BinImg::OFF[] = {0};
const unsigned char BinImg::CLEAR[] = {128};
const bool BinImg::REDRAW = true;
const char BinImg::TITLE[] = "BinViz, click or wheel";

/**
 * @brief BinImg::BinImg create a simple image (black/white)
 * @param width the width of the image
 * @param height the height of the image
 * @param start_pattern marks start of a packet and puts new line before the pattern (e.g. '101010')
 * @param end_pattern marks end of packet and wraps to new line
 * @param drop_pattern will kill all occurences of the pattern recoursively but will not apply if both, start and stop patterns are defined. The drop pattern have precedence over start and stop patterns.
 */
BinImg::BinImg(int width, int height, const std::string &start_pattern, const std::string &end_pattern, const std::string &drop_pattern)
    :CImg<unsigned char>(width, height, 1, 1), start(start_pattern), end(end_pattern), drop(drop_pattern)
{
    // queue is empty
    queue = std::deque<bool>();

    // check start and end detection, failover to off on error
    in_packet = false;

    if (!checkPattern(start)) {
        start = "";
    }

    if (!checkPattern(end)) {
        end = "";
    }

    if (!checkPattern(drop)) {
        drop = "";
    }

    // set first pixel and default zoom (resize) 4x
    position = 0;
    resize = 4;

    // do not show disp_info as this state is blocking
    disp_info = false;

    // create inital display, set title and zoom
    disp = cimg_library::CImgDisplay(width, height, TITLE);
    disp.resize(width*resize, height*resize, REDRAW);

    /**
     * clear image and set two dummy pixles (CImg does calculate a mean
     * for some reason which causes the image to appear black even though
     * it was filled with 128/half of the char unless a 0 and 255 pixel
     * are set.
     */
    fill(128);
    on(width-1,height-1);
    off(width-2,height-1);
    update();
}

/**
 * @brief BinImg::consume one bit and process/display according to
 * start/stop/drop patterns. Note, drop patterns will have precedence
 * over start or stop patterns. Further note, the drop pattern will not be
 * applied when both, the start and stop pattern are defined.
 * @param in_byte to be consumed
 */
void
BinImg::consume(const unsigned char in_byte)
{
    // move byte to queue
    for (int i=7; i>=0; i--)
    {
        // check if current bit is true
        if (in_byte & (1 << i))
        {
                queue.push_back(true);
        }
        else
        {
                queue.push_back(false);
        }
    }

    // neither start nor end patterns have been defined
    if (start.length() == 0 && end.length() == 0) {

        // clean stream of drop sequences recursively
        if (drop.length() > 0) {

            int pos = 0; // assure the loop runs once

            while ((pos = detectPattern(drop)) > -1) {
                remove(drop.length(),pos);
            }
        }

        // display all bits immediately
        while(queue.size() > 0) {

            // set bit at position
            set(queue.front());

            // remove bit from queue
            queue.pop_front();
        }
    }
    // only end pattern is defined
    else if (start.length() == 0 && end.length() > 0) {

        // clean stream of drop sequences recursively
        if (drop.length() > 0) {

            int pos = 0; // assure the loop runs once

            while ((pos = detectPattern(drop)) > -1) {
                remove(drop.length(),pos);
            }
        }

        /**
         * improve speed by not searching the whole queue as we
         * searched the queue allready with the last 8 bit consumed
         */
        int skip = queue.size() - 8 - (end.length() - 1);

        // detect end
        int pos_end = detectEnd(skip);

        // check if end detected
        if (pos_end > -1) {

            // display bits until end pattern
            flush(pos_end);

            // wrap to next line of image
            wrapPosition();
        }

        // keep start.length()-1 bits as these might match next time
        flushPartial(end.length());
    }
    // only start pattern is defined
    else if (start.length() > 0 && end.length() == 0) {

        // clean stream of drop sequences recursively
        if (drop.length() > 0) {

            int pos = 0; // assure the loop runs once

            while ((pos = detectPattern(drop)) > -1) {
                remove(drop.length(),pos);
            }
        }

        /**
         * improve speed by not searching the whole queue as we
         * searched the queue allready with the last 8 bit consumed
         */
        int skip = queue.size() - 8 - (start.length() - 1);

        // detect start
        int pos_start = detectStart(skip);

        // check if detected
        if (pos_start > -1) {

            // display bits in front of start pattern
            flush(pos_start);

            // wrap to next line
            wrapPosition();

            // display start pattern
            flush(start.length());
        }

        // keep start.length()-1 bits as these might match next time
        flushPartial(start.length());
    }
    // start and end pattern are defined
    else {

        int pos = -1;

        // detect start or end depending on state
        if (in_packet) {
            /**
             * improve speed by not searching the whole queue as we
             * searched the queue allready with the last 8 bit consumed
             */
            int skip = queue.size() - 8 - (end.length() - 1);

            pos = detectEnd(skip);
        }
        else {
            /**
             * improve speed by not searching the whole queue as we
             * searched the queue allready with the last 8 bit consumed
             */
            int skip = queue.size() - 8 - (start.length() - 1);

            pos = detectStart(skip);
        }

        // check detected ?
        if (pos > -1) {

            // flush bits from start to end or remove bits after end until start
            if (in_packet) {
                flush(pos);
            }
            else {
                remove(pos);
            }

            // display start pattern or wrap after end pattern
            if (!in_packet) {
                flush(start.length());
            }
            else {
                // wrap line
                wrapPosition();
            }

            // switch state as we found pattern
            in_packet = !in_packet;
        }
    }
}

/**
 * @brief BinImg::set pixel to color
 * @param x the x-coordinate of the pixel to be set
 * @param y the y-coordinate of the pixel to be set
 * @param color the gray shade. eg. {128}
 */
void
BinImg::set(int x, int y, const unsigned char color[])
{
    draw_point(x, y, color);
    update();
}

/**
 * @brief BinImg::set position to color
 * @param position to be set
 * @param color the gray shade. eg. {128}
 */
void
BinImg::set(int position, const unsigned char color[])
{
    int x = position % width();
    int y = position / width();

    draw_point(x, y, color);
    update();
}

/**
 * @brief BinImg::on set pixel at arbitrary location to on
 * @param x x-coordinate of the pixel
 * @param y y-coordinate of the pixel
 */
void
BinImg::on(int x, int y)
{
    set(x, y, ON);
}

/**
 * @brief BinImg::on switch pixel at a specific position to on
 * @param position of the pixel (lu-corner position is 0)
 */
void
BinImg::on(int position)
{
    set(position, ON);
}

/**
 * @brief BinImg::off set pixel at arbitrary location to off
 * @param x x-coordinate of the pixel
 * @param y y-coordinate of the pixel
 */
void
BinImg::off(int x, int y)
{
    set(x, y, OFF);
}

/**
 * @brief BinImg::off switch pixel at a specific position to off
 * @param position of the pixel (lu-corner position is 0)
 */
void
BinImg::off(int position)
{
    set(position, OFF);
}

/**
 * @brief BinImg::clear gray out pixel at specific position
 * @param position of the pixel (lu-corner position is 0)
 */
void
BinImg::clear(int position) {
    set(position, CLEAR);
}

/**
 * @brief BinImg::set current bit on or off and increment position
 * @param on true to show bit as on, false to show bit as off
 */
void
BinImg::set(bool state)
{
    if(state) {
        on(curPosition());
    }
    else {
        off(curPosition());
    }

    // next position from queue
    incPosition();
}

/**
 * @brief BinImg::getMaxPixels
 * @return the total number of pixels (width * height)
 */
int
BinImg::getMaxPixels()
{
    return width() * height();
}

/**
 * @brief BinImg::getMaxPosition
 * @return the maximum position in the image (actually, nr pixels -1)
 */
int
BinImg::getMaxPosition()
{
    return getMaxPixels() -1;
}

/**
 * @brief BinImg::incPosition use this function to increment the position
 * as it does take care of the wrapping when you hit the last pixel
 */
void
BinImg::incPosition()
{
    position++;

    // wrap around; point to the top of the image if we reached the end
    if (position >= getMaxPosition())
    {
        position = 0;
    }
}

/**
 * @brief BinImg::curPosition
 * @return the current position
 */
int
BinImg::curPosition()
{
    return position;
}

/**
 * @brief BinImg::wrapPosition to next line
 * @return the position of the first pixel of the next row - CR LF :)
 */
int
BinImg::wrapPosition()
{
    int new_pos = position - (position % width()) + width();

    for (int i = position; i < new_pos; i++) {
        clear(i);
    }

    position = new_pos;
}

/**
 * @brief BinImg::nextPosition provides location of next position relative
 * to current position and takes image wrapping into account
 * @return the next position (0 if current position is at end of image)
 */
int
BinImg::nextPosition()
{
    if (position >= getMaxPosition())
    {
        return 0;
    }
    else
    {
        return position + 1;
    }
}

/**
 * @brief BinImg::update display. sets title, takes care of resizing (mouse
 * wheel) as well as mouse clicks and key events
 */
void
BinImg::update() {

    // mouse wheel
    if (disp.wheel()) {

        // wheel counter should be something else but 0
        resize += disp.wheel();
        disp.resize(width()*resize, height()*resize, REDRAW);

        // clear wheel counter
        disp.set_wheel();
    }

    // left click
    if (disp.button() & 1) {

        // switch on display_info
        disp_info = true;

        // clear clicks
        disp.set_button();
    }

    refresh();
}

/**
 * @brief BinImg::refresh handles selection state when user selects and
 * zooms part of the image.
 */
void
BinImg::refresh() {
    if (disp_info) {
        disp.set_title("Paused, ESC to continue");
        display(disp, disp_info);

        // immediately return to non-blocking mode, when
        // user escapes (ESC) disp_info mode
        disp_info = false;
    }
    else {
        disp.set_title(TITLE);
        display(disp);
    }
}

/**
 * @brief BinImg::wait until a keyboard or mouse event occurs
 */
void
BinImg::wait() {
    disp.wait();
}


/**
 * @brief BinImg::detectPattern
 * @param pattern to search for
 * @param start_pos improve speed by start searching from start_pos
 * @return -1 if not matched or vposition that matched (0 for first pos.)
 */
int
BinImg::detectPattern(const std::string &pattern, int start_pos) {

    // for each bit in the queue check pattern
    int match = -1;
    int queue_pos = -1;

    // assure start_pos fits valid range otherwise search whole queue
    if (start_pos < 0 || start_pos >= queue.size()) {
        start_pos=0;
    }

    /**
     * Iterate trough queue and try to match the pattern. Iteration will
     * run until queue.size() - len_pattern bits are left since the pattern
     * can't match less queued bits than the pattern itself. For efficiency
     * reasons, the start position could be shifted by start_pos. This will
     * allow to skip bits that have already searched before.
     */
    for(queue_pos=start_pos; queue_pos < queue.size() - (pattern.length() - 1); queue_pos++) {

        int len_queue = queue.size() - queue_pos;

        // pattern can't mach if queue is shorter than pattern
        if (len_queue < pattern.length()) {
            return -1;
        }


        for(int pattern_pos=0; pattern_pos < pattern.length(); pattern_pos++) {

            // in each queue element, check if pattern matches
            if (queue.at(pattern_pos+queue_pos) == true
                    && pattern[pattern_pos] == '1'
                    || queue.at(pattern_pos+queue_pos) == false
                    && pattern[pattern_pos] == '0') {

                /**
                 * bit matches... check whether we successfully iterated
                 * through the whole pattern
                 */
                if (pattern_pos == pattern.length() -1) {
                    return queue_pos;
                }
            }
            else {
                // bit did not match... continue to shift queue
                break;
            }
        }
    }

    return -1;
}

/**
 * @brief BinImg::detectStart detects start pattern
 * @return position of left most pattern bit if detected otherwise -1
 */
int
BinImg::detectStart(int start_pos=0) {
    return detectPattern(start, start_pos);
}

/**
 * @brief BinImg::detectEnd detects end pattern
 * @return position of right most pattern bit if detected otherwise -1
 */
int
BinImg::detectEnd(int start_pos=0) {
    int len_pattern = end.length();
    int pos_pattern = detectPattern(end, start_pos);

    if (pos_pattern > -1) {
        return pos_pattern + len_pattern;
    }

    return -1;
}

/**
 * @brief BinImg::flush displays all queued bits immediately and will
 * result in an empty queue.
 */
void
BinImg::flush() {

    while(queue.size()) {
        // set bit at position
        set(queue.front());

        // remove bit from queue
        queue.pop_front();
    }
}

/**
 * @brief BinImg::remove remove a number of bits from the front of the queue
 * @param nr_bits the number of bits popped from the front of the queue
 * @param position skip to this position and remove nr_bits starting at position. 0 if param is omitted.
 */
void
BinImg::remove(int nr_bits, int position) {

    // check bounds position and nr_bits
    if (position < queue.size() && position >= 0
        && nr_bits >= 1 && nr_bits + position <= queue.size()) {

        /**
         * create start and end iterators of range to be removed. queue.begin()
         * is equivalent to 1 therefore it_start -1 because we use 0 for the
         * first pos all over the code.
         */
        std::deque<bool>::iterator it_start = queue.begin() + position;
        std::deque<bool>::iterator it_end = it_start + nr_bits;

        // erase part of queue (automatically resizes queue)
        queue.erase(it_start, it_end);
    }
}
/**
 * @brief BinImg::flush displays a specified number of bits from the queue
 * @param nr_bits the number of bits taken from the front of the queue and being displayed
 */
void
BinImg::flush(int nr_bits) {
    // display bits until nr_bits
    for (int i=0; i < nr_bits; i++) {
        set(queue.front());

        // remove bit from queue
        queue.pop_front();
    }
}

/**
 * @brief BinImg::flush_partial flushes the queue but keeps keep_bits bits in the queue
 * @param keep_bits the number of tailing bits to be kept
 */
void
BinImg::flushPartial(int keep_bits) {

    // keep end.length()-1 bits as these might match next time
    while(queue.size() >= keep_bits) {
        // set bit at position
        set(queue.front());

        // remove bit from queue
        queue.pop_front();
    }
}

/**
 * @brief BinImg::check_pattern if it only consists of '1' and '0'
 * @param pattern to be checked
 * @return true if pattern is composed of '1' and '0' only
 */
bool
BinImg::checkPattern(const std::string &pattern) {

    // iterate through all characters of the pattern
    for (int i=0; i < pattern.length(); i++) {

        // break loop if the character does not comply 0 or 1
        if (pattern[i] != '0' && pattern[i] != '1') {
            return false;
        }
    }

    return true;
}
