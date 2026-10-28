# Circular Array Searching

[한국어](README.ko.md)

A C++ DLL for GameMaker that finds the nearest object of the same group on a grid. The room is divided into cells of a fixed size, each cell records one object, and a search walks an offset table that was sorted in advance from the nearest cell outward. With many units, it can pick a target without measuring the distance between every pair. It consists of the DLL, a GameMaker extension package, and a demo that shows it working.

<p>
  <img src="docs/screenshots/screenshot-1.png" width="480" alt="Demo screen">
</p>


## How to use

The archive downloaded from Releases contains the GameMaker 8 extension package `cas.gex` and the demo project. Install the extension package and include it in a project to get four functions.

| Function | Action |
|---|---|
| `cas_create(width, height, cellsize)` | Builds the grid and the distance table from the room size and the cell size. Returns 0 if one already exists |
| `cas_update(xprevious, yprevious, x, y, group, id)` | Removes the object from its previous cell and records its group and id in the current cell |
| `cas_search(x, y, id, range, group)` | Returns the id of the nearest `group` object within `range` of that position, or `noone` if there is none |
| `cas_destroy()` | Deletes the grid and the table |

Only one structure can exist at a time, and a cell holds only one object. Each unit in the demo calls them every step like this.

```gml
cas_update(xprevious, yprevious, x, y, group, id)

target = cas_search(x, y, id, range, group)

if target != noone {
  image_angle = point_direction(x, y, target.x, target.y)
}
```

The demo scatters 250 units on a 1000x1000 room with 20 pixel cells and assigns each to the blue or red group at random. Every unit turns toward the nearest unit of its own group, and the view moves when the mouse touches the edge of the screen. To build the source, open `CAS.dev` in wxDev-C++ or use `Makefile.win`.


## How it works

**An offset table sorted by distance is built in advance.** `cas_create` counts the squared distance `i` up from 1, finds integer pairs with `j² + u² = i`, and flips each pair through seven symmetry transforms to spread it into cell offsets in eight directions. Offsets at the same distance are grouped into one ring, and each ring starts with a header entry holding its count and distance. The whole table runs from the shortest ring to the longest, so search order and distance order are the same.

```cpp
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
                ...
                buffer->push_back(j);
                buffer->push_back(u);
                Table.push_back(buffer);
                ...
            }
            else{break;}
        }
    }
    ...
}
```

**A search follows the table and ends at the first cell that matches.** `cas_search` starts from the cell at the query position, adds the table's offsets to it one by one, and checks whether each resulting cell holds an object of the given group. It stops when the distance in a ring header times the cell size reaches `range`. The origin cell is not in the table, so the calling object never matches itself.

**One object per cell.** Each cell of the grid holds only two values, a group and an id. `cas_update` clears the previous cell only if it still belongs to the object, then overwrites the new cell. Since GameMaker exchanges only doubles with a DLL, every argument and return value is a double.


## Files

| Path | Content |
|---|---|
| `source/cpp/cas.cpp`, `source/cpp/export.h` | DLL source |
| `source/cpp/CAS.dev`, `source/cpp/Makefile.win` | wxDev-C++ project and MinGW makefile |
| `source/gex/cas.ged` | GameMaker 8 extension package definition |
| `docs/screenshots/` | Screenshots |
| Releases | Extension package and demo |


## License

zlib. See [LICENSE](LICENSE).
