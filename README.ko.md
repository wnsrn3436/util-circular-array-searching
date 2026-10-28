# Circular Array Searching

[English](README.md)

격자 위에서 같은 그룹의 가장 가까운 오브젝트를 찾아 주는 게임메이커용 C++ DLL 이다. 화면을 일정 크기의 칸으로 나눠 각 칸에 오브젝트 하나를 기록해 두고, 가까운 칸부터 멀어지는 순서로 미리 정렬해 둔 오프셋 표를 따라 훑는다. 유닛이 많을 때 모든 쌍의 거리를 재지 않고도 표적을 고를 수 있다. DLL 과 게임메이커 확장 패키지, 동작을 보여주는 데모로 구성된다.

<p>
  <img src="docs/screenshots/screenshot-1.png" width="480" alt="데모 화면">
</p>


## 사용 방법

Releases에서 받은 압축 파일에는 게임메이커 8 확장 패키지 `cas.gex` 와 데모 프로젝트가 들어 있다. 확장 패키지를 설치하고 프로젝트에 포함하면 함수 넷을 쓸 수 있다.

| 함수 | 동작 |
|---|---|
| `cas_create(width, height, cellsize)` | 룸 크기와 칸 크기로 격자와 거리 표를 만든다. 이미 있으면 0 을 돌려준다 |
| `cas_update(xprevious, yprevious, x, y, group, id)` | 오브젝트를 이전 칸에서 지우고 지금 칸에 그룹과 id 로 기록한다 |
| `cas_search(x, y, id, range, group)` | 그 위치에서 `range` 안에 있는 `group` 오브젝트 중 가장 가까운 것의 id 를 돌려준다. 없으면 `noone` |
| `cas_destroy()` | 격자와 표를 지운다 |

구조는 한 번에 하나만 둘 수 있고, 한 칸에는 오브젝트 하나만 들어간다. 데모의 유닛은 스텝마다 이렇게 쓴다.

```gml
cas_update(xprevious, yprevious, x, y, group, id)

target = cas_search(x, y, id, range, group)

if target != noone {
  image_angle = point_direction(x, y, target.x, target.y)
}
```

데모는 1000x1000 룸에 20픽셀 칸으로 유닛 250개를 뿌리고 파랑과 빨강 그룹을 무작위로 정한다. 유닛마다 같은 그룹의 가장 가까운 유닛 쪽으로 돌아서고, 마우스를 화면 가장자리에 대면 뷰가 움직인다. 소스는 `CAS.dev` 를 wxDev-C++ 로 열거나 `Makefile.win` 으로 빌드한다.


## 구현 원리

**거리순 오프셋 표를 미리 만든다.** `cas_create` 는 거리의 제곱 `i` 를 1 부터 올리면서 `j² + u² = i` 를 만족하는 정수 쌍을 찾고, 찾은 쌍을 대칭 변환 일곱 개로 뒤집어 여덟 방향의 칸 오프셋으로 펼친다. 같은 거리의 오프셋을 한 링으로 묶고 링 앞에 개수와 거리를 적은 머리 항목을 둔다. 표 전체가 거리가 짧은 링부터 긴 링 순서로 놓이므로 탐색 순서가 곧 거리 순서가 된다.

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

**탐색은 표를 따라가다 처음 걸리는 칸에서 끝난다.** `cas_search` 는 질의 위치의 칸을 기준으로 표의 오프셋을 차례로 더해 가며 그 칸에 지정한 그룹의 오브젝트가 있는지 보다가 링 머리 항목의 거리에 칸 크기를 곱한 값이 `range` 에 이르면 멈춘다. 표에 원점 칸이 없어서 호출한 오브젝트 자신은 걸리지 않는다.

**칸 하나에 오브젝트 하나다.** 격자는 칸마다 그룹과 id 두 값만 갖고 `cas_update` 는 이전 칸이 아직 자기 것일 때만 비운 뒤 새 칸에 덮어쓴다. 게임메이커가 DLL 과 주고받는 값이 double 뿐이라 인자와 반환값을 전부 double 로 두었다.


## 파일

| 경로 | 내용 |
|---|---|
| `source/cpp/cas.cpp`, `source/cpp/export.h` | DLL 소스 |
| `source/cpp/CAS.dev`, `source/cpp/Makefile.win` | wxDev-C++ 프로젝트와 MinGW 메이크파일 |
| `source/gex/cas.ged` | 게임메이커 8 확장 패키지 정의 |
| `docs/screenshots/` | 스크린샷 |
| Releases | 확장 패키지와 데모 |


## 라이선스

zlib 라이선스다. 자세한 내용은 [LICENSE](LICENSE) 에 있다.
