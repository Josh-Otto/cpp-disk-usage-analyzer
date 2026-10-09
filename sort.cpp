#include "sort.h"
#include "Folder.h"
#include <vector>
#include <algorithm> // for std::swap
#include <filesystem>

using namespace std;


void pleaseSortThisBySizeASC(vector<Folder>& folders)
{
    for(auto a = folders.begin(); a != folders.end(); ++a)
    {
        for(auto b = folders.begin(); b != folders.end() - 1; ++b)
        {
            if(b->size > (b + 1)->size)
            {
                swap(*b,*(b + 1));
            }
        }
    }
}