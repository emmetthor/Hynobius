# engine improvement

| 日期 | 初始局面深度 6 所需時間 | 精度 | Negamax 節點數 | QuietScence 節點數 | 功能改動 |
| - | - | - | - | - | - |
| 2026/2/3 之前 | $\approx 2 s$ | $50 /100$ | $\approx 70000$ | $\approx 50000$ | $\alpha \beta\ Search$+QuietScence |
| 2026/2/3 | $\approx 5 s$ | $70/100$ | $125636$ | $108325$ | 加入 Mobility |
| 2026/2/5 | 初始局面 $1560ms$ | 未測量 | $39917$ | $31046$ | 加入 `TT` |
| 2026/2/10 |初始局面 $3350ms$，中盤 $10s$ | 未測量 | $428791$ | $417649$ | 還原成正確版 $\text{Negamax}$ |
| 2026/2/10 | 初始局面 $3350ms$，中盤 $10s$ | 未測量 | $471392$ | $414627$ | 加入 $\text{Delta Pruning}$ |
| 2026/2/10 | 初始局面 $527ms$，中盤 $2s$ | 未測量 | $51745$ | $41488$ | 加入 $\text{LMR}$ |
| 2026/2/15 | 初始局面 $851ms$，中盤 $5s$ | 未測量 | $70665$ | $55754$ | 加入 $\text{aspiration window}$，並在 QS 內使用完整分析 |

從 2026/2/18 開始，統一以 `position startpos moves d2d4 g8f6 c1f4 d7d6 e2e3 c8g4 g1f3 b8c6 b1d2` 此位置為準，與 `go depth 6` 來測量以下數據。

| 日期 | 總節點數 | QS節點數 | 耗時 | 每秒節點數 | 備註 |
| - | - | - | - | - | - |
| 2026/2/18 | $191563$ | $101219$ | $1244ms$ | $153989$ | 加入 $\text{Killer move}$ 紀錄，並以相同中盤為基準測量。 |

From 4/11, engine improvement is measured using A/B test. Please see [TODO] to check engine strength.
