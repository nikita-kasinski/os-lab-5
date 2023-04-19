#pragma once

class Protocol
{
public:
    static constexpr char FAILURE = 0;
    static constexpr char SUCCESS = 1;
    static constexpr char FINISH = 2;
    static constexpr char QUIT = 3;
    static constexpr char READ = 4;
    static constexpr char MODIFY = 5;
    static constexpr char SIZE = 1;
};