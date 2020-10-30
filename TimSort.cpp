#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

//https://neerc.ifmo.ru/wiki/index.php?title=Timsort
//Сортировать нужно в неубывающем порядке (1, 3, 2, 6, 2, 4) => (1, 2, 2, 3, 4, 6)

///Тут реализована сортировка вставками 
void insertionsort(vector<int>::iterator begin, vector<int>::iterator end) {
    for (vector<int>::iterator i = begin + 1; i < end; ++i)
    {
        int value = *i;
        vector<int>::iterator j = i - 1;
        while ((j > begin) && (*j > value))
        {
            *(j + 1) = *j;
            --j;
        }
        if ((j == begin) && (*j > value))
        {
            *(j + 1) = *j;
            *j = value;
        }
        else
            *(j + 1) = value;
    }
}
//Тут функция для определения minrun - копипаста из конспекта ИТМО c:
int minRunLength(int n) {
    bool flag = 0;// будет равно 1, если среди сдвинутых битов есть хотя бы один ненулевой
    while (n >= 64)
    {
        flag |= n & 1;
        n >>= 1;
    }
    return n + flag;
}

//Алгоритм, считающий длину очередного run подмассива. Наверное, много лишнего, черт бы побрал эти итераторы¯＼_(ツ)_/¯
int runLength(vector<int>::iterator begin, vector<int>::iterator end, int minrun)//ищем и сортируем все подмассивы run сортируемого массива
{
    vector<int>::iterator i = begin;
    int runSize = 0;//индекс начала подмассива и его размер
    if (i != end)
    {
        //типа по дефолту всегда добавляем сразу 2 элемента, но из-за итераторов делаю 100 проверок на end
        ++i;//здесь 
        ++runSize;
        if (i != end)
        {
            ++i;//и здесь
            ++runSize;
            if (i != end)
            {
                if (*(i - 2) <= *(i - 1))//если 2 предыдущих выполняют условию, то добавляем ещё пока также выполняется условие
                {
                    while (((i != end) && (*i >= *(i - 1))) || ((i != end) && (runSize < minrun)))
                    {
                        ++i;
                        ++runSize;
                    }
                }
                else//if (*(i - 2) > *(i - 1))
                {
                    while (((i != end) && (*i < *(i - 1))) || ((i != end) && (runSize < minrun)))
                    {
                        ++i;
                        ++runSize;
                    }
                }
            }
        }
    }
    return runSize;
}

//Дальше всё необходимое для слияния
////сортировка слиянием:
void Merge(vector<int>::iterator lbegin, vector<int>::iterator lend, vector<int>::iterator rbegin, vector<int>::iterator rend)
{
    vector<int> tmp;
    //Копируем левый массив во временный
    for (vector<int>::iterator i = lbegin; i != lend; ++i)
        tmp.push_back(*i);

    vector<int>::iterator i = lbegin;
    vector<int>::iterator j = rbegin;
    vector<int>::iterator k = tmp.begin();
    while ((i != rend) && (j != rend) && (k != tmp.end()))
    {
        if (*k <= *j)
        {
            *i = *k;
            ++i;
            ++k;
        }
        else
        {
            *i = *j;
            ++i;
            ++j;
        }

    }
    while (j != rend)
    {
        *i = *j;
        ++i;
        ++j;
    }
    while (k != tmp.end())
    {
        *i = *k;
        ++i;
        ++k;
    }
}

void timsortMerge(vector<pair<int, int>>& stek, vector<int>::iterator begin, vector<int>::iterator end)
{
    while (stek.size() > 1)
    {
        int X = stek[stek.size() - 1].second;
        int Y = stek.size() < 2 ? 0 : stek[stek.size() - 2].second;
        int Z = stek.size() < 3 ? 0 : stek[stek.size() - 3].second;

        if ((stek.size() >= 2) && (Y <= X))
            //Merge(Y,X);
        {
            Merge(begin + stek[stek.size() - 2].first, begin + stek[stek.size() - 2].first + stek[stek.size() - 2].second, begin + stek[stek.size() - 1].first, begin + stek[stek.size() - 1].first + stek[stek.size() - 1].second);
            stek[stek.size() - 2].second += stek[stek.size() - 1].second;
        }
        if ((stek.size() >= 3) && (Z <= X + Y))
        {
            if (X <= Z)
                //Merge(Y,X)
            {
                Merge(begin + stek[stek.size() - 2].first, begin + stek[stek.size() - 2].first + stek[stek.size() - 2].second, begin + stek[stek.size() - 1].first, begin + stek[stek.size() - 1].first + stek[stek.size() - 1].second);
                stek[stek.size() - 2].second += stek[stek.size() - 1].second;
            }
            else
                //Merge(Z, Y)
            {
                Merge(begin + stek[stek.size() - 3].first, begin + stek[stek.size() - 3].first + stek[stek.size() - 3].second, begin + stek[stek.size() - 2].first, begin + stek[stek.size() - 2].first + stek[stek.size() - 2].second);
                stek[stek.size() - 3].second += stek[stek.size() - 2].second;
            }
        }
        stek.pop_back();
    }
}

#define ITER

#ifdef ITER

//сортировать нужно интервал [begin, end)
void timsort(vector<int>::iterator begin, vector<int>::iterator end) {
    pair<int, int> buffer;
    vector<pair<int, int>> stek;

    //Шаг 1. Вычисление minrun
    int minrun = minRunLength(distance(begin, end));
    int run;
    int index = 0;//индекс итерации
    //Шаг 2. Алгоритм разбиения на подмассивы и их сортировка
    for (vector<int>::iterator i = begin; i != end; i += run)
    {
        run = runLength(i, end, minrun);//определяем длину очередного подмассива run
        insertionsort(i, i + run);//сортируем его
        buffer.first = index;
        buffer.second = run;
        stek.push_back(buffer);
        index += run;
    }
    //Шаг 3. Слияние
    //for (int i = 0; i < stek.size(); ++i)
    //{
    //    cout << stek[i].first << ' ' << stek[i].second << endl;
    //}
    timsortMerge(stek, begin, end);
}
#else

//индексация должна быть с нуля, сортировать нужно интервал [begin, end)
void timsort(vector<int>& vector, int begin, int end) {

}

#endif

int testcaseNumber = 1;

bool testcase(const vector<int>& testVector,
    const vector<int>::const_iterator& begin,
    const vector<int>::const_iterator& end
) {
    try {
        vector<int> f(testVector), s(testVector);
        auto fl = f.begin() + distance(testVector.begin(), begin), fr = fl + distance(begin, end);
        auto sl = s.begin() + distance(testVector.begin(), begin), sr = sl + distance(begin, end);
        sort(fl, fr);
#ifdef ITER
        timsort(sl, sr);
#else
        timsort(s, distance(s.begin(), sl), distance(s.begin(), sr));
#endif
        cout << testcaseNumber++ << " testcase: ";
        if (equal(fl, fr, sl)) {
            cout << "Test passed" << endl;
            return true;
        }

        cout << "Something is wrong" << endl;
        auto iter = fl;
        cout << "Answer:      ";
        while (iter != fr) {
            cout << setw(4) << *iter << ' ';
            iter++;
        }
        cout << endl << "Your output: ";
        iter = sl;
        while (iter != sr) {
            cout << setw(4) << *iter << ' ';
            iter++;
        }
        cout << endl;
        return false;
    }
    catch (...) {
        cout << "exception >_<";
        return false;
    }
}

int main() {
    vector<int> testVector = { 1, 5, 2, 5, 7, 1, -1, 0, 0, 6, 2, 13, 6, 2, 6, 2, 3, 6, 34, 234, 234, 45, 63, 46, 345, 32,
                              5432, 34, 5, 4, 32, 53, 24, 6736, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    bool result = true;
    result &= testcase(testVector, testVector.begin(), testVector.end());
    result &= testcase(testVector, testVector.begin(), testVector.begin() + 3);
    result &= testcase(testVector, testVector.begin(), testVector.begin());
    result &= testcase(testVector, testVector.begin() + 5, testVector.begin() + 5 + 3);
    result &= testcase(testVector, testVector.begin() + 13, testVector.begin() + 13 + 10);
    result &= testcase(testVector, testVector.begin() + 34, testVector.begin() + 34 + 9);

    vector<int> newTestVector = { 1 };//тут можете добавлять свои тестовые данные
    //тесты пишите по аналогии с тем, как они написаны выше
    result &= testcase(newTestVector, newTestVector.begin(), newTestVector.end());


    cout << "Result: ";
    cout << ((result) ? "Your solution passed the pretests c:"
        : "Something's wrong, I can feel it. Just a feeling I've got, like something's about to happen, but I don't know what")
        << endl;
}