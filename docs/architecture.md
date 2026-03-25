# Archetecture

## Purpose

## Modules

### Piece

Contains:
- Check a piece is white or black.
- Check whether two pieces share the same color.
- Transform functions between `Piece` and `char`.

### Board

Contains:
- 8x8 board array.
- every pieces' positions (`piecePos[][]`).
- every pieces' count on the board (`pieceCnt[]`).
- material score.
- PST score.
- side to play (`player`).
- castling rights.
- zobrist key for the current board.

### Attack

Contains:
- check a square is either attacked or not.
- count attacks in a square.