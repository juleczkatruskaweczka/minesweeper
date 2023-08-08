#include "MineField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"

void MineField::Tile::SpawnMines()
{
    assert(!hasBomb);
    hasBomb = true;
}

bool MineField::Tile::HasMine()
{
    return hasBomb;
}

void MineField::Tile::Draw(const Vei2& screenPos, Graphics& gfx) const
{
    switch (state) {
    case State::Hidden:
        SpriteCodex::DrawTileButton(screenPos,gfx);
        break;
    case  State::Flagged:
        SpriteCodex::DrawTileButton(screenPos, gfx);
        SpriteCodex::DrawTileFlag(screenPos, gfx);
        break;
    case  State::Revealed:
        if (!hasBomb) {
            SpriteCodex::DrawTile0(screenPos, gfx);
        }
        else {
            SpriteCodex::DrawTileBomb(screenPos, gfx);
        }
        break;
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


MineField::MineField(int nMines)
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
    for (int i = 0; i < 120; i++) {
        Vei2 gridPos = { xDist(rng),yDist(rng) };
        if (!TileAt(gridPos).IsRevealed()) {
            TileAt(gridPos).Reveal();
        }
    }  
}

void MineField::Draw(Graphics& gfx) const
{
    gfx.DrawRect(GetRect(),SpriteCodex::baseColor);
    for (Vei2 gridPos {0, 0}; gridPos.y<height;gridPos.y++){
        for (gridPos.x=0; gridPos.x < width; gridPos.x++) {
            TileAt(gridPos).Draw(gridPos*SpriteCodex::tileSize,gfx);
            }
    }
}

RectI MineField::GetRect() const
{
    return RectI(0,width*SpriteCodex::tileSize,0,height* SpriteCodex::tileSize);
}

void MineField::RevealClick(const Vei2& screenPos)
{
    const Vei2 gridPos = ScreenToGrid(screenPos);
    assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);

    Tile& tile = TileAt(gridPos);
    if (!tile.IsRevealed() && !tile.IsFlagged()) {
        tile.Reveal();
    }

}

void MineField::FlagClick(const Vei2& screenPos)
{
    const Vei2 gridPos = ScreenToGrid(screenPos);
    assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
    Tile& tile = TileAt(gridPos);
    if (!tile.IsFlagged()) {
        tile.ToggleFlag();
    }

}

Vei2 MineField::ScreenToGrid(const Vei2& screenPos)
{
    return screenPos/SpriteCodex::tileSize;
}

MineField::Tile& MineField::TileAt(const Vei2& gridPos)
{
    return field[gridPos.y * width + gridPos.x ];
}

const MineField::Tile& MineField::TileAt(const Vei2& gridPos) const
{
    return field[gridPos.y * width + gridPos.x];
}
