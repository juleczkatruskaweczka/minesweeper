#pragma once
#include "Graphics.h"

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
		void Draw(const Vei2& screenPos, Graphics& gfx) const;
	private: 
		State state = State::Hidden;
		bool hasBomb = false;
};
public:
	MineField(int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width* height];
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;

};