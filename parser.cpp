#include <string>
#include <vector>
#include <type_traits>
#include <iostream>
#include <unordered_set>
using namespace std;

constexpr auto make_single_character_parser(char c)
{
    return [=](string &s , int pos){
        if (pos < s.size() && (s[pos] == c || c == '.'))
            return unordered_set<int> {pos + 1};
        else
            return unordered_set<int> {};
    };
};

constexpr auto make_string_parser(const char *c , unsigned n)
{
    return [=](string &s , int pos){
        int cur_pos = pos;

        for (int i = 0 ; i < n ; i ++)
        {
            if (cur_pos < s.size() && (s[cur_pos] == c[i] || c[i] == '.'))
                cur_pos ++;
            else
                return unordered_set<int> {};
        }

        return unordered_set<int> {cur_pos};
    };
};

int constexpr length(const char* str)
{
    return *str ? 1 + length(str + 1) : 0;
}

template<const char *res>
constexpr auto make_string_parser_templ()
{
    return [=](string &s , int pos){
        int cur_pos = pos;

        for (int i = 0 ; i < length(res) ; i ++)
        {
            if (cur_pos < s.size() && (s[cur_pos] == res[i] || res[i] == '.'))
                cur_pos ++;
            else
                return unordered_set<int> {};
        }

        return unordered_set<int> {cur_pos};
    };
}


constexpr auto concat(auto a , auto b)
{
    return [=](string &s , int pos)
    {
        unordered_set<int> to_return;
        auto a_result = a(s , pos);

        for (int i : a_result)
        {
            auto k = b(s , i);
            for (int j : k)
                to_return.insert(j);
        }

        return to_return;
    };
};


constexpr auto star(auto a)
{
    return [=](string &s , int pos)
    {
        unordered_set<int> to_return = {pos};
        unordered_set<int> cur_pos = {pos};

        while (true)
        {
            unordered_set<int> next_pos;
            for (int i : cur_pos)
            {
                for (int j : a(s , i))
                {
                    to_return.insert(j);
                    next_pos.insert(j);
                }
            }

            if (next_pos.size() == 0)
                return to_return;

            cur_pos = move(next_pos);
        }

    };
};

constexpr auto make_or(auto a , auto b)
{
    return [=](string &s , int pos)
    {
        auto a_result = a(s , pos);

        for (int j : b(s , pos))
                a_result.insert(j);

        return a_result;
    };
}

bool check(auto parser , string &s)
{
    auto v = parser(s , 0);

    for (int i : v)
    {   
        if (i == s.size())
            return true;
    }

    return false;
}



int main()
{
    static const char teststr1[] = "ERROR";
    static const char teststr2[] = ".";
    static const char teststr3[] = "abc";
    constexpr auto kk1 = concat(
        star(make_string_parser_templ<teststr2>()) ,
        make_string_parser_templ<teststr1>()
    );

    constexpr auto kk2 = concat(
        kk1 ,
        star(make_string_parser_templ<teststr2>()) 
    );
 
    constexpr auto kk3 = make_or(
        kk2 ,
        star(make_string_parser_templ<teststr3>())
    );

    string t4 = string("abcabcERROR");
    
    

    cout << check(kk2 , t4) << '\n';
}