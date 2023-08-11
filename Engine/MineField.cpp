#include "MineField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include <algorithm>

void MineField::Tile::SpawnMines()
{
    assert(!hasBomb);
    hasBomb = true;
}

bool MineField::Tile::HasMine()
{
    return hasBomb;
}

void MineField::Tile::Draw(const Vei2& screenPos,bool ended, Graphics& gfx) const
{
    if (!ended) {
        switch (state) {
        case State::Hidden:
            SpriteCodex::DrawTileButton(screenPos, gfx);
            break;
        case  State::Flagged:
            SpriteCodex::DrawTileButton(screenPos, gfx);
            SpriteCodex::DrawTileFlag(screenPos, gfx);
            break;
        case  State::Revealed:
            if (!hasBomb) {
                SpriteCodex::DrawTileNumbers(screenPos, nNeighbourMines, gfx);
            }
            else {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
            }
            break;
        }
    }
    else {
        switch (state) {
        case State::Hidden:
            if (hasBomb) {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
            }
            else {
                SpriteCodex::DrawTileButton(screenPos, gfx);
            }
            break;
        case  State::Flagged:
            if (hasBomb) {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
                SpriteCodex::DrawTileFlag(screenPos, gfx);
            }
            else {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
                SpriteCodex::DrawTileCross(screenPos, gfx);
            }
            break;
        case  State::Revealed:
            if (!hasBomb) {
                SpriteCodex::DrawTileNumbers(screenPos, nNeighbourMines, gfx);
            }
            else {
                SpriteCodex::DrawTileBomb(screenPos, gfx);
            }
            break;
        }
    }
}

void MineField::Tile::Reveal()
{
    assert(state == State::Hidden);
    state = State::Revealed;
}

bool MineField::Tile::IsRevealed() const
{
    return state==State::Revealed;
}

void MineField::Tile::ToggleFlag()
{
    assert(!IsRevealed());
        if (state==State::Hidden)
            state = State::Flagged;
        else state = State::Hidden;
}

bool MineField::Tile::IsFlagged() const
{
    return state == State::Flagged;
}

void MineField::Tile::SetMinesCount(int mineCount)
{
    assert(nNeighbourMines == -1);
    nNeighbourMines = mineCount;
}


MineField::MineField(const Vei2& center, int nMines)
    :
    topCorner(center - Vei2(width * SpriteCodex::tileSize, height * SpriteCodex::tileSize)/2 )
{
    assert(nMines > 0 && nMines < width + height);
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> xDist(0, width - 1);
    std::uniform_int_distribution<int> yDist(0, height - 1);

    for (int nSpawned = 0; nSpawned < nMines; nSpawned++) {
        Vei2 spawnPos;
        do
        {
            spawnPos = { xDist(rng), yDist(rng) };
        } while (TileAt(spawnPos).HasMine());

        TileAt(spawnPos).SpawnMines();
    }
    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++) {
        for (gridPos.x=0; gridPos.x < width; gridPos.x++) {

            TileAt(gridPos).SetMinesCount(CountNeighbourMines(gridPos));
        }
    }
  
}

void MineField::Draw(Graphics& gfx) const
{
    gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
    for (Vei2 gridPos = {0, 0}; gridPos.y < height; gridPos.y++) {
        for (gridPos.x=0; gridPos.x <width; gridPos.x++) {
            TileAt(gridPos).Draw(topCorner +(gridPos*SpriteCodex::tileSize),isEnded,gfx);
            }
    }
}

RectI MineField::GetRect() const
{
    return RectI(topCorner,width*SpriteCodex::tileSize,height* SpriteCodex::tileSize);
}

void MineField::RevealClick(const Vei2& screenPos)
{
    if (!isEnded) {
        const Vei2 gridPos = ScreenToGrid(screenPos);
        assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

        Tile& tile = TileAt(gridPos);
        if (!tile.IsRevealed() && !tile.IsFlagged()) {
            tile.Reveal();

            if (tile.HasMine()) {
                isEnded = true;
            }
        }
    }
}

void MineField::FlagClick(const Vei2& screenPos)
{
    if (!isEnded) {
        const Vei2 gridPos = ScreenToGrid(screenPos);
        assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
        Tile& tile = TileAt(gridPos);
        if (!tile.IsFlagged()) {
            tile.ToggleFlag();
        }
    }
}

Vei2 MineField::ScreenToGrid(const Vei2& screenPos)
{
    return (screenPos-topCorner)/SpriteCodex::tileSize;
}

int MineField::CountNeighbourMines(const Vei2& gridPos)
{
    const int xStart = std::max(0, gridPos.x - 1);
    const int yStart = std::max(0, gridPos.y - 1);
    const int xEnd = std::min(width-1, gridPos.x + 1);
    const int yEnd = std::min(height-1, gridPos.y + 1);

    int count = 0;
    for (Vei2 gridPos = { xStart,yStart }; gridPos.y <= yEnd; gridPos.y++) {
        for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++) {

            if (TileAt(gridPos).HasMine()) {
                count++;
            }
        }
    }
    return count;
}

MineField::Tile& MineField::TileAt(const Vei2& gridPos)
{
    return field[gridPos.y * width + gridPos.x ];
}

const MineField::Tile& MineField::TileAt(const Vei2& gridPos) const
{
    return field[gridPos.y * width + gridPos.x];
}
