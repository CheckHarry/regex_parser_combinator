#include <string>
#include <iostream>
using namespace std;

constexpr auto cont_inject_1()
{
    return [](string &s , unsigned pos) {return s.size() == pos;};
};

constexpr auto cont_inject_1_s()
{
    return [](const char *s) {return *s == '\0';};
};

template<const char *c , unsigned n>
struct BasicMatch
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        for (unsigned i = 0 ; i < n ; i ++)
        {
            if (pos < s.size() && (s[pos] == c[i] || c[i] == '.'))
                pos ++;
            else
                return false;
        }
        return con(s , pos);
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        for (unsigned i = 0 ; i < n ; i ++)
        {
            if (*s == c[i])
                s ++;
            else
                return false;
        }
        return con(s);
    }

};


template<typename Expr>
struct Repeat
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        return Expr::EVAL(s , pos , [con](string &s , unsigned pos){
            return Repeat<Expr>::EVAL(s , pos , con);
        }) || con(s , pos);
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        return Expr::EVAL_s(s , [con](const char *rest){
            return Repeat<Expr>::EVAL_s(rest , con);
        }) || con(s);
    }
};

template<typename Left , typename Right>
struct Alt
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        return Left::EVAL(s , pos , con) || Right::EVAL(s , pos , con);
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        return Left::EVAL_s(s , con) || Right::EVAL_s(s , con);
    }
};

struct Any
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        return pos < s.size() && con(s , pos + 1);
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        return *s != '\0' && con(s + 1);
    }
};


template<typename Start , typename ...Rest>
struct Concat
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        return Start::EVAL(s , pos , [con](string &s , unsigned pos){
            return Concat<Rest ...>::EVAL(s , pos , con);
        });
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        return Start::EVAL_s(s , [con](const char *s){
            return Concat<Rest ...>::EVAL_s(s , con);
        });
    }
};

template<typename Left , typename Right>
struct Concat<Left , Right>
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        return Left::EVAL(s , pos , [con](string &s , unsigned pos){
            return Right::EVAL(s , pos , con);
        });
    }

    template<typename Cont>
    static bool EVAL_s(const char *s , Cont con)
    {
        return Left::EVAL_s(s , [con](const char *rest){
            return Right::EVAL_s(rest , con);
        });
    }
};

template<typename Expr>
struct Capture
{
    template<typename Cont>
    static bool EVAL(string &s , unsigned pos , Cont con)
    {
        int start = pos;
        int end = -1;
        return Expr::EVAL(s , pos , [& , con](string &s , unsigned pos){
            end = pos;
            cout << "Capture : " << s.substr(start , end - start) << '\n';
            return con(s , pos);
        });
    }
};


int main()
{
    static const char teststr1[] = "ERROR";
    static const char teststr2[] = ".";
    static const char teststr3[] = "abc";
    using dotstar = Repeat<Any>;
    using regex = Concat<Capture<Repeat<BasicMatch<teststr3 , 3>>> , Any>;
    string t4 = string("abcabc");

    std::string s;
    std::getline(std::cin , s);
    std::cout << "s:" << s << '\n';

    
    auto k = regex::EVAL(s , 0 ,  cont_inject_1());
    std::cout << k << '\n';
        
}