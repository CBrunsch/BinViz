#ifndef BINIMG_H
#define BINIMG_H

#include <deque>
#include <string>
#include "CImg.h"

class BinImg: public cimg_library::CImg<unsigned char>
{
    private:
        // colors of pixels
        static const unsigned char ON[];
        static const unsigned char OFF[];
        static const unsigned char CLEAR[];

        static const bool REDRAW;
        static const char TITLE[];

        // remember cursor position and size
        int position;
        int resize;
        cimg_library::CImgDisplay disp;
        bool disp_info;
        bool in_packet;

        // used as a bit queue ... so we can detect starts or ends
        std::deque<bool> queue;

        // used to detect start and end sequences (wrap to new line)
        std::string start;
        std::string end;
        std::string drop;

        int detectPattern(const std::string &pattern, int start_pos = 0);
        int getMaxPixels();
        int getMaxPosition();
        void incPosition();
        int curPosition();
        int nextPosition();
        int wrapPosition();
        void update();
        void refresh();
        void remove(int nr_bits, int position = 0);
        void flush(int nr_bits);
        void flushPartial(int keep_bits);
        bool checkPattern(const std::string &pattern);
        void clear(int position);

    public:
        BinImg(int width, int height, const std::string &start_pattern = "", const std::string &end_pattern = "", const std::string &drop_pattern = "");

        void consume(const unsigned char in_byte);

        void on(int x, int y);
        void on(int position);
        void off(int x, int y);
        void off(int position);
        void set(bool state);
        void set(int x, int y, const unsigned char color[]);
        void set(int position, const unsigned char color[]);

        void setStart(const char detect_start[]);
        void setEnd(const char detect_end[]);
        int detectStart(int start_pos);
        int detectEnd(int start_pos);

        void wait();
        void flush();
};

#endif // BINIMG_H
