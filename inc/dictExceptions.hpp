#pragma once
#include <stdexcept>
#include <exception>

namespace DICX
{
    class dictexception: public std::exception
    { 
    public:
        enum dictxtypes {
            ADV = 0,
            CHAR = 1,
            LST = 2,
            WPL = 3,
            INT = 4,
            OFF1 = 5,
            DUP = 6,
            INS = 7,
            PVT = 8,
            OFF2 = 9
        };
        const char * d_Exception[10] = {
            "Error - invalid advance past the end of the string",
            "Error - invalid character get past the end of the string",
            "Error - invalid last character",
            "Error - too many words per line",
            "Error - wrong size for an integer",
            "Error - offset not found",
            "Error - too many duplicate indices",
            "Error - failed to insert string",
            "Error - invalid pivot",
            "Error - offset AVL error"
        };
    };

    class advance: public dictexception
    {
    public:
        virtual const char* what() const throw() {return d_Exception[ADV];};
    };

    class get_char: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[CHAR];};
    };

    class last_char: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[LST];};
    };

    class words_per_line: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[WPL];};
    };

    class integer: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[INT];};
    };

    class offset_not_found: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[OFF1];};
    };

    class duplicate_indices: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[DUP];};
    };

    class insert: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[INS];};
    };

    class pivot: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[PVT];};
    };

    class offsetAVL: public dictexception
    {
    public:
        virtual const char * what() const throw() {return d_Exception[OFF2];};
    };


};