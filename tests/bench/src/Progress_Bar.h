#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>

using cc = std::chrono::steady_clock;

class progressBar
{
public:
    progressBar(int _total, int _updateSecound = 1) : total(_total), updateSecond(_updateSecound)
    {
        lst = cc::now();
        start = cc::now();
    }

    void update(int curr)
    {
        auto now = cc::now();
        int secPassed = std::chrono::duration_cast<std::chrono::seconds>(now - lst).count();

        int percent = curr * 100 / total;
        if (secPassed >= updateSecond || curr == total)
        {
            double speed = (curr - lstShow) / updateSecond;
            int esti = (total - curr == 0 ? 0 : (total - curr) / speed);
            int elap = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

            std::cout << '\r' << std::string(100, ' ') << '\r';

            std::cout << std::setw(3) << percent << "% | " << curr << '/' << total
                      << " | elapsed time: " << elap << " | estimate time last: " << esti << 's';

            lst = now;
            lstShow = curr;
        }
    }

    void finish()
    {
        update(total);
        std::cout << '\n';
    }

private:
    int total, updateSecond;
    int lstShow = 0;
    cc::time_point start;
    cc::time_point lst;
};