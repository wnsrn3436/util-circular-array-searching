#include "export.h"
#include <windows.h>
#include <math.h>
#include <vector>

using namespace std;
vector< vector<double>* > Table;

INT8 Check = 0, ItTable[][4] = {{0, 1, 1, 0}, {0, -1, 1, 0}, {-1, 0, 0, 1}, {-1, 0, 0, -1}, {0, -1, -1, 0}, {0, 1, -1, 0}, {1, 0, 0, -1}};
double CellSize, ***MapDist;
int hCells, vCells, CountTable = 0;

///////////////////////////////////////////////////////////////////////////////////////////////

export double cas_create(double width, double height, double cellsize)
{
    if(!Check)
    {
        CellSize = cellsize;
        hCells = ceil(width/cellsize), vCells = ceil(height/cellsize);
        
        int i, j, k, u, pivot, gcnt = 0, lcnt = 0, ipos = 0, radius = floor((hCells*hCells + vCells*vCells)/cellsize);
        
        MapDist = new double**[hCells];
        for(i=0; i<hCells; i++)
        {
            MapDist[i] = new double*[vCells];
            for(j=0; j<vCells; j++)
            {
                MapDist[i][j] = new double[2];
                MapDist[i][j][0] = -4;
                MapDist[i][j][1] = -4;
            }
        }
        //MapDist Create
        
        vector<double> *buffer = new vector<double>;
        buffer->resize(2);
        Table.push_back(buffer);
        gcnt++;
        
        for(i=1; i<radius; i++)
        {
            for (j=floor(sqrt(i)); j>=0; j--)
            {
                k = i-j*j;
                u = floor(sqrt(k));
                if(u*u == k)
                {
                    if(j >= u)
                    {
                        vector<double> *buffer = new vector<double>;
                        buffer->reserve(2);
                        buffer->push_back(j);
                        buffer->push_back(u);
                        Table.push_back(buffer);
                        lcnt++;
                        gcnt++;
                    }
                    else{break;}
                }
            }
            
            if(lcnt == 0) continue;
            
            pivot = lcnt;
            for(j=0; j<7; j++)
            {
                for(k=ipos+pivot; k>ipos; k--)
                {
                    if((j & 1) == 0 and (*Table[k]->begin() == *(Table[k]->end()-1) or *(Table[k]->end()-1) == 0)){continue;}
                    double tempx, tempy;
                    tempx = ItTable[j][0] * (*Table[k]->begin()) + ItTable[j][1] * (*(Table[k]->end()-1));
                    tempy = ItTable[j][2] * (*Table[k]->begin()) + ItTable[j][3] * (*(Table[k]->end()-1));
                    vector<double> *buffer = new vector<double>;
                    buffer->reserve(2);
                    buffer->push_back(tempx);
                    buffer->push_back(tempy);
                    Table.push_back(buffer);
                    lcnt++;
                    gcnt++;
                }
            }
            
            *Table[ipos]->begin() = lcnt;
            *(Table[ipos]->end()-1) = sqrt(i);
            
            vector<double> *buffer = new vector<double>;
            buffer->resize(2);
            Table.push_back(buffer);
            
            lcnt = 0;
            ipos = gcnt++;
        }
        //Table Create
        
        Check = 1;
        CountTable = gcnt;
        return 1;
    }
    return 0;
}

export double cas_destroy()
{
    if(Check)
    {
        int i, j;
        
        for(i=0; i<hCells; i++)
        {
            for(j=0; j<vCells; j++)
            {
                delete [] MapDist[i][j];
            }
            delete [] MapDist[i];
        }
        delete [] MapDist;
        // MapDist Free
        
        for(i=0; i<CountTable; i++)
        {
            delete Table[i];
        }
        Table.clear();
        //Table Free
        
        CountTable = 0;
        Check = 0;
        return 1;
    }
    return 0;
}

export double cas_update(double xprevious, double yprevious, double x, double y, double group, double id)
{
    if(Check)
    {
        int hpos = floor(xprevious/CellSize), vpos = floor(yprevious/CellSize);
        
        if(MapDist[hpos][vpos][0] == group and MapDist[hpos][vpos][1] == id)
        {
            MapDist[hpos][vpos][0] = -4;
            MapDist[hpos][vpos][1] = -4;
        }
        
        hpos = floor(x/CellSize);
        vpos = floor(y/CellSize);
        MapDist[hpos][vpos][0] = group;
        MapDist[hpos][vpos][1] = id;
        return 1;
    }
    return 0;
}

export double cas_search(double x, double y, double id, double range, double group)
{
    if(Check)
    {
        double target = -4;
        int cnt = *Table[0]->begin(), radius = *(Table[0]->end()-1), hpos = floor(x/CellSize), vpos = floor(y/CellSize), i, hval, vval;
        
        for(i=1; radius*CellSize<range; i++)
        {
            if(cnt != 0)
            {
                hval = *Table[i]->begin() + hpos;
                vval = *(Table[i]->end()-1) + vpos;
                if(hval>=0 and hval<hCells and vval>=0 and vval<vCells)
                {
                    if(MapDist[hval][vval][0] == group and MapDist[hval][vval][1] != -4)
                    {
                        target = MapDist[hval][vval][1];
                        break;
                    }
                }
                cnt--;
            }
            else
            {
                cnt = *Table[i]->begin();
                radius = *(Table[i]->end()-1);
            }
        }
        return target;
    }
    return -4;
}
