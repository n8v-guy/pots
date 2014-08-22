#include <iostream>
#include <deque>

typedef unsigned int VAL;
typedef std::deque<VAL> CDeque;

class CPotsHolder
{
    CDeque pots;
public:
    CPotsHolder(CDeque& src)
    {
        pots = src;
    }

    void Print()
    {
        if(!IsEmpty())
        {
            for(auto& v : pots)
                std::cout << v << ", ";
            std::cout << std::endl;
        }
    }

    const CDeque& GetPots() const
    {
        return pots;
    }

    VAL TakeL()
    {
        auto val = pots.front();
        pots.pop_front();
        return val;
    }

    VAL TakeR()
    {
        auto val = pots.back();
        pots.pop_back();
        return val;
    }

    bool IsEmpty()
    {
        return pots.empty();
    }
};

typedef VAL (*Strategy)(CPotsHolder& game);

VAL AlwaysL(CPotsHolder& potsHolder)
{
    return potsHolder.TakeL();
}

VAL AlwaysR(CPotsHolder& potsHolder)
{
    return potsHolder.TakeR();
}

VAL AlwaysHi(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    return pots.front() > pots.back() ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL AlwaysLo(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    return pots.front() < pots.back() ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL LessForEnemy(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    if(pots.size() <= 3)
        return AlwaysHi(potsHolder);
    VAL L = std::max(pots[1], pots[pots.size()-1]);
    VAL R = std::max(pots[0], pots[pots.size()-2]);
    return L < R ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL MoreForEnemy(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    if(pots.size() <= 3)
        return AlwaysHi(potsHolder);
    VAL L = std::max(pots[1], pots[pots.size()-1]);
    VAL R = std::max(pots[0], pots[pots.size()-2]);
    return L > R ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL MaximizePair(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    if(pots.size() <= 3)
        return AlwaysHi(potsHolder);
    // ours minus their potential points
    signed L = pots.front() - std::max(pots[1], pots[pots.size()-1]);
    signed R = pots.back() - std::max(pots[0], pots[pots.size()-2]);
    return L > R ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL MaximizeThree(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    if(pots.size() <= 3)
        return AlwaysHi(potsHolder);
    // ours minus their potential points
    signed L = pots.front();
    if(pots[1] > pots[pots.size()-1])
    {
        L = L - pots[1] + std::max(pots[2], pots[pots.size()-1]);
    }
    else
    {
        L = L - pots[pots.size()-1] + std::max(pots[1], pots[pots.size()-2]);
    }
    signed R = pots.back();
    if(pots[0] > pots[pots.size()-2])
    {
        R = R - pots[0] + std::max(pots[1], pots[pots.size()-2]);
    }
    else
    {
        R = R - pots[pots.size()-2] + std::max(pots[0], pots[pots.size()-3]);
    }
    return L > R ? potsHolder.TakeL() : potsHolder.TakeR();
}

VAL CheckMaximize(const CDeque& pots, size_t l, size_t r, bool plus)
{
    int mul = plus ? 1 : -1;
    if(l==r)
    {
        return mul*pots[l];
    }
    VAL mod = 0;
    if(pots[l] > pots[r])
    {
        mod = pots[l];
        ++l;
    }
    else
    {
        mod = pots[r];
        --r;
    }
    return mod*mul+CheckMaximize(pots, l, r, !plus);
}

VAL MaximizeAll(CPotsHolder& potsHolder)
{
    auto pots = potsHolder.GetPots();
    if(pots.size() <= 3)
        return AlwaysHi(potsHolder);
    // ours minus their potential points
    signed L = pots.front() + CheckMaximize(pots, 1, pots.size()-1, false);
    signed R = pots.back()  + CheckMaximize(pots, 0, pots.size()-2, false);
    return L > R ? potsHolder.TakeL() : potsHolder.TakeR();
}

struct strategyInfo
{
    Strategy    strategy;
    const char* name;
};

strategyInfo STRATEGIES[] =
{
        { &AlwaysL,      "always left" },
        { &AlwaysR,      "always right" },
        { &AlwaysLo,     "always lowest" },
        { &AlwaysHi,     "always highest" },
        { &LessForEnemy, "less for enemy" },
        { &MoreForEnemy, "more for enemy" },
        { &MaximizePair, "maximize pair" },
        { &MaximizeThree,"maximize three" },
        { &MaximizeAll,  "maximize all" },
};

class CPlayer
{
    VAL points;
    Strategy& strategy;
public:
    CPlayer(Strategy& st)
    : points(0)
    , strategy(st)
    {
    }

    VAL Move(CPotsHolder& game)
    {
        VAL pts = strategy(game);
        points += pts;
        return pts;
    }

    VAL GetPoints() const
    {
        return points;
    }
};

bool gen = true;
bool verbose = !gen;

CDeque GenPots()
{
    CDeque arrGen = {5, 7, 1, 3, 6, 1};
    if(gen) arrGen.clear();
    auto potsNum = 6;//4+rand()%10;
    potsNum -= potsNum%2;
    for(auto i = 0; i < potsNum; ++i)
    {
        if(gen) arrGen.push_back(1+rand()%2);
    }
    return std::move(arrGen);
}

int main(int argc, const char * argv[])
{
    const auto lastStrategyIdx = std::extent<decltype(STRATEGIES)>::value-1;
    std::cout << "Testing [" << STRATEGIES[lastStrategyIdx].name << "] strategy:" << std::endl;
    for(int i = 0; i < (gen?999:1); ++i)
    {
        CDeque arrGen = GenPots();
        CPotsHolder(arrGen).Print();
        for(auto strategyInfo : STRATEGIES)
        {
            CPotsHolder pots(arrGen);
            if(verbose) pots.Print();
            CPlayer A(STRATEGIES[lastStrategyIdx].strategy), B(strategyInfo.strategy);
            while(!pots.IsEmpty())
            {
                VAL ptsA = A.Move(pots);
                if(verbose) std::cout << "A+" << ptsA << std::endl;
                if(pots.IsEmpty()) break;
                VAL ptsB = B.Move(pots);
                if(verbose) std::cout << "B+" << ptsB << std::endl;
            }
            if(verbose) std::cout << "A:" << A.GetPoints() << " B:" << B.GetPoints() << std::endl;
            bool win = A.GetPoints() >= B.GetPoints();
            std::cout << (win ? "Wins" : "FAIL") << " vs [" << strategyInfo.name << "]" << std::endl;
            if(gen && !win) asm {int 3}
        }
    }
    return 0;
}
