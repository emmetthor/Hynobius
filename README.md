<div>
  <button> test </button>
</div>
# chess-analysis-engine

這是一個以**二維陣列**實作的教學型西洋棋引擎，支援 `PGN` 輸入，並以網頁介面呈現結果 (正在實作中)。

## 特色

- 二維棋盤，稍慢但易懂
- Move generation 具備 pseudo-legal 性質。 (也就是生成時就已經保證吃子與落點正確，降低時間複雜度)
- $\alpha \beta$ Search + Quiescence (減緩地平線效應)。
- Zobrist hashing + Transportation Table (大型 dp 記憶化搜索)
- PGN parser
- debug system

## 文檔

完整說明與實作方式請見：
### [總覽 Documention Index](docs/project-overview.md)
