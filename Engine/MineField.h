#pragma once
#include "Graphics.h"
#include "SpriteCodex.h"

class MineField {
private: 
	class Tile {
	public:
		enum class State {
			Hidden,
			Flagged,
			Revealed
		};
		void SpawnMines();
		bool HasMine();
		void Draw(const Vei2& screenPos,bool ended, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SetMinesCount(int mineCount);
	private: 
		State state = State::Hidden;
		bool hasBomb = false;
		int nNeighbourMines = -1;
};
public:
	MineField(const Vei2& center,int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void RevealClick(const Vei2& screenPos);
	void FlagClick(const Vei2& screenPos);
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Vei2 topCorner;
	Tile field[width* height];
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
	int CountNeighbourMines(const Vei2& gridPos);
	bool isEnded = false;

};